#define INT_MAX 2147483647

#include <stdio.h>
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

struct Field
{
	int cost;
};

string in, out;

int getCost(Field **tab, int y, int x)
{
	if (y < 0 || x < 0)
		return -1;

	return tab[y][x].cost;
}

void setCostFor(Field **tab, int y, int x, int cost, int operationCost)
{
	if (cost != -1 && cost < tab[y][x].cost)
	{
		tab[y][x].cost = cost + operationCost;
	}
}

void setMinFor(Field **tab, int y, int x)
{
	//copy - characters are the same
	if (in[x] == out[y])
	{
		setCostFor(tab, y, x, getCost(tab, y - 1, x - 1), 0);
	}

	//add character
	setCostFor(tab, y, x, getCost(tab, y - 1, x), 1);

	//delete character
	setCostFor(tab, y, x, getCost(tab, y, x - 1), 1);
}

int main()
{
	int in_len, out_len;

	cin >> in_len >> in;
	cin >> out_len >> out;

	Field **tab = new Field*[out_len];
	for (int i = 0; i < out_len; i++)
	{
		tab[i] = new Field[in_len];

		for (int j = 0; j < in_len; j++)
		{
			tab[i][j].cost = INT_MAX;
		}
	}

	//initialize first row
	tab[0][0].cost = 0;
	for (int i = 1; i < in_len; i++)
	{
		tab[0][i].cost = tab[0][i - 1].cost + 1;
	}

	//dynamic - calculate array
	for (int o = 1; o < out_len; o++)
	{
		for (int i = 0; i < in_len; i++)
		{
			setMinFor(tab, o, i);
		}
	}


	cout << tab[out_len - 1][in_len - 1].cost << endl;
	return 0;
}