#include "ExpenseCategoryRulesFinder.h"
#include <math.h>

#ifndef _WIN32
	#define sprintf_s snprintf
#endif

//=================================================================================================
ExpenseCategoryRulesFinder::ExpenseCategoryRulesFinder(Database & db, double min_support, double min_confidence)
	:	db(db), min_support(min_support), min_confidence(min_confidence), 
		transact_table(NULL), transactions_size(0), logger("logs.txt")
{	
	db.getTransactions(transactions);
	prepareTransactTable();

	char log[101];
	sprintf_s(log, 100, "minimum support: %.3f, minimum confidence: %.3f\n\n", min_support, min_confidence);
	logger.log(log);
	logger.log(transactions);
}
//=================================================================================================
ExpenseCategoryRulesFinder::~ExpenseCategoryRulesFinder()
{
	cleanUp();
}
//=================================================================================================
void ExpenseCategoryRulesFinder::cleanUp()
{	
	if (transact_table)
	{
		for (int i = 0; i < transactions_size; i++)
			delete[] transact_table[i];
		delete[] transact_table;

		transact_table = NULL;
		transactions_size = 0;
	}
}
//=================================================================================================
int ExpenseCategoryRulesFinder::expenseCategoryToIndex(ExpenseCategory & category)
{
	int status_offset;

	switch(category.status)
	{
	case eLowStatus:
		status_offset = 0;
		break;
	case eMediumStatus:
		status_offset = 1;
		break;
	case eHighStatus:
		status_offset = 2;
		break;
	}

	return categoryToIndex[category.expense_category_id] * sizeof(expenseCategoryStatuses) + status_offset;
}
//=================================================================================================
ExpenseCategoriesSets ExpenseCategoryRulesFinder::flattenFrequent(vector<ExpenseCategoriesSets> & sets)
{
	ExpenseCategoriesSets result;
	if (sets.empty())
		return result;

	result = sets.front();																	//copy first set

	ExpenseCategoriesSets::iterator j;
	for (vector<ExpenseCategoriesSets>::iterator i = sets.begin()+1; i != sets.end(); i++)	//get each list
		for (j = i->begin(); j != i->end(); j++)											//get each set
			result.push_back(*j);															//add to first list

	return result;
}
//=================================================================================================
bool ExpenseCategoryRulesFinder::areCategoriesInTransaction(int transaction_index, vector<ExpenseCategory> & categories)
{
	if (categories.size() == 0)
		return false;

	unsigned j;
	for (j = 0; j < categories.size(); j++)
		if (!transact_table[transaction_index][expenseCategoryToIndex(categories[j])])
			return false;

	return true;
}
//=================================================================================================
bool ExpenseCategoryRulesFinder::hasMinSupport(ExpenseCategoriesSet & set)
{
	int support = 0;

	for(int i = 0; i < transactions_size; i++)
	{
		if (areCategoriesInTransaction(i, set.categories) && ((double)++support / (double)transactions_size >= min_support))
			return true;
		else if (double(support + transactions_size - i - 1) / double(transactions_size) < min_support)
			return false; //if it is impossible to reach minimum support, then we can break
	}

	return false;
}
//=================================================================================================
ExpenseCategoriesSets ExpenseCategoryRulesFinder::generateCandidates(ExpenseCategoriesSets & sets)
{
	ExpenseCategoriesSets result;
	ExpenseCategoriesSet candidate;

	unsigned j;
	logger.log("Candidates: {\n");
	for (unsigned i = 0; i < sets.size(); i++)
		for (j = i + 1; j < sets.size(); j++)
		{
			//we don't want to join sets with the same categories at the end, because they cannot exist together in one rule
			if (sets[i].categories.back().expense_category_id == sets[j].categories.back().expense_category_id)
				continue;

			if (sets[i].equalsWithoutLast(sets[j]))
			{
				candidate = sets[i].joinWith(sets[j]);
				if (hasMinSupport(candidate))	
				{
					logger.log(candidate, true);
					result.push_back(candidate);
				}
				else
					logger.log(candidate, false);
			}
		}
	logger.log("}\n");

	return result;
}
//=================================================================================================
void ExpenseCategoryRulesFinder::prepareTransactTable()
{
	//map ids to values [0;ids_count-1] - to avoid too large transact_table
	int index = 0;
	vector<ExpenseCategory> categories = db.getCategories();
	for(vector<ExpenseCategory>::iterator cat = categories.begin(); cat != categories.end(); cat++)
		categoryToIndex[cat->expense_category_id] = index++;

	//transact_table is used to check if a candidate has a minimum support
	
	//we just have to check transact_table[transaction][category_index] 
	//to know if a category is in a specific transaction
	//using this structure access to this information is O(1) 

	int transact_table_size =  index * sizeof(expenseCategoryStatuses);
	transactions_size = transactions.size();

	//allocate structure
	transact_table = new bool* [transactions_size];
	for (int i = 0; i < transactions_size; i++)
	{
		transact_table[i] = new bool[transact_table_size];
		for (int j = 0; j < transact_table_size; j++)
			transact_table[i][j] = false;
	}

	//fill structure
	for (int i = 0; i < transactions_size; i++)
		for (unsigned j = 0; j < transactions[i].categories.size(); j++)
			transact_table[i][expenseCategoryToIndex(transactions[i].categories[j])] = true;
}
//=================================================================================================
void ExpenseCategoryRulesFinder::removeRulesWithLowConfidence(Rules & rules)
{
	vector<Rules::iterator> toRemove;
	int rule_support, cond_support, trans_index;
	int remain_transactions;

	logger.log("Rules: {\n");
	for(Rules::iterator rule = rules.begin(); rule != rules.end(); rule++)
	{
		rule_support = 0;
		cond_support = 0;
		trans_index  = 0;

		//count support
		for(Transactions::iterator trans = transactions.begin(); trans != transactions.end(); trans++, trans_index++)
		{
			//if it is impossible to reach minimum confidence, then we can break
			remain_transactions = transactions.size() - trans_index;
			if (double(remain_transactions + rule_support) / double(remain_transactions + cond_support) < min_confidence)
				break;

			if (areCategoriesInTransaction(trans_index, rule->conditions))
			{
				cond_support++;

				if (areCategoriesInTransaction(trans_index, rule->implications))
					rule_support++;
			}
		}

		//check confidence
		if ((double)rule_support / (double)cond_support < min_confidence)
		{
			logger.log(*rule, false);
			toRemove.push_back(rule);
		}
		else
			logger.log(*rule, true);
	}
	logger.log("}\n");

	//remove rules with low confidence
	for(int i = toRemove.size() - 1; i >= 0; i--)
		rules.erase(toRemove[i]);
}
//=================================================================================================
ExpenseCategoriesSets ExpenseCategoryRulesFinder::apriori()
{
	/* frequent variable structure:
	 *
	 * -- Candidates (i-th candidate contains i categories)
	 * ---- categories
	 * ------ expense_category_id
	 * ------ status;
	 *
	 */
	
	ExpenseCategoriesSets candidates;
	vector<ExpenseCategoriesSets> frequent;

	// Apriori:
	frequent.push_back(db.getFrequentOneElementSets(min_support));

	logger.log("Candidates: {\n");
	ExpenseCategoriesSets frequent1Elements = frequent.back();
	for (unsigned i = 0; i < frequent1Elements.size(); i++)
		logger.log(frequent1Elements[i], true);
	logger.log("}\n");

	while (true)
	{
		candidates = generateCandidates(frequent.back());

		if (candidates.empty())
			break;

		frequent.push_back(candidates);
	}

	// Remove sets with single elements 
	frequent.erase(frequent.begin());	

	return flattenFrequent(frequent);
}
//=================================================================================================
Rules ExpenseCategoryRulesFinder::generateRules(ExpenseCategoriesSets & frequentSets)
{
	Rules rules;
	int rule_id = 1;
	for(ExpenseCategoriesSets::iterator set = frequentSets.begin(); set != frequentSets.end(); set++)
	{
		int offset;
		int cat_count = set->categories.size();
		unsigned long long bitCount = (unsigned long long)pow(2, set->categories.size());

		//generates all combinations - we split frequent set to 1 - condition, 0 - implication
		for(unsigned long long i = 1; i < bitCount - 1; i++, rule_id++)
		{
			Rule * rule = new Rule();
			for (offset = 0; offset < cat_count; offset++)
			{
				ExpenseCategory cat = set->categories[offset];

				if (i & (1ULL << offset))					
					rule->conditions.push_back(cat);
				else
					rule->implications.push_back(cat);
			}
			rules.push_back(*rule);
		}
	}

	return rules;
}
//=================================================================================================
ExpenseCategoriesSets ExpenseCategoryRulesFinder::execute()
{
	printf("Number of transactions: %d\n", transactions.size());
	return apriori();
}
//=================================================================================================
