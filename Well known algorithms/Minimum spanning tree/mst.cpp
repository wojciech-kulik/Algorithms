#include <stdio.h>

struct Edge
{
	int a, b;
	int weight;
};

struct Vertex
{
	int id;
	Vertex * parent;
	int rank;
};

void sort(Edge * edges, int start, int end)
{
	int d = edges[(start + end) / 2].weight;

	int i = start, j = end;
	Edge tmp;

	do
	{
		while (d > edges[i].weight) i++;
		while (d < edges[j].weight) j--;

		if (i <= j)
		{
			tmp = edges[i];
			edges[i] = edges[j];
			edges[j] = tmp;

			i++;
			j--;
		}
	} while (i <= j);

	if (start < j)
		sort(edges, start, j);

	if (i < end)
		sort(edges, i, end);
}

void makeSet(Vertex * v)
{
	v->parent = NULL;
	v->rank = 0;
}

Vertex * find(Vertex * v)
{
	while (v->parent != NULL)
		v = v->parent;

	return v;
}

void unionSets(Vertex * x, Vertex * y)
{
	Vertex * xRoot = find(x);
	Vertex * yRoot = find(y);

	if (xRoot->rank > yRoot->rank)
		yRoot->parent = xRoot;
	else if (xRoot->rank < yRoot->rank)
		xRoot->parent = yRoot;
	else if (xRoot != yRoot)
	{
		yRoot->parent = xRoot;
		xRoot->rank++;
	}
}

int main()
{
	//Kruskal's Algorithm
	int v, e;
	scanf("%d %d", &v, &e);

	Edge * edges = new Edge[e];
	for (int i = 0; i < e; i++)
		scanf("%d %d %d", &edges[i].a, &edges[i].b, &edges[i].weight);
	
	sort(edges, 0, e - 1);

	//disjoint-set
	Vertex * vertices = new Vertex[v + 1];
	for (int i = 0; i <= v; i++)
	{
		vertices[i].id = i + 1;
		makeSet(&vertices[i]);
	}		

	int count = 0;
	long sum = 0;
	for (int i = 0; i < e; i++)
	{
		if (find(&vertices[edges[i].a]) != find(&vertices[edges[i].b]))
		{
			unionSets(&vertices[edges[i].a], &vertices[edges[i].b]);			
			sum += edges[i].weight;
			count++;
		}

		if (count == v - 1)
			break;
	}

	printf("%ld", sum);
	return 0;
}