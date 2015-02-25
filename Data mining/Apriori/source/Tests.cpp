#include "Tests.h"
#include <time.h>
#include <iostream>
#include <sstream>

//=================================================================================================
void startFixedTests(Database & db, double min_support, double min_confidence)
{
	Rule rule;
	RuleCondition cond;
	RuleImplication implic;
	/*cond.expense_category_id = 1;
	cond.status = eLowStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 2;
	cond.status = eMediumStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 3;
	cond.status = eHighStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 6;
	cond.status = eHighStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 7;
	cond.status = eHighStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 8;
	cond.status = eHighStatus;
	rule.conditions.push_back(cond);
	implic.expense_category_id = 4;
	implic.status = eLowStatus;
	rule.implications.push_back(implic);
	implic.expense_category_id = 5;
	implic.status = eLowStatus;
	rule.implications.push_back(implic);*/

	cond.expense_category_id = 1;
	cond.status = eLowStatus;
	rule.conditions.push_back(cond);
	cond.expense_category_id = 3;
	cond.status = eHighStatus;
	rule.conditions.push_back(cond);
	implic.expense_category_id = 2;
	implic.status = eMediumStatus;
	rule.implications.push_back(implic);

	generateTestUserExpenses(db, rule, min_support, min_confidence);
}
//=================================================================================================
void generateTestUserExpenses(Database & db, Rule rule, double min_support, double min_confidence)
{
	if (!db.getIsConnected())
	{
		printf("No connection with database.\n");
		return;
	}

	const int USERS = 1000;
	const int MONTHS = 10;
	const int CATEGORIES = 10;
	const int TRANSACTIONS = USERS * MONTHS;
    
    //insert test user expenses
    string cmd = "DELETE FROM user_expenses; INSERT INTO user_expenses (user_id, month, year, category, status) VALUES ";
	stringstream ss;
	double condition_support;
	int users_in_system = 0;

	condition_support = min_support / min_confidence;

	//fill transactions which will fulfill whole rule
	for(vector<RuleImplication>::iterator implic = rule.implications.begin(); implic != rule.implications.end(); implic++)
	{
		int month = 0, user = 0;
		for(int i = 0; i <= TRANSACTIONS * min_support; i++, month++)
		{
			month %= MONTHS;
			if (month == 0 && i > 0)
				user++;
			user %= USERS;

			ss.str("");
			ss << "(" << user + 1 << ", " << month + 1 << ", " << 2013 << ", " << implic->expense_category_id << ", '" << (char)implic->status << "'), ";
			cmd += ss.str();
		}
	}

	//fill transactions which will fulfill conditions
	for(vector<RuleCondition>::iterator cond = rule.conditions.begin(); cond != rule.conditions.end(); cond++)
	{
		int month = 0, user = 0;
		for(int i = 0; i <= TRANSACTIONS * condition_support; i++, month++)
		{				
			month %= MONTHS;
			if (month == 0 && i > 0)
			{
				users_in_system++;
				user++;
			}
			user %= USERS;				

			ss.str("");
			ss << "(" << user + 1 << ", " << month + 1 << ", " << 2013 << ", " << cond->expense_category_id << ", '" << (char)cond->status << "'), ";
			cmd += ss.str();
		}			
	}

	//fill remaining transactions
	int user = users_in_system + 1;
	int remains = TRANSACTIONS - (int)(TRANSACTIONS * condition_support);
	for(int i = 0; i < remains; i++, user++)
	{
		ss.str("");
		ss << "(" << user + 1 << ", " << 1 << ", " << 2013 << ", " << CATEGORIES << ", 'L'), ";
		cmd += ss.str();
	}

	cmd = cmd.substr(0, cmd.length() - 2);
			
    if (PQresultStatus(db.execSql(cmd.c_str())) != PGRES_COMMAND_OK)
	{
        printf("Error while adding test user expenses: %s\n", db.getError());
		return;
	}
}
//=================================================================================================
void generateRandomData(Database & db)
{
	const int USERS = 1000;
	const int CATEGORIES = 9;
	const int MONTHS = 12;

	if (!db.getIsConnected())
	{
		printf("No connection with database.\n");
		return;
	}

    stringstream ss;
    srand((unsigned)time(NULL));



    //insert test transactions
    string cmd = "DELETE FROM transactions; INSERT INTO transactions (date, amount, category, user_id) VALUES ";

    for (unsigned u = 0; u < USERS; u++) //user
        for (int m = 0; m < MONTHS; m++)      //month
            for (int k = 0; k < CATEGORIES; k++)  //category
            {
                ss.str("");
                ss << "(NOW() - interval '" << m + 1 << " month" << (m > 0 ? "s" : "") << "', ";
				cmd += ss.str();

                ss.str("");
                ss << -(rand() % 2000 + 1) << ", " << k + 1 << ", " << u << ")";
                cmd += ss.str();

                if (u != USERS - 1 || m != MONTHS - 1 || k != CATEGORIES - 1)
                    cmd += ", ";
            }

	PGresult* result = db.execSql(cmd.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while adding test transactions: %s\n", db.getError());
		return;
	}
}
//=================================================================================================
