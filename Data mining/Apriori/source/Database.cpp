#include "Database.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>

//=================================================================================================
void debug(Transactions & rules)
{
    printf("{\n");
    for (unsigned i = 0; i < rules.size(); i++)
    {
        printf("{");
        for(unsigned j = 0; j < rules[i].categories.size(); j++)
        {
            printf("%d %d %d %c", rules[i].categories[j].month, rules[i].categories[j].year, rules[i].categories[j].expense_category_id, rules[i].categories[j].status);
            if (j != rules[i].categories.size() - 1)
                printf(", ");
        }
        printf("}, \n");
    }
    printf("}\n");
}
//=================================================================================================
Database::Database(const char * connectionString)
{
	connect(connectionString);
}
//=================================================================================================
Database::Database()
{
	isConnected = false;
}
//=================================================================================================
bool Database::connect(const char * connectionString)
{
	conn = PQconnectdb(connectionString);
    isConnected = (PQstatus(conn) != CONNECTION_BAD);

    if (!isConnected)
        printf("Error while connecting: %s\n", getError());
	else
		createTables();

	return isConnected;
}
//=================================================================================================
Database::~Database()
{
	if (isConnected)
		disconnect();
}
//=================================================================================================
void Database::disconnect()
{
	if (isConnected)
	{
		PQfinish(conn);
		isConnected = false;
	}
}
//=================================================================================================
ExpenseCategoriesSets Database::getFrequentOneElementSets(double min_support)
{
	ExpenseCategoriesSets result;
	if (!isConnected)
	{
		printf("No connection with database.\n");
		return result;
	}




    //get total number of transactions - each transaction is a tuple (user, month, year)
	string cmd =    string("") +
                    "SELECT count(*) as total \n" +
                    "FROM \n" +
                    "( \n" +
                        "SELECT user_id, month, year \n" +
                        "FROM user_expenses \n" +
                        "GROUP BY user_id, month, year \n" +
                    ") as counter";
	PGresult * res = PQexec(conn, cmd.c_str());
	int total;
	if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
        total = atoi(PQgetvalue(res, 0, 0));
    else
	{
        printf("Error while getting total number of transactions: %s\n", PQerrorMessage(conn));
		return result;
	}





    //get frequent tuples (category, status)
    stringstream having;
    having << "HAVING(count(*)::float / " << total << " >= " << min_support << ")\n";

    cmd =   string("") +
            "SELECT category, status \n" +
            "FROM user_expenses \n" +
            "GROUP BY category, status \n" +
             having.str().c_str() +
             "ORDER BY category, status";
	res = PQexec(conn, cmd.c_str());
	if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int size = PQntuples(res);
        for (int i = 0; i < size; i++)
        {
            ExpenseCategoriesSet * set = new ExpenseCategoriesSet();
            ExpenseCategory * c = new ExpenseCategory();

            c->expense_category_id = atoi(PQgetvalue(res, i, 0));
            c->status = (ExpenseCategoryStatus)*PQgetvalue(res, i, 1);
            set->categories.push_back(*c);
            result.push_back(*set);
        }
    }
    else
        printf("Error while getting supported categories: %s\n", PQerrorMessage(conn));


	return result;
}
//=================================================================================================
void Database::getTransactions(Transactions & transactions)
{
	if (!isConnected)
	{
		printf("No connection with database.\n");
		return;
	}

    //gets transactions - we need tuples (user, month, year, category, status)
    //to prepare transactions [(category, status), (category1, status1), ...] for each (user, month, year)
    string cmd =    string("") +
                    "SELECT user_id, month, year, category, status \n" +
                    "FROM user_expenses \n" +
                    "ORDER BY user_id, month, year, category, status";
	PGresult * res = PQexec(conn, cmd.c_str());


	if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int size = PQntuples(res);
        int curr_user, curr_month, curr_year, user, month, year;
        ExpenseCategoriesSet * set = new ExpenseCategoriesSet();

        curr_user  = atoi(PQgetvalue(res, 0, 0));
        curr_month = atoi(PQgetvalue(res, 0, 1));
        curr_year  = atoi(PQgetvalue(res, 0, 2));

        for (int i = 0; i < size; i++)
        {
            user  = atoi(PQgetvalue(res, i, 0));
            month = atoi(PQgetvalue(res, i, 1));
            year  = atoi(PQgetvalue(res, i, 2));

            //one set per tuple (user, month, year)
            if (curr_user != user || curr_month != month || curr_year != year)
            {
                transactions.push_back(*set);

                set = new ExpenseCategoriesSet();
                curr_user  = user;
                curr_month = month;
                curr_year  = year;
            }

            ExpenseCategory * c = new ExpenseCategory();
            c->month = month;
            c->year = year;
            c->expense_category_id = atoi(PQgetvalue(res, i, 3));
            c->status = (ExpenseCategoryStatus)*PQgetvalue(res, i, 4);
            set->categories.push_back(*c);          
        }

		std::sort(set->categories.begin(), set->categories.end());
		transactions.push_back(*set);
    }
    else
        printf("Error while getting tuples for transactions: %s\n", PQerrorMessage(conn));
}
//=================================================================================================
vector<ExpenseCategory> Database::getCategories()
{
	vector<ExpenseCategory> result;

	if (!isConnected)
	{
		printf("No connection with database.\n");
		return result;
	}

    string cmd =    string("") +
                    "SELECT category, status \n" +
                    "FROM user_expenses \n" +
					"GROUP BY category, status \n" + 
                    "ORDER BY category, status";
	PGresult * res = PQexec(conn, cmd.c_str());

	if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int size = PQntuples(res);
        for (int i = 0; i < size; i++)
        {
            ExpenseCategory * c = new ExpenseCategory();
            c->expense_category_id = atoi(PQgetvalue(res, i, 0));
            c->status = (ExpenseCategoryStatus)*PQgetvalue(res, i, 1);
            result.push_back(*c);          
        }
    }
    else
        printf("Error while getting tuples for transactions: %s\n", PQerrorMessage(conn));

	return result;
}
//=================================================================================================
void Database::insertRulesToDB(Rules & rules)
{
	if (!isConnected)
	{
		printf("No connection with database.\n");
		return;
	}

	if (rules.size() == 0)
		return;

	string cmd_conditions   = "DELETE FROM associations_conditions; INSERT INTO associations_conditions (rule_id, category_id, status) VALUES ";
	string cmd_implications = "DELETE FROM associations_implications; INSERT INTO associations_implications (rule_id, category_id, status) VALUES ";
	stringstream ss;

	int rule_id = 1;
	for(Rules::iterator rule = rules.begin(); rule != rules.end(); rule++, rule_id++)
	{
		for(vector<RuleCondition>::iterator cond = rule->conditions.begin(); cond != rule->conditions.end(); cond++)
		{
			ss.str("");
			ss << "(" << rule_id << ", " << cond->expense_category_id << ", '" << (char)cond->status << "'), ";
			cmd_conditions += ss.str();
		}

		for(vector<RuleImplication>::iterator implic = rule->implications.begin(); implic != rule->implications.end(); implic++)
		{
			ss.str("");
			ss << "(" << rule_id << ", " << implic->expense_category_id << ", '" << (char)implic->status << "'), ";
			cmd_implications += ss.str();
		}
	}

		
	//remove last ', '
	cmd_conditions   = cmd_conditions.substr(0, cmd_conditions.length()-2);
	cmd_implications = cmd_implications.substr(0, cmd_implications.length()-2);



	//insert to db
    PGresult * result = PQexec(conn, cmd_conditions.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while adding conditions: %s\n", PQerrorMessage(conn));
		return;
	}
	result = PQexec(conn, cmd_implications.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while adding implications: %s\n", PQerrorMessage(conn));
		return;
	}
}
//=================================================================================================
PGresult * Database::execSql(string sql)
{
	if (!isConnected)
	{
		printf("No connection with database.\n");
		return NULL;
	}
    return PQexec(conn, sql.c_str());
}
//=================================================================================================
char * Database::getError()
{
    return PQerrorMessage(conn);
}
//=================================================================================================
void Database::createTables()
{
	//create table transactions
    string cmd = string("") +
			"CREATE TABLE IF NOT EXISTS transactions (id serial PRIMARY KEY, date date, amount integer, category bigint, user_id bigint);" +
			"CREATE INDEX transactions_category_user_id_idx ON transactions USING btree (category, user_id);";
    PGresult * result = PQexec(conn, cmd.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while creating table transactions: %s\n", PQerrorMessage(conn));
		return;
	}



	//create table user_expenses
    cmd = string("") +
			"CREATE TABLE IF NOT EXISTS user_expenses (id serial PRIMARY KEY, user_id bigint, month integer, year integer, category bigint, average integer, status char(1));" +
			"CREATE INDEX user_expenses_category_status_idx ON user_expenses USING btree (category, status);" + 
			"CREATE INDEX user_expenses_user_id_month_year_idx ON user_expenses USING btree (user_id, month, year);" +
			"CREATE INDEX user_expenses_user_id_month_year_category_status_idx ON user_expenses USING btree (user_id, month, year, category, status);";
    result = PQexec(conn, cmd.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while creating table user_expenses: %s\n", PQerrorMessage(conn));
		return;
	}


	//create table associations_conditions
	cmd = string("") + 
			"CREATE TABLE IF NOT EXISTS associations_conditions (rule_id bigint, category_id bigint, status char(1),  CONSTRAINT assoc_c_key PRIMARY KEY(rule_id, category_id));" +
			"CREATE INDEX associations_conditions_category_id_status_idx ON associations_conditions USING btree (category_id, status);" + 
			"CREATE INDEX associations_conditions_rule_id_idx ON associations_conditions USING btree (rule_id);";
    result = PQexec(conn, cmd.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while creating table associations_conditions: %s\n", PQerrorMessage(conn));
		return;
	}



	//create table associations_implications
	cmd = string("") +
			"CREATE TABLE IF NOT EXISTS associations_implications (rule_id bigint, category_id bigint, status char(1), CONSTRAINT assoc_i_key PRIMARY KEY(rule_id, category_id));" +
			"CREATE INDEX associations_implications_category_id_status_idx ON associations_implications USING btree (category_id, status);" + 
			"CREATE INDEX associations_implications_rule_id_idx ON associations_implications USING btree (rule_id);";
    result = PQexec(conn, cmd.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while creating table associations_implications: %s\n", PQerrorMessage(conn));
		return;
	}
}
