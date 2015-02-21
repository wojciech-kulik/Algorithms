#include <stdio.h>

using namespace std;

int * numbers;
int * tmp;

long long merge(int begin, int middle, int end)
{
	int pos = begin;
	int left_it = begin;
	int right_it = middle + 1;

	int left_size = middle - begin + 1;
	long long counter = 0;

	//tmp copy
	for (int i = begin; i <= end; i++)
		tmp[i] = numbers[i];

	//merge
	while (left_it <= middle && right_it <= end)
	{
		if (tmp[left_it] <= tmp[right_it])
		{
			numbers[pos++] = tmp[left_it++];
			left_size--;
		}
		else
		{
			numbers[pos++] = tmp[right_it++];
			counter += left_size;
		}
	}

	//insert remaining elements
	while (left_it <= middle)
		numbers[pos++] = tmp[left_it++];
	while (right_it <= end)
		numbers[pos++] = tmp[right_it++];

	return counter;
}

long long inversion_count(int begin, int end)
{
	if (begin >= end)
		return 0;

	int middle = (begin + end) / 2;
	long long l_count = inversion_count(begin, middle);
	long long r_count = inversion_count(middle + 1, end);

	return l_count + r_count + merge(begin, middle, end);
}

int main()
{
	int n;
	scanf("%d", &n);

	numbers = new int[n];
	tmp = new int[n];

	for (int i = 0; i < n; i++)
		scanf("%d", &numbers[i]);

	printf("%lld", inversion_count(0, n-1));
	return 0;
}