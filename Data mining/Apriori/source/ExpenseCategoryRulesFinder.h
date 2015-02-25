#ifndef _EXPENSE_CATEGORY_RULES_FINDER_H_
#define _EXPENSE_CATEGORY_RULES_FINDER_H_

#include "Common.h"
#include "Database.h"
#include "ExpenseCategoriesSet.h"
#include "Logger.h"
#include <map>


class ExpenseCategoryRulesFinder
{
private:
	map<int, int> categoryToIndex;
	bool ** transact_table;
	int transactions_size;
	Transactions transactions;

	void					cleanUp();
	void					prepareTransactTable();
	int						expenseCategoryToIndex(ExpenseCategory & category);
	bool					areCategoriesInTransaction(int transaction_index, vector<ExpenseCategory> & categories);

	ExpenseCategoriesSets	flattenFrequent(vector<ExpenseCategoriesSets> & sets);
	ExpenseCategoriesSets	generateCandidates(ExpenseCategoriesSets & sets);
	bool					hasMinSupport(ExpenseCategoriesSet & set);

	ExpenseCategoriesSets	apriori();	

protected:
	const double min_support;
	const double min_confidence;
	Database & db;
	Logger logger;

public:
	ExpenseCategoryRulesFinder(Database & db, double min_support, double min_confidence);
	~ExpenseCategoryRulesFinder();

	void removeRulesWithLowConfidence(Rules & rules);
	Rules generateRules(ExpenseCategoriesSets & frequentSets);
	ExpenseCategoriesSets execute();
};


#endif
