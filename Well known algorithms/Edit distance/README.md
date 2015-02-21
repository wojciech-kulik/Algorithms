# Edit distance

Edit distance is a way of quantifying how dissimilar two strings (e.g., words) are to one another by counting the minimum number of operations required to transform one string into the other.  
http://en.wikipedia.org/wiki/Edit_distance

In this implementation, we assume that only three operations are available:
* copy - cost 0
* insert - cost 1
* delete - cost 1

## Input 
n a   
m b

n, m - length of string  
a, b - strings (without spaces, new lines etc.)

## Sample input
5 heart  
4 head