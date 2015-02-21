#include <stdio.h>
#include <string>
#include <climits>

using namespace std;

int main()
{
	//length of sequence
	int n;
	scanf("%d", &n);

	//read dimensions
	int * d = new int[n + 1];
	for (int i = 0; i <= n; i++)
		scanf("%d", &d[i]);
		
	//initialize array
	int ** m = new int*[n + 1];
	for (int i = 0; i <= n; i++)
	{
		m[i] = new int[n + 1];			

		for (int j = 0; j <= n; j++)
			m[i][j] = INT_MAX;
		m[i][i] = 0;
	}

	//calculate needed multiplies
	for (int l = 2; l <= n; l++)
	{
		for (int i = 1; i <= n - l + 1; i++)
		{
			int j = i + l - 1;
			int mult;
			for (int k = i; k < j; k++)
			{
				mult = m[i][k] + m[k + 1][j] + d[i - 1] * d[k] * d[j];
				if (mult < m[i][j])
					m[i][j] = mult;
			}
		}
	}

	//result
	printf("%d\n", m[1][n]);

	//clean up
	for (int i = 0; i <= n; i++)
		delete[] m[i];
	delete[] m;
	delete[] d;

	return 0;
}