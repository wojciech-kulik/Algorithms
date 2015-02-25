#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef unsigned char _month;
typedef unsigned int _year;

enum ExpenseCategoryStatus
{
	eLowStatus	  = 'L',
	eMediumStatus = 'M',
	eHighStatus   = 'H'
};

const char expenseCategoryStatuses[3] = { eLowStatus, eMediumStatus, eHighStatus };

struct ExpenseCategory
{
	int expense_category_id;
	ExpenseCategoryStatus status;
	_month month;
	_year year;

	bool operator< (const ExpenseCategory &ec) const { return expense_category_id < ec.expense_category_id || (expense_category_id == ec.expense_category_id && status < ec.status); }
	bool operator> (const ExpenseCategory &ec) const { return expense_category_id > ec.expense_category_id || (expense_category_id == ec.expense_category_id && status > ec.status); }
	bool operator==(const ExpenseCategory &ec) const { return expense_category_id == ec.expense_category_id && status == ec.status; }
	bool operator!=(const ExpenseCategory &ec) const { return expense_category_id != ec.expense_category_id || status != ec.status; }
};

typedef ExpenseCategory RuleCondition;
typedef ExpenseCategory RuleImplication;
struct Rule
{
	vector<RuleCondition> conditions;
	vector<RuleImplication> implications;
};
typedef vector<Rule> Rules;


#endif
