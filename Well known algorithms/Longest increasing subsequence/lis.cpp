#include <stdio.h>
#include <stack>

using namespace std;

struct Number
{
	int value;
	Number * prev;
};

//returns length of the longest prefix of "elem"
int binSearch(Number * numbers, int * seq, Number elem, int length)
{
	// Binary search for the largest positive j ≤ L such that numbers[seq[j]] < elem
	int lo = 1;
	int hi = length;
	int mid;

	while (lo <= hi)
	{
		mid = (lo + hi) / 2;
		if (numbers[seq[mid]].value < elem.value)
			lo = mid + 1;
		else
			hi = mid - 1;
	}

	return lo - 1;
}


int main()
{
	int n;
	scanf("%d", &n);

	//load data	
	Number * numbers = new Number[n];
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &numbers[i].value);
		numbers[i].prev = NULL;
	}

	//in each moment we've got increasing sequence: numbers[seq[1]].value, numbers[seq[1]].value, ..., numbers[seq[length]].value
	//source: http://en.wikipedia.org/wiki/Longest_increasing_subsequence

	//dynamic
	int * seq = new int[n + 1]; //seq[i] - index of smallest number which is the last element in sequence (length = i)
	seq[0] = 0;
	int length = 0, newLength;
	for (int i = 0; i < n; i++)
	{
		newLength = binSearch(numbers, seq, numbers[i], length) + 1;

		//set predecessor as the last element of sequence of length (newLength - 1)
		numbers[i].prev = &numbers[seq[newLength - 1]];

		//if we have the new longest sequence then save it
		if (newLength > length) 
		{
			seq[newLength] = i;
			length = newLength;
		}
		//update last elem if we have smaller
		else if (numbers[i].value < numbers[seq[newLength]].value) 
		{
			seq[newLength] = i;
		}
	}

	//result
	stack<int> result;
	Number * nr = &numbers[seq[length]];
	for (int i = 0; i < length; i++)
	{
		result.push(nr->value);
		nr = nr->prev;
	}

	printf("%d\n", length);
	
	int val;
	while (!result.empty())
	{
		val = result.top();
		result.pop();
		printf("%d ", val);
	}
	return 0;
}