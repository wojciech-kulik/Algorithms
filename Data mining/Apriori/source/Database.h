#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <vector>
#include <string>
#include <libpq-fe.h>
#include "ExpenseCategoriesSet.h"

using namespace std;

class Database
{
protected:
    bool isConnected;
	PGconn *conn;
	void createTables();

public:
    Database(const char * connectionString);
	Database();
	~Database();

	bool connect(const char * connectionString);
	void disconnect();
	bool getIsConnected() { return isConnected; }

	PGresult * execSql(string sql);
	char * getError();

	ExpenseCategoriesSets getFrequentOneElementSets(double min_support);
	void getTransactions(Transactions & transactions);
	vector<ExpenseCategory> getCategories();
	void insertRulesToDB(Rules & rules);
};

#endif
