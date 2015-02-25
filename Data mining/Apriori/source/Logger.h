#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <fstream>
#include "Common.h"
#include "ExpenseCategoriesSet.h"

#define LOGGER_ON

using namespace std;

class Logger
{
protected:
	fstream file;
	void writeToFile(string data);

public:
	Logger(string fileName);
	~Logger();
	void log(Transactions & transactions);
	void log(Rule & rule, bool hasMinConfidence);
	void log(ExpenseCategoriesSet & candidate, bool hasMinSupport);
	void log(string text);
};

#endif