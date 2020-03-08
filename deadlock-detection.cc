/*
 * Filename       deadlock-detection.cc
 * Date           3/8/2020
 * Author         Dylan Kreth
 * Email          dylan.kreth@utdallas.edu
 * Course         CS 4348.001 Spring 2020
 * Copyright      2020, All Rights Reserved
 * Procedures:
 * main - performs a deadlock detection algorithm on a set of given data
 * //TODO fill in rest of methods
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <vector>
#include "deadlock-detection.h"

using namespace std;

/***************************************************************************
* int main( int argc, char **argv )
* Author: Dylan Kreth
* Date: 3/8/2020
* Description:  performs a deadlock detection algorithm on a set of given data
*
* Parameters:
* argc I/P int The number of arguments on the command line
* argv I/P char *[] The arguments on the command line. 
* 		argv[1] shall be an int that defines the number of resources
* 		argv[2] shall be an int that defines the number of processes
* main O/P int Status code (not currently used)
**************************************************************************/
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "too few command arguments provided!" << endl;
		return 1; //
	}


	int numResources = atoi(argv[1]);

	int numProcesses = atoi(argv[2]);


	ifstream requestMatrixFile; // ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(requestMatrixFile,"Request Matrix", "reqMat.txt"); //opens the file based on user prompt
	vector <vector <int>> requestMatrix(numResources, vector<int>(numProcesses)); //define the vector for the requestMatrix
	requestMatrix = parseMatrixFile(requestMatrixFile, numProcesses, numResources); // fill the vector with data from the file


	ifstream allocationMatrixFile; // ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(allocationMatrixFile, "Allocation Matrix", "allocMat.txt"); //opens the file based on user prompt
	vector <vector <int>> allocationMatrix(numResources, vector<int>(numProcesses)); //define the vector for the requestMatrix
	allocationMatrix = parseMatrixFile(allocationMatrixFile, numProcesses, numResources); // fill the vector with data from the file

	// the below code can be used if the avaible vector file is provided
	// ifstream resourceVectorFile; // ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	// customOpen(resourceVectorFile, "Resource Vector", "resVect.txt"); //opens the file based on user prompt
	// vector <int> resourceVector(numResources); //declare vector of proper size
	// resourceVector = parseVectorFile(resourceVectorFile, numResources); // fill the vector with data from the file

	ifstream availableVectorFile; // ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(availableVectorFile, "Available Vector", "availVect.txt"); //opens the file based on user prompt
	vector <int> availableVector(numResources); //declare vector of proper size
	availableVector = parseVectorFile(availableVectorFile, numResources); // fill the vector with data from the file


	cout << "Program started with the following specs:" << endl;
	cout << "Number of Resources: " << numResources << endl;
	cout << "Number of Processes: " << numProcesses << endl;



	// prints the contents of the request matrix
	cout << endl << "Request matrix Q:" << endl;
	for(int process = 0; process < numProcesses; process++){
		for(int resource = 0; resource < numResources; resource++){
			cout << requestMatrix[resource][process] << " " << flush;
		}
		cout << endl;
	}

	// prints the contents of the allocation matrix
	cout << endl << "Allocation matrix A:" << endl;
	for(int process = 0; process < numProcesses; process++){
		for(int resource = 0; resource < numResources; resource++){
			cout << allocationMatrix[resource][process] << " " << flush;
		}
		cout << endl;
	}

	// // prints the contents of the resource vector
	// cout << endl << "Resource vector:" << endl;
	// for(int resource = 0; resource < numResources; resource++){
	// 	cout << resourceVector[resource] << " " << flush;
	// }
	// cout << endl;


	// prints the contents of the available vector
	cout << endl << "Available vector:" << endl;
	for(int resource = 0; resource < numResources; resource++){
		cout << availableVector[resource] << " " << flush;
	}
	cout << endl;


	bool processMarked[numProcesses]; // keeps track of which processes are marked
	for (int i = 0; i < numProcesses; i++)
	{
		processMarked[i] = false;
	}

	performStep1(allocationMatrix, processMarked, numProcesses, numResources); // A process that has no allocated resources cannot participate in a deadlock

	// // prints the processMarked array for manual verification
	// for(int i = 0; i<numProcesses; i++)
	// 	cout << processMarked[i] << " " << flush;


	cout << "Success!" << endl;

	return 0;
}

