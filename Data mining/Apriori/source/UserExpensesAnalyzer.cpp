#include "UserExpensesAnalyzer.h"

//=================================================================================================
UserExpensesAnalyzer::UserExpensesAnalyzer(Database & db)
    :db(db)
{

}
//=================================================================================================
void UserExpensesAnalyzer::updateUsersExpenses()
{
	if (!db.getIsConnected())
	{
		printf("No connection with database.\n");
		return;
	}

	//clear table
	PGresult * result = db.execSql("DELETE FROM user_expenses");
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while clearing table user_expenses: %s\n", db.getError());
		return;
	}


    //fill user_expenses table
    result =
        db.execSql(string("") +
                   "DELETE FROM user_expenses; \n\n" +

                    "INSERT INTO user_expenses \n" +
                    "SELECT  nextval('user_expenses_id_seq'::regclass) AS id, \n" +
                        "user_id, \n" +
                        "extract(month from date) AS month, \n" +
                        "extract(year from date) AS year, \n" +
                        "category, \n" +
                        "CAST(trunc(avg(amount) * -1) AS integer) AS average \n" +
                    "FROM transactions \n" +
					"WHERE amount < 0 \n" + //only expenses
                    "GROUP BY user_id, category, month, year \n" +
                    "ORDER BY user_id, category, year, month;");

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
        printf("Error while updating user expenses table: %s\n", db.getError());
		return;
	}


    //update statuses
    result =
        db.execSql(string("") +
                    "WITH averages as \n" +
                    "( \n" +
                        "SELECT user_id, \n" +
                               "category, \n" +
                               "CAST(trunc(avg(average)) AS integer) AS total_average \n" +
                        "FROM user_expenses \n" +
                        "GROUP BY user_id, category \n" +
                    ") \n" +
                    "UPDATE user_expenses AS curr \n" +
                    "SET status = (CASE \n" +
                                    "WHEN curr.average > before.total_average * 1.25 THEN 'H' \n" +
                                    "WHEN curr.average < before.total_average * 0.75 THEN 'L' \n" +
                                    "ELSE 'M' \n" +
                                   "END) \n" +
                    "FROM averages AS before \n" +
                    "WHERE curr.user_id = before.user_id AND \n" +
                          "curr.category = before.category;");

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
        printf("Error while updating user expenses statuses: %s\n", db.getError());
}
//=================================================================================================
