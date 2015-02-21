#include <stdio.h>
#include <vector>
#include <climits>

using namespace std;

struct Vertex
{
	long long minCost;
};

struct Edge
{
	int u;
	int v;	
	int cost;
};


int main()
{
	int v, e, s;
	scanf("%d %d %d", &v, &e, &s);

	//VERTICES
	Vertex * vertices = new Vertex[v + 1];
	for (int i = 0; i <= v; i++)
		vertices[i].minCost = LONG_MAX;

	//EDGES
	Edge * edges = new Edge[e];
	for (int i = 0; i < e; i++)
	{
		scanf("%d %d %d", &edges[i].u, &edges[i].v, &edges[i].cost);
	}

	//BELLMAN-FORD
	vertices[s].minCost = 0;
	long long newCost;

	//relax edges
	for (int i = 0; i < v; i++)
	{
		for (int j = 0; j < e; j++)
		{
			if (vertices[edges[j].u].minCost == LONG_MAX)
				continue;

			newCost = vertices[edges[j].u].minCost + edges[j].cost;
			if (vertices[edges[j].v].minCost > newCost)
				vertices[edges[j].v].minCost = newCost;
		}
	}

	//check for negative cycles
	for (int j = 0; j < e; j++)
	{
		if (vertices[edges[j].u].minCost == LONG_MAX)
			continue;

		if (vertices[edges[j].v].minCost > vertices[edges[j].u].minCost + edges[j].cost)
		{
			printf("Negative cycle");
			return 0;
		}
	}

	//RESULT
	for (int i = 0; i <= v; i++)
	{
		if (i != s && vertices[i].minCost != LONG_MAX)
			printf("%d %lld\n", i, vertices[i].minCost);
	}

	return 0;
}