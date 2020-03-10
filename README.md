# stats.cpp

This program uses multiple threads along with mutexes to quickly calculate the standard deviation of a given large array of integers.

The first argument is the name of the file containing the integers (one number per
line) to be read into the array, and the second argument, T, is the number of threads to be created.
The program assumes that the input contains N integers where N ≤ 1,000,000 and the number of
threads is always smaller than or equal to the number of integers in the file, i.e. T ≤ N.
