#include <iostream>
#include <string>

using namespace std;

struct Field
{
	char c;
	int length;
	Field * previous;
};

int main()
{	
	//get strings
	string s1, s2;
	cin >> s1;
	cin >> s2;

	//get lengths
	int len1 = s1.length();
	int len2 = s2.length();

	//init array
	Field ** lcs = new Field*[len1 + 1];
	for (int i = 0; i <= len1; i++)
	{
		lcs[i] = new Field[len2 + 1];

		for (int j = 0; j <= len2; j++)
		{
			lcs[i][j].c = '\0';
			lcs[i][j].length = 0;
			lcs[i][j].previous = NULL;
		}
	}

	//dynamic
	for (int idx1 = 1; idx1 <= len1; idx1++)
	{
		for (int idx2 = 1; idx2 <= len2; idx2++)
		{
			if (s1[idx1 - 1] == s2[idx2 - 1])
			{
				lcs[idx1][idx2].c		 =   s1[idx1 - 1];
				lcs[idx1][idx2].length	 =  lcs[idx1 - 1][idx2 - 1].length + 1;
				lcs[idx1][idx2].previous = &lcs[idx1 - 1][idx2 - 1];
			}
			else
			{
				if (lcs[idx1 - 1][idx2].length > lcs[idx1][idx2 - 1].length)
				{
					lcs[idx1][idx2].length   =  lcs[idx1 - 1][idx2].length;
					lcs[idx1][idx2].previous = &lcs[idx1 - 1][idx2];
				}
				else
				{
					lcs[idx1][idx2].length   =  lcs[idx1][idx2 - 1].length;
					lcs[idx1][idx2].previous = &lcs[idx1][idx2 - 1];
				}
			}				
		}
	}

	//result
	string result;
	Field * current = &lcs[len1][len2];
	while (current)
	{
		if (current->c != '\0')
			result = current->c + result;
		current = current->previous;
	}
	cout << result.c_str();

	return 0;
}