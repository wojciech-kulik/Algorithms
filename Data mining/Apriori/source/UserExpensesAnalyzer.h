#ifndef _USER_EXPENSES_ANALYZER_H_
#define _USER_EXPENSES_ANALYZER_H_

#include <stdio.h>
#include <vector>
#include "Common.h"
#include "Database.h"


using namespace std;


class UserExpensesAnalyzer
{
protected:
    Database & db;

public:
    UserExpensesAnalyzer(Database & db);
	void updateUsersExpenses();
};

#endif