/***************************************************************************
* string getFileName
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: returns the file name provided by the user, or returns the default if no name is provided
* Parameters:
* defaultName I/P the name returned if no name is provided to cin
* fileName O/P the the file name provided by the user, or the defaultName if no name is provided
**************************************************************************/
string getFileName(string defaultName)
{
	string fileName;
	if (cin.peek() == '\n')
	{
		fileName = defaultName;
	}
	else
	{
		cin >> fileName;
	}

	clearcin(); //clears the \n character out of the cin buffer

	return fileName;
}

/***************************************************************************
* string customOpen
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: returns the file name provided by the user, or returns the default if no name is provided
* Parameters:
* formalName I/P the name to be used in the user prompt (ex. "Request Matrix")
* defaultFileName I/P the name of the file to open if no name is provided to cin (ex. "reqMat.txt")
* filestream O/P the the ifstream opened using the computed file name
**************************************************************************/
int customOpen(ifstream& filestream, string formalName, string defaultFileName)
{

	//print prompt
	cout << "Enter the name of the file that contains the "
		 << formalName //ex. Request Matrix
		 << " [default = "
		 << defaultFileName // ex. reqMat.txt
		 << "]: "
		 << flush; //print, but no newline char

	string fileName = getFileName(defaultFileName); // we need the file name to open the file
	// ifstream filestream;							//stream for opening the file
	cout << "Opening file " << fileName << endl;	//notify the user in case of typo
	filestream.open(fileName);						//open the file so we can read it

	//if the file fails to open, close the program
	if (!filestream)
	{
		cerr << "Unable to open "
			 << formalName
			 << " file ("
			 << fileName
			 << "). Aborting."
			 << endl;
		exit(1);
	}
	cout << endl;

	return 1; //return 1 on success
}

/***************************************************************************
* void clearcin
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: clears the cin buffer
**************************************************************************/
void clearcin()
{
	string buf;		   //create a buffer to hold the \n
	getline(cin, buf); //grab the \n from the command line
}

/***************************************************************************
* void parseMatrixFile
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: parses the file and returns a vector with the contents of the file
* Parameters:
* ifstream I/P the file to be read from (pass by reference)
* numProcesses I/P number of processes
* numResources I/P number of resources
* requestMatrix O/P vector that holds the matrix that was parsed from the file
**************************************************************************/
vector<vector<int>> parseMatrixFile(ifstream& file, int numProcesses, int numResources){
	if(!file)
		exit(1); //if file isn't open, program is hosed so just exit
	vector <vector <int>> matrix(numResources, vector<int>(numProcesses)); //declare vector of proper size
	for(int process = 0; process < numProcesses; process++){ //fill the vector with matrix data from file
		for(int resource = 0; resource < numResources; resource++){
			file >> matrix[resource][process];
		}
	}

	return matrix;

}

/***************************************************************************
* void parseVectorFile
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: parses the file and returns a vector with the contents of the file
* Parameters:
* ifstream I/P the file to be read from (pass by reference)
* size I/P number of values in the vector of the specified file
* vect O/P vector that holds the vector that was parsed from the file
**************************************************************************/
vector<int> parseVectorFile(ifstream& file, int size){
	if(!file)
		exit(1); //if file isn't open, program is hosed so just exit
	vector <int> vect(size); //declare vector of proper size
	for(int val = 0; val < size; val++){ //fill the vector with matrix data from file
		file >> vect[val];
	}

	return vect;

}

/***************************************************************************
* void performStep1
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: performs step 1 of the deadlock algorithm. Mark each process that has a row in the Allocation matrix of all zeros.
* Parameters:
* allocationMatrix I/P the allocation matrix being checked
* processMarked[] I/O array that says which processes are marked
* numProcesses I/P number of processes
* numResources I/P number of resources
**************************************************************************/
void performStep1(vector <vector <int>> allocationMatrix, bool processMarked[], int numProcesses, int numResources) {
	for(int process = 0; process < numProcesses; process++){
		// if(processMarked[process]) // optimization: skip any processes that are already marked
		// 	continue;
		bool allZero = true; // assume all zero
		for(int resource = 0; resource < numResources; resource++){ // iterate thru each resource in a given process
			if(allocationMatrix[resource][process] != 0)
				allZero = false; // change assumption if we find a non-zero resource in the process
		}
		processMarked[process] = allZero; // mark this process if all its resources were zero
	}


}