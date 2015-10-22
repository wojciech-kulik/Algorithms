#include <stdio.h>
#include <set>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Edge
{
    Edge(int to, int cost)
    {
        this->to = to;
        this->cost = cost;
    }

    int to;
    int cost;
};

struct Vertex
{
    int id;
    long long minCost;
    vector<Edge> edges;

    bool operator<(const Vertex &other) const 
    { 
        if (minCost == other.minCost)
            return id < other.id;

        return minCost < other.minCost; 
    }
};


//PRIORITY QUEUE
struct CompareNode
{
    bool operator()(const Vertex* lhs, const Vertex* rhs) const
    {
        return *lhs < *rhs;
    }
};
set<Vertex*, CompareNode> tree;

void push(Vertex* v)
{
    tree.insert(v);
}

void decrease_key(Vertex* v)
{
    set<Vertex*, CompareNode>::iterator it = tree.find(v);
    if (it != tree.end())
        tree.erase(it);
    push(v);
}

Vertex* pop()
{
    Vertex* v = *tree.begin();
    tree.erase(tree.begin());
    return v;
}

bool empty()
{
    return tree.empty();
}
//=========================================================================

int main()
{
    int v, e, s;
    scanf("%d %d %d", &v, &e, &s);

    //VERTICES
    Vertex * vertices = new Vertex[v];
    for (int i = 0; i < v; i++)
    {
        vertices[i].id = i;
        vertices[i].minCost = LONG_MAX;     
    }   

    //EDGES
    int a, b, c;
    for (int i = 0; i < e; i++)
    {
        scanf("%d %d %d", &a, &b, &c);
        vertices[a - 1].edges.push_back(Edge(b - 1, c));
        vertices[b - 1].edges.push_back(Edge(a - 1, c));
    }


    //DIJKSTRA
    vertices[s - 1].minCost = 0;
    push(&vertices[s - 1]);

    Vertex* current;
    long long newCost;
    while (!empty())
    {
        current = pop();

        for (vector<Edge>::iterator it = current->edges.begin(); it != current->edges.end(); it++)
        {
            newCost = vertices[current->id].minCost + it->cost;

            if (vertices[it->to].minCost > newCost)
            {                           
                vertices[it->to].minCost = newCost;
                decrease_key(&vertices[it->to]);
            }
        }
    }

    //RESULT
    for (int i = 1; i < v; i++)
        printf("%ld ", vertices[i].minCost);

    return 0;
}
