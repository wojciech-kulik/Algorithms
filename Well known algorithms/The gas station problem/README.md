# The gas station problem

We have a vehicle with a given tank capacity. We assume that at each vertex gas
may be purchased at a certain price. The objective is to find the cheapest route to go from s to
t, or the cheapest tour visiting a given set of locations.

Helpful documents:
* [To Fill or not to Fill: The Gas Station Problem](documents/To Fill or not to Fill.pdf)
* [The Gas Station Problem](documents/The Gas Station Problem.pdf)

## Input 
n m  
p(1) p(2) ... p(n)  
a b c  
...  
x y z


n - number of cities  
m - number of roads  
p(i) - price for one fuel unit in i-th city   
a b c - road from "a" to "b", distance "c"  
z x y - query about the cheapest tour from "x" to "y" with tank capacity "z"

## Sample input
5 5  
10 10 20 12 13  
0 1 9  
0 2 8  
1 2 1  
1 3 11  
2 3 7  
10 0 3