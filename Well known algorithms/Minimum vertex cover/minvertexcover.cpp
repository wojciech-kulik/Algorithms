#include <stdio.h>
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

using namespace std;

//DFS
int cover_vertex_number(list<int> *edges, vector<bool> &visited, vector<bool> &chosen, int root)
{
	if (visited[root])
		return 0;
	else
		visited[root] = true;

	int counter = 0;
	for (list<int>::iterator it = edges[root].begin(); it != edges[root].end(); it++)
	{
		//to avoid loops, because edges are undirected
		if (visited[*it])
			continue;

		counter += cover_vertex_number(edges, visited, chosen, *it);

		//if two vertices of the edge (root, *it) are not picked, then we choose root
		if (!chosen[*it] && !chosen[root])
		{
			chosen[root] = true;
			counter++;
		}
	}

	return counter;
}

int main()
{
	int n, a, b;
	scanf("%d", &n);

	list<int> *edges = new list<int>[n + 1];
	vector<bool> visited(n + 1, false);
	vector<bool> chosen(n + 1, false);

	for (int i = 0; i < n - 1; i++)
	{
		scanf("%d %d", &a, &b);
		edges[a].push_back(b);
		edges[b].push_back(a);
	}

	printf("%d", cover_vertex_number(edges, visited, chosen, 1));
	return 0;
}