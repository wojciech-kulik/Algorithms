#include "ExpenseCategoriesSet.h"

//=================================================================================================
short ExpenseCategoriesSet::compare(const ExpenseCategoriesSet & set) const
{
	if (set.categories.size() < categories.size())
		return 1;
	else if (set.categories.size() > categories.size())
		return -1;

	unsigned count = categories.size();
	for (unsigned i = 0; i < count; i++)
		if (set.categories[i] < categories[i])
			return 1;
		else if (set.categories[i] > categories[i])
			return -1;

	return 0;
}
//=================================================================================================
bool ExpenseCategoriesSet::equalsWithoutLast(const ExpenseCategoriesSet & set)
{
	if (categories.size() != set.categories.size())
		return false;

    int count = set.categories.size() - 1;
	for (int i = 0; i < count; i++)
		if (categories[i] != set.categories[i])
            return false;

	return true;
}
//=================================================================================================
ExpenseCategoriesSet ExpenseCategoriesSet::joinWith(const ExpenseCategoriesSet & set)
{
	ExpenseCategoriesSet result;

	if (set.categories.back() > categories.back())
	{
		result.categories = categories;
		result.categories.push_back(set.categories.back());
	}
	else
	{
		result.categories = set.categories;
		result.categories.push_back(categories.back());
	}

	return result;
}
//=================================================================================================
