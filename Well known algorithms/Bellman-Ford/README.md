# Bellman-Ford algorithm

It is an algorithm that computes shortest paths from a single source vertex to all of the other vertices in a weighted digraph.[1] It is slower than Dijkstra's algorithm for the same problem, but more versatile, as it is capable of handling graphs in which some of the edge weights are negative numbers.  
http://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm

## Input 
v e s  
a b c  
...


v - number of vertices  
e - number of edges   
s - start point  
a b c - edge between "a" and "b" and with cost "c"

## Sample input
3 2 1  
0 1 10  
1 2 20

## Output

This algorithm returns shortest distances from city "s" to all other cities.  
It can also return "Negative cycle".