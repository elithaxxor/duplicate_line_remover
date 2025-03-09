# duplicate_line_remover
written in C-- This is if you dont have access to cuda cores. works fasat as hell 
Features and Optimizations:

## Custom Hash Table Implementation:

Uses a hash table with separate chaining for fast O(1) lookups
Employs the DJB2 hashing algorithm, which is both fast and offers good distributions
Uses a prime number table size (1,000,003) to minimize collisions


## Efficient Memory Management:

Only stores each unique line once in memory
Uses dynamic memory allocation to handle lines of any length
Properly frees all allocated memory to prevent leaks


## Performance Optimizations:

Single-pass linear time algorithm (O(n))
Avoids sorting or multiple file passes
Optimized string handling for minimum overhead


## I/O Efficiency:

Uses buffered I/O for better performance
Streams results directly to output files without storing the entire dataset in memory


## Robust Error Handling:

Checks all memory allocations and file operations
Provides informative error messages
Gracefully cleans up resources on error



# I've also included a Makefile with optimization flags to help compile the program with maximum performance. The Makefile uses:

# -O3 for aggressive optimization
# -march=native to optimize for your specific CPU architecture
# Standard warning flags to ensure code quality
