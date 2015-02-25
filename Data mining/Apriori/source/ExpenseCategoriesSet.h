#ifndef _EXPENSE_CATEGORIES_SET_H_
#define _EXPENSE_CATEGORIES_SET_H_

#include "Common.h"

class ExpenseCategoriesSet;
typedef vector<ExpenseCategoriesSet> ExpenseCategoriesSets;

typedef ExpenseCategoriesSet Transaction;
typedef vector<Transaction> Transactions;


class ExpenseCategoriesSet
{
public:
	vector<ExpenseCategory> categories;

	short compare(const ExpenseCategoriesSet & set) const;
	bool equalsWithoutLast(const ExpenseCategoriesSet & set);
	ExpenseCategoriesSet joinWith(const ExpenseCategoriesSet & set);
};

#endif

