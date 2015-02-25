#include "Logger.h"

#ifndef _WIN32
	#define sprintf_s snprintf
#endif

//=================================================================================================
Logger::Logger(string fileName)
{
	file.open(fileName.c_str(), ios::out);
	file.clear();
}
//=================================================================================================
Logger::~Logger()
{
	file.close();
}
//=================================================================================================
void Logger::writeToFile(string data)
{
	file.write(data.c_str(), data.length());
#ifndef _WIN32
	file.seekg(0, ios_base::end);
#endif
}
//=================================================================================================
void Logger::log(string text)
{
#ifndef LOGGER_ON
	return;
#endif
	writeToFile(text);
}
//=================================================================================================
void Logger::log(Transactions & transactions)
{
#ifndef LOGGER_ON
	return;
#endif

	char data[100];

	sprintf_s(data, 50, "Transactions [%d] :{\n", transactions.size());
	writeToFile(data);
    for (unsigned i = 0; i < transactions.size(); i++)
    {
		sprintf_s(data, 50, "\t{index: %d | ", i);
        writeToFile(data);
        for(unsigned j = 0; j < transactions[i].categories.size(); j++)
        {			
			sprintf_s(data, 50, "(%d, %c)", transactions[i].categories[j].expense_category_id,
				(char)transactions[i].categories[j].status);
            writeToFile(data);

            if (j != transactions[i].categories.size() - 1)
                writeToFile(", ");
        }
        writeToFile("}, \n");
    }
    writeToFile("}\n\n\n");
}
//=================================================================================================
void Logger::log(ExpenseCategoriesSet & candidate, bool hasMinSupport)
{
#ifndef LOGGER_ON
	return;
#endif

	char data[100];

	writeToFile(string("\t") + "{hasMinSupport: " + (hasMinSupport ? "yes | " : "no  | "));
	for(unsigned j = 0; j < candidate.categories.size(); j++)
    {	
		sprintf_s(data, 50, "(%d, %c)", candidate.categories[j].expense_category_id,
			(char)candidate.categories[j].status);
		writeToFile(data);

		if (j != candidate.categories.size() - 1)
			writeToFile(", ");
	}
	writeToFile("}\n");
}
//=================================================================================================
void Logger::log(Rule & rule, bool hasMinConfidence)
{
#ifndef LOGGER_ON
	return;
#endif

	char data[100];

	writeToFile(string("\t") + "{hasMinConfidence: " + (hasMinConfidence ? "yes | " : "no  | "));
	for(unsigned i = 0; i < rule.conditions.size(); i++)
    {	
		sprintf_s(data, 50, "(%d, %c)", rule.conditions[i].expense_category_id,
			(char)rule.conditions[i].status);
		writeToFile(data);

		if (i != rule.conditions.size() - 1)
			writeToFile(", ");
		else
			writeToFile(" ===> ");
	}

	for(unsigned i = 0; i < rule.implications.size(); i++)
    {	
		sprintf_s(data, 50, "(%d, %c)", rule.implications[i].expense_category_id,
			(char)rule.implications[i].status);
		writeToFile(data);

		if (i != rule.implications.size() - 1)
			writeToFile(", ");
	}
	writeToFile("}\n");
}
//=================================================================================================