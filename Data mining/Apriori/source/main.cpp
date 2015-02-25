#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "ExpenseCategoryRulesFinder.h"
#include "UserExpensesAnalyzer.h"
#include "Tests.h"

#ifdef _WIN32
	#include <Windows.h>
#elif defined(__MACH__)
  #include <mach/clock.h>
  #include <mach/mach.h>
  #include <cstdio>
  #define sprintf_s snprintf
#else
    #include <time.h>
	#define sprintf_s snprintf
#endif

#ifdef _WIN32
    void printTime(int time)
    {
        printf("Calculation time: %ds = %.1fm\n\n", time / 1000, time / 1000.0 / 60.0);
    }

    int GetTime()
    {
        return GetTickCount();
    }

#elif defined(__MACH__)

    void printTime(int time)
    {
        printf("Calculation time: %ds = %.1fm\n\n", time, time / 60.0);
    }

    int GetTime()
    {
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        return mts.tv_sec;
    }
#else

    void printTime(int time)
    {
        printf("Calculation time: %ds = %.1fm\n\n", time, time / 60.0);
    }
    int GetTime()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return tp.tv_sec;
    }
#endif

//#define FIXED_TEST
//#define RANDOM_TEST

int main(int argc, char ** argv)
{
	printf("Parameters: host port dbname user password min_confidence min_support\n\n");

	if (argc < 8)
	{
		printf("Wrong number of parameters...\n");
		getchar();
		return 1;
	}

	const double min_confidence = std::stof(argv[6]);
	const double min_support = std::stof(argv[7]);

	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("%s\n\n", asctime(timeinfo));

	int start, total_start = GetTime();

	printf("Connecting to database...\n");
	char connectionString[200];
	sprintf_s(connectionString, 200, "host=%s port=%s dbname=%s user=%s password=%s", argv[1], argv[2], argv[3], argv[4], argv[5]);
	Database db(connectionString);
	printf("\n");

#ifdef RANDOM_TEST
	printf("Generating test data...\n");
	generateRandomData(db);
	printf("\n");	
#endif

#ifdef FIXED_TEST
	printf("Generating test data...\n");
	startFixedTests(db, 0.80, 0.90);
	printf("\n");
#else
	printf("Updating users expenses data...\n");
	start = GetTime();
	UserExpensesAnalyzer analyzer(db);
	analyzer.updateUsersExpenses();
	printTime(GetTime() - start);
#endif

	printf("Calculating frequent sets...\n");
	start = GetTime();
	ExpenseCategoryRulesFinder f(db, min_support, min_confidence);
	ExpenseCategoriesSets frequentSets = f.execute();
	printf("Number of frequent sets: %d\n", frequentSets.size());
	printTime(GetTime() - start);

	printf("Generating rules and saving to database...\n");
	start = GetTime();
	Rules rules = f.generateRules(frequentSets);
	printf("Number of rules: %d\n", rules.size());
	printTime(GetTime() - start);

	printf("Removing rules from database with low confidence...\n");
	start = GetTime();
	f.removeRulesWithLowConfidence(rules);
	printf("Number of rules: %d\n", rules.size());
	printTime(GetTime() - start);

	printf("Inserting rules to database...\n");
	start = GetTime();
	db.insertRulesToDB(rules);
	printTime(GetTime() - start);

	printf("\n");
	printTime(GetTime() - total_start);
	printf("Operation complete. Press any key to exit.");
	getchar();

	return 0;
}
