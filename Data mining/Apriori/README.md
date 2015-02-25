# Apriori

It is an algorithm for frequent item set mining and association rule learning over transactional databases. It proceeds by identifying the frequent individual items in the database and extending them to larger and larger item sets as long as those item sets appear sufficiently often in the database. The frequent item sets determined by Apriori can be used to determine association rules which highlight general trends in the database: this has applications in domains such as market basket analysis.  
http://en.wikipedia.org/wiki/Apriori_algorithm  

## Implementation  
This implementation:  
1. Reads user expenses from table *transactions*.
2. Creates tuples (user, month, year, category, status) in table *user_expenses*, where status means how much money a user spent for each category each month. Status will be one of the following values: L - Low, M - Medium, H - High.
3. Generates rules using Apriori algorithm, which allow to find associations between expenses in categories.
4. Inserts rules into tables *associations_conditions* and *associations_implications*.

## Requirements
It requires PostgreSQL database with a table:  
```
transactions (id serial PRIMARY KEY, date date, amount integer, category bigint, user_id bigint)
```

## Input 
The application needs the following parameters:  
*host port dbname user password min_confidence min_support*


## Output

Two tables in a database, which contain generated association rules:   
- associations_conditions  
- associations_implications

If logging is turned on, all association rules will be also in a file *logs.txt*.

## Remarks
- It is possible to generate random transactions by defining *RANDOM_TEST* in *main.cpp*.   
- Can be useful to understand Apriori algorithm:  
http://nikhilvithlani.blogspot.com/2012/03/apriori-algorithm-for-data-mining-made.html