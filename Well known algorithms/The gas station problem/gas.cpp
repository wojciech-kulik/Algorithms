#include <stdio.h>
#include <list>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;


const int MAX_TANK = 100;
int v, e;


struct Edge
{
	Edge(int to, int dist)
	{
		this->to = to;
		this->dist = dist;
	}

	int to;
	int dist;
};

struct Vertex
{
	int price;
	int minDistance;
	list<Edge> edges;
};

struct SolutionState
{
	int id;
	int fuel;
	long long minCost;
	bool inHeap;

	bool operator>(const SolutionState &other) const
	{
		if (minCost == other.minCost)
			return fuel == other.fuel ? id > other.id : fuel > other.fuel;

		return minCost > other.minCost;
	}
};



//PRIORITY QUEUE
vector<SolutionState*> heap;

struct CompareNode
{
	bool operator()(const SolutionState* lhs, const SolutionState* rhs) const
	{
		return *lhs > *rhs;
	}
};

void push(SolutionState* v)
{
	v->inHeap = true;
	heap.push_back(v);
	std::push_heap(heap.begin(), heap.end(), CompareNode());
}

void decrease_key(SolutionState* v)
{
	unsigned i;
	for (i = 1; i <= heap.size(); i++)
		if (heap[i - 1] == v)
			break;

	while (i > 1 && *heap[(i / 2) - 1] > *heap[i - 1])
	{
		std::swap(heap[(i / 2) - 1], heap[i - 1]);
		i /= 2;
	}
}

void push_or_update(SolutionState* v)
{
	if (v->inHeap)
	{
		decrease_key(v);
	}
	else
	{
		push(v);
	}
}

SolutionState* pop()
{
	SolutionState* v = heap.front();
	std::pop_heap(heap.begin(), heap.end(), CompareNode());
	heap.pop_back();
	v->inHeap = false;
	return v;
}

bool empty()
{
	return heap.empty();
}

void clear()
{
	heap.clear();
}
//=========================================================================




void relax(SolutionState **D, bool **visited, int tankCapacity, int u, int f, long long newCost)
{
	if (f >= 0 && f <= tankCapacity && (!visited[u][f] || D[u][f].minCost > newCost))
	{
		if (!visited[u][f])
		{			
			D[u][f].id = u;
			D[u][f].fuel = f;
			D[u][f].inHeap = false;
			visited[u][f] = true;
		}

		D[u][f].minCost = newCost;
		push_or_update(&D[u][f]);
	}
}

long long solution(SolutionState **D, Vertex *vertices, bool **visited, int source, int destination, int tankCapacity)
{
	//trivial case
	if (destination == source)
	{
		return 0;
	}
	if (vertices[destination].edges.size() == 0)
	{
		return -1;
	}

	//initialize
	for (int i = 0; i < v; i++)
	{
		memset(visited[i], 0, sizeof(bool) * (tankCapacity + 1));
	}
	heap.reserve(v * (tankCapacity + 1));

	//set start
	clear();
	D[source][0].minCost = 0;
	D[source][0].fuel = 0;
	D[source][0].id = source;
	visited[source][0] = true;
	push(&D[source][0]);

	//DIJKSTRA
	SolutionState *current;
	long long newCost;
	int fuelUnits;
	while (!empty())
	{
		current = pop();
		//printf("     id: %5d, fuel: %5d, cost: %6lld\n", current->id, current->fuel, visited[current->id][current->fuel] ? D[current->id][current->fuel].minCost : -1);

		//don't go further than destination
		if (current->id == destination && current->fuel == 0)
			return D[destination][0].minCost;

		//stay in the city and buy more fuel - it can improve minCost for D[v][f + fuelUnits]
		//don't refuel less than needed to get to the next city
		fuelUnits = current->fuel < vertices[current->id].minDistance ? (vertices[current->id].minDistance - current->fuel) : 1;
		newCost = D[current->id][current->fuel].minCost + fuelUnits * vertices[current->id].price;
		relax(D, visited, tankCapacity, current->id, current->fuel + fuelUnits, newCost);

		//go to the adjacent cities "u" - it can improve minCost for D[u][f - d]	
		newCost = D[current->id][current->fuel].minCost;
		for (list<Edge>::iterator it = vertices[current->id].edges.begin(); it != vertices[current->id].edges.end(); it++)
		{
			relax(D, visited, tankCapacity, it->to, current->fuel - it->dist, newCost);
		}
	}

	return -1;
}

int main()
{
	scanf("%d %d", &v, &e);

	//FUEL PRICE, VERTICES
	bool **visited = new bool*[v];
	Vertex *vertices = new Vertex[v];
	SolutionState **D = new SolutionState*[v];
	for (int i = 0; i < v; i++)
	{
		scanf("%d", &vertices[i].price);
		D[i] = new SolutionState[MAX_TANK + 1];
		visited[i] = new bool[MAX_TANK + 1];
	}

	//LOAD EDGES
	int a, b, c;
	for (int i = 0; i < e; i++)
	{
		scanf("%d %d %d", &a, &b, &c);
		vertices[a].edges.push_back(Edge(b, c));
		vertices[b].edges.push_back(Edge(a, c));

		//calculate minimum distance to get to the next city
		if (vertices[a].edges.size() == 1)
			vertices[a].minDistance = c;
		else
			vertices[a].minDistance = std::min(vertices[a].minDistance, c);

		if (vertices[b].edges.size() == 1)
			vertices[b].minDistance = c;
		else
			vertices[b].minDistance = std::min(vertices[b].minDistance, c);
	}

	//SOLUTION
	int source, destination, tankCapacity;
	long long result;

	scanf("%d %d %d", &tankCapacity, &source, &destination);
	result = solution(D, vertices, visited, source, destination, tankCapacity);

	if (result == -1)
		printf("impossible\n");
	else
		printf("%lld\n", result);

	return 0;
}