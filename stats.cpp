// CPSC 457 Assignment 2 Q5
// Usage: ./scan [suffix] [N] [type]
// To compile: g++ -o stats -lpthread stats.cpp
// By: Evan Wheeler
// UCID# 30046173

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

//Initializing global variables
long long globalSum = 0;
int globalNumInts = 0;
int integers[1000000];
int maxThreads;
double globalMean;
double globalDiffSqr;
double globalStdev;


//Initializing Mutexes
pthread_mutex_t mutexMean = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexStdev = PTHREAD_MUTEX_INITIALIZER;


//Function the threads will execute when finding the mean
void* getMean(void* param) {
	//Thread ID
	long int ID = (long int) param;
	// The section of the array that will be larger
	int firstSect = globalNumInts % maxThreads;
	//Place holders to know where to access in the array
	int start;
	int end;
	//The offsets of the front and back of the array
	int frontOffset = ceil((float)globalNumInts / (float)maxThreads);
	int backOffset = floor((float)globalNumInts / (float)maxThreads);

	//Determine which indices the thread will access
	if (ID < firstSect) {
		start = frontOffset * ID;
		end = start + frontOffset;
	} else {
		start = (frontOffset * firstSect) + (backOffset * (ID - firstSect));
		end = start + backOffset;
	}

	//To keep track of the sum in the thread and number of ints
	int localSum = 0;
	int localNumInts = 0;
	//Find localSum
	for (int i = start; i < end; i++) {
		localSum += integers[i];
		localNumInts++;
	}

	double localMean = (double)localSum / (double)localNumInts;

	//Critical section
	pthread_mutex_lock(&mutexMean);

	cout << "Thread " << (ID + 1) << ": " << fixed << setprecision(2) << localMean << endl;
	//For Debugging
	// cout << "Thread " << (ID + 1) << "localSum: " << localSum << endl;
	// cout << "Thread " << (ID + 1) << "localNum: " << localNumInts << endl;

	globalSum += localSum;

	pthread_mutex_unlock(&mutexMean);

	pthread_exit(0);
	return NULL;
}


//The function the threads will execute when finding the Standard Deviation
void* getStdev(void* param) {
	//Thread ID
	long int ID = (long int) param;
	// N/T
	int firstSect = globalNumInts % maxThreads;
	//Place holders to know where to access in the array
	int start;
	int end;
	//Offsets for front and back of the array
	int frontOffset = ceil((float)globalNumInts / (float)maxThreads);
	int backOffset = floor((float)globalNumInts / (float)maxThreads);

	//Determine which indices the thread will access
	if (ID < firstSect) {
		start = frontOffset * ID;
		end = start + frontOffset;
	} else {
		start = (frontOffset * firstSect) + (backOffset * (ID - firstSect));
		end = start + backOffset;
	}


	//To keep track of the sum in the thread and number of ints
	double localDiffSqr = 0;
	int localNumInts = 0;
	double temp;
	//Find the sum of the differences^2
	for (int i = start; i < end; i++) {
		temp = ((double)integers[i] ) - globalMean;
		localDiffSqr += pow(temp, 2.0);
		localNumInts++;
	}

	//Using the differences^2 calculate the StDev of the thread
	double localVar = localDiffSqr / (double)localNumInts;
	double localStdev = sqrt(localVar);

	//Critical section
	pthread_mutex_lock(&mutexStdev);

	cout << "Thread " << (ID + 1) << ": " << fixed << setprecision(2) << localStdev << endl;
	//For debugging
	// cout << "Thread " << (ID + 1) << "localSum: " << localSum << endl;
	// cout << "Thread " << (ID + 1) << "localNum: " << localNumInts << endl;

	//Update global sum of differences ^2
	globalDiffSqr += localDiffSqr;

	pthread_mutex_unlock(&mutexStdev);

	pthread_exit(0);
	return NULL;
}


//Main Function
//Takes in a text file and number of threads from command line
// Then uses threads to calculate the mean and StDev of each thread
//	and uses that info to calculate the mean and StDev of the whole file
int main(int argc, char* argv[]){
	// Check args
	if (argc != 3) {
		cout << "Usage: ./stats [text file] [number of threads]" << endl;
		exit(1);
	}

	// Get params
	char *fileName = argv[1];
	// string fileName(f);	//Cast suffix to string

	//Get the number of threads
	maxThreads = atoi(argv[2]);

	//Open file and read ints into an array
	ifstream f(fileName);

	globalNumInts = 0;
	string line;

	//Read in the ints into integers[]
	while (getline(f, line)) {
			// int i = stoi(line);
			integers[globalNumInts] = stoi(line);
			globalNumInts++;
	}
	f.clear();

	//Init an array of threads
	pthread_t threadArray[maxThreads];

	//Create threads to get mean's
	for(long i = 0; i < maxThreads; i++){
		long status = pthread_create(&threadArray[i], NULL, getMean, (void*) i);
		if(status){
			printf("Error in creating thread! \n");
			exit(0);
		}
	}
	//Join threads
	for(long i = 0; i < maxThreads; i++){
		pthread_join(threadArray[i], NULL);
	}

	//Calculate the mean of all numbers
	globalMean = (double)globalSum / (double)globalNumInts;

	cout << "Mean = " << fixed << setprecision(2) << globalMean<< endl;

	//Create threads to get Stdev's
	for(long i = 0; i < maxThreads; i++){
		long status = pthread_create(&threadArray[i], NULL, getStdev, (void*) i);
		if(status){
			printf("Error in creating thread! \n");
			exit(0);
		}
	}
	//Join threads
	for(long i = 0; i < maxThreads; i++){
		pthread_join(threadArray[i], NULL);
	}

	//Calculate the Standard deviation of the whole file
	globalStdev = sqrt(globalDiffSqr / (double) globalNumInts);

	cout << "StDev = " << fixed << setprecision(2) << globalStdev << endl;

	return 0;
}
