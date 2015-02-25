#ifndef _TESTS_H_
#define _TESTS_H_

#include "Common.h"
#include "Database.h"

void startFixedTests(Database & db, double min_support, double min_confidence);
void generateTestUserExpenses(Database & db, Rule rule, double min_support, double min_confidence);

void generateRandomData(Database & db);

#endif