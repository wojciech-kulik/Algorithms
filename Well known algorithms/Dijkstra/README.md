# Dijkstra's algorithm

It is a graph search algorithm that solves the single-source shortest path problem for a graph with non-negative edge path costs, producing a shortest path tree.  
http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm

## Input 
v e s   
a b c  (e times)  
...  


v - number of vertices  
e - number of edges   
s - start vertex   
a b c - edge between "a" and "b" and with cost "c"

## Sample input
6 10  
1 2 2  
1 6 1  
1 5 3  
4 1 5  
2 6 2  
2 3 5  
4 3 4  
3 5 4  
4 5 4  
5 6 3

## Output

This algorithm returns shortest distances from city "s" to all other cities.
