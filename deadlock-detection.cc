/*
 * Filename       deadlock-detection.cc
 * Date           3/8/2020
 * Author         Dylan Kreth
 * Email          dylan.kreth@utdallas.edu
 * Course         CS 4348.001 Spring 2020
 * Copyright      2020, All Rights Reserved
 * Procedures:
 * 		main - performs a deadlock detection algorithm on a set of given data
 * 		getFileName - returns the file name provided by the user, or returns the default if no name is provided
 * 		customOpen - returns the file name provided by the user, or returns the default if no name is provided
 * 		clearcin - clears the cin buffer
 * 		parseVectorFile - parseMatrixFile - parses the file and returns a vector with the contents of the file
 * 		parses the file and returns a vector with the contents of the file
 * 		markSatisfiedProcess - Mark each process that has a row in the Allocation matrix of all zeros.
 * 		sufficientResources - determines whether or not there are sufficient resources of each resource of available vector to satisfy request of process p in request matrix
 * 		increment - add values of each resource of process p in allocationMatrix to w
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
	if (argc < 3) 																						// filename, numResources, and numProcesses
	{
		cerr << "too few command arguments provided!" << endl;
		exit(1);	 																					// if there aren't enough command line args, exit bc algorithm cannout execute
	}


	int numResources = atoi(argv[1]); 																	// get the number of resources from the command line arg as an int

	int numProcesses = atoi(argv[2]); 																	// get the number of processes from the command line arg as an int

	cout << "=====================SETUP=======================" << endl; //style

	ifstream requestMatrixFile;																			// ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(requestMatrixFile,"Request Matrix", "reqMat.txt"); 										//opens the file based on user prompt
	vector <vector <int> > requestMatrix(numResources, vector<int>(numProcesses)); 						//define the vector for the requestMatrix
	requestMatrix = parseMatrixFile(requestMatrixFile, numProcesses, numResources); 					// fill the vector with data from the file


	ifstream allocationMatrixFile; 																		// ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(allocationMatrixFile, "Allocation Matrix", "allocMat.txt");								//opens the file based on user prompt
	vector <vector <int> > allocationMatrix(numResources, vector<int>(numProcesses)); 					//define the vector for the requestMatrix
	allocationMatrix = parseMatrixFile(allocationMatrixFile, numProcesses, numResources); 				// fill the vector with data from the file

	/* the below code can be used if the avaible vector file is provided */
				// ifstream resourceVectorFile; // ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
				// customOpen(resourceVectorFile, "Resource Vector", "resVect.txt"); //opens the file based on user prompt
				// vector <int> resourceVector(numResources); //declare vector of proper size
				// resourceVector = parseVectorFile(resourceVectorFile, numResources); // fill the vector with data from the file

	ifstream availableVectorFile; 																		// ifstream needs to be passed by reference instead of being returned bc there is no copy constructor
	customOpen(availableVectorFile, "Available Vector", "availVect.txt");								//opens the file based on user prompt
	vector <int> availableVector(numResources); 														//declare vector of proper size
	availableVector = parseVectorFile(availableVectorFile, numResources); 								// fill the vector with data from the file

	cout << "\n=====================SPECS=======================" << endl; 								//style

	cout << "Program started with the following specs:" << endl; 										//inform user
	cout << "Number of Resources: " << numResources << endl; 											//inform user
	cout << "Number of Processes: " << numProcesses << endl; 											//inform user



	/* prints the contents of the request matrix */
	cout << endl << "Request matrix Q:" << endl; 														// header before printing data
	for(int process = 0; process < numProcesses; process++){ 											//for each process...
		for(int resource = 0; resource < numResources; resource++){ 									// ...go thru each resource and print the value
			cout << requestMatrix[resource][process] << " " << flush;
		}
		cout << endl; 																					//style
	}

	/* prints the contents of the allocation matrix */
	cout << endl << "Allocation matrix A:" << endl; 													// header before printing data
	for(int process = 0; process < numProcesses; process++){ 											// for each process...
		for(int resource = 0; resource < numResources; resource++){ 									// ...go thru each resource and print the value
			cout << allocationMatrix[resource][process] << " " << flush;
		}
		cout << endl; 																					//style
	}

	/* prints the contents of the resource vector */
				// cout << endl << "Resource vector:" << endl;
				// for(int resource = 0; resource < numResources; resource++){
				// 	cout << resourceVector[resource] << " " << flush;
				// }
				// cout << endl;


	/* prints the contents of the available vector */
	cout << endl << "Available vector:" << endl; 														// header before printing data
	for(int resource = 0; resource < numResources; resource++){ 										// for each resource...
		cout << availableVector[resource] << " " << flush; 												// ...go thru and print the value
	}
	cout << endl;																						//style


	bool processMarked[numProcesses]; 																	// keeps track of which processes are marked
	for (int i = 0; i < numProcesses; i++) 																// go thru each process...
	{
		processMarked[i] = false; 																		//... and set each to false (unmarked) to start
	}

	markSatisfiedProcesses(allocationMatrix, processMarked, numProcesses, numResources); 				// A process that has no allocated resources cannot participate in a deadlock
	/* prints the processMarked array for manual verification */
				// for(int i = 0; i<numProcesses; i++)
				// 	cout << processMarked[i] << " " << flush;

	vector<int> w = availableVector; 																	// Initialize a temporary vector W equal the Available vector.
	/* prints the W vector for manual verification */
				// for(int i = 0; i<numResources; i++)
				// 	cout << w[i] << " " << flush;

	/*
	* Find an index i such that process i is currently unmarked and the ith row of Q
	* is less than or equal to W. That is, Qik … Wk, for 1 … k … m. If no such row is
	* found, terminate the algorithm.
	* If such a row is found, mark process i and add the corresponding row of the allocation matrix to W. That is, set Wk = Wk + Aik, for 1 … k … m. Return to step 3
	*/
	bool needToRepeat; 																					//flipped to true if any changes are made
	do {
		needToRepeat = false;
		for(int process = 0; process < numProcesses; process++){ 										// check each process
			if(!processMarked[process] && sufficientResources(requestMatrix, w, process, numResources)){// find a process that is currently unmarked for which there are enough resources to satisfy the request
				processMarked[process] = true; 															//mark that process
				increment(w, allocationMatrix, process, numResources);									// free the resources in the allocation matrix that correspond to the marked process and add them to w
				needToRepeat = true; 																	// repeat, skipping this process from now on
				break;																					// start over
			}
		}
	} while(needToRepeat); 																				// keep repeating until there are no unmarked processes that can be satisfied

	int unmarkedCount = 0; 																				// start with no unmarked processes
	for(int process = 0; process < numProcesses; process++){ 											// go thru each process...
		if(!processMarked[process])
			unmarkedCount++; 																			// ...and count how many are unmarked
	}

	cout << "\n=====================RESULTS=======================" << endl; 							//style
	cout << "Number of processes left unmarked: " << unmarkedCount << endl;								// header before printing data
	if(unmarkedCount == 0)
		cout << "Therefore, no deadlock exists!" << endl; 												// all processes marked, so no deadlock
	else {
		cout << "Therefore, deadlock exists!" << endl; 													// unmarked processes, so deadlock
		cout << "Processes caught in deadlock: " << flush; 												// header before printing
		for(int i = 0; i < numProcesses; i++){
			if(!processMarked[i])
				cout << "P" << i+1 << " " << flush; 													// print the unmarked process (they're the ones in deadlock)
		}
		cout << endl;																					//style
	}

	cout << "Algorithm completed successfully." << endl; 												// tell the user we're done

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
	string fileName; 																					//to fill
	if (cin.peek() == '\n') 																			//if user presses enter...
	{
		fileName = defaultName; 																		// ...use the dafault name
	}
	else
	{
		cin >> fileName; 																				// otherwise, read from the command line
	}

	clearcin(); 																						//clears the \n character out of the cin buffer

	return fileName; 																					// we need the file name
}

/***************************************************************************
* void customOpen
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: returns the file name provided by the user, or returns the default if no name is provided
* Parameters:
* formalName I/P the name to be used in the user prompt (ex. "Request Matrix")
* defaultFileName I/P the name of the file to open if no name is provided to cin (ex. "reqMat.txt")
**************************************************************************/
void customOpen(ifstream& filestream, string formalName, string defaultFileName)
{

	/* print prompt */
	cout << "Enter the name of the file that contains the "
		 << formalName 																					//ex. Request Matrix
		 << " [default = "
		 << defaultFileName 																			// ex. reqMat.txt
		 << "]: "
		 << flush; 																						//print, but no newline char

	string fileName = getFileName(defaultFileName); 													// we need the file name to open the file
	cout << "Opening file " << fileName << endl;														//notify the user in case of typo
	filestream.open(fileName.c_str());																	//open the file so we can read it

	/*if the file fails to open, close the program */
	if (!filestream)
	{
		cerr << "Unable to open "
			 << formalName
			 << " file ("
			 << fileName
			 << "). Aborting."
			 << endl;
		exit(1); 																						//exit bc program is hosed
	}

}

/***************************************************************************
* void clearcin
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: clears the cin buffer
**************************************************************************/
void clearcin()
{
	string buf;		   																					//create a buffer to hold the \n
	getline(cin, buf); 																					//grab the \n from the command line
}

/***************************************************************************
* vector<vector<int>> parseMatrixFile
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: parses the file and returns a vector with the contents of the file
* Parameters:
* ifstream I/P the file to be read from (pass by reference)
* numProcesses I/P number of processes
* numResources I/P number of resources
* requestMatrix O/P vector that holds the matrix that was parsed from the file
**************************************************************************/
vector<vector<int> > parseMatrixFile(ifstream& file, int numProcesses, int numResources){
	if(!file)
		exit(1); 																						//if file isn't open, program is hosed so just exit
	vector <vector <int> > matrix(numResources, vector<int>(numProcesses)); 							//declare vector of proper size
	for(int process = 0; process < numProcesses; process++){ 											// go thru each process-
		for(int resource = 0; resource < numResources; resource++){ 									// -resource pair and 
			file >> matrix[resource][process]; 															//fill the vector with matrix data from file
		}
	}

	return matrix; 																						//we need the generated matrix

}

/***************************************************************************
* vector<int> parseVectorFile
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
		exit(1); 																						//if file isn't open, program is hosed so just exit
	vector <int> vect(size); 																			//declare vector of proper size
	for(int val = 0; val < size; val++){ 																//fill the vector with matrix data from file
		file >> vect[val];
	}

	return vect; 																						//we need this vector

}

/***************************************************************************
* void markSatisfiedProcesses
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: Mark each process that has a row in the Allocation matrix of all zeros.
* Parameters:
* allocationMatrix I/P the allocation matrix being checked
* processMarked[] I/O array that says which processes are marked
* numProcesses I/P number of processes
* numResources I/P number of resources
**************************************************************************/
void markSatisfiedProcesses(vector <vector <int> > allocationMatrix, bool processMarked[], int numProcesses, int numResources) {
	for(int process = 0; process < numProcesses; process++){
		// if(processMarked[process]) // optimization: skip any processes that are already marked
		// 	continue;
		bool allZero = true; // assume all zero
		for(int resource = 0; resource < numResources; resource++){ 									// iterate thru each resource in a given process
			if(allocationMatrix[resource][process] != 0)
				allZero = false; 																		// change assumption if we find a non-zero resource in the process
		}
		processMarked[process] = allZero; 																// mark this process if all its resources were zero
	}


}

/***************************************************************************
* bool sufficientResources
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: determines whether or not there are sufficient resources of each resource of available vector to satisfy request of process p in request matrix
* Parameters:
* requestMatrix I/P matrix with the resources being requested
* availableVector I/P vector with which resources are currently available
* process I/P index of process we want to check on
* numResources I/P total number of different resources in the system
* sufficientResources O/P whether or not there are sufficient resources of each resource of available vector to satisfy request of process p in request matrix
**************************************************************************/
bool sufficientResources(vector <vector <int> > requestMatrix, vector <int> availableVector, int process, int numResources){
	for(int resource = 0; resource < numResources; resource++){ 										// for each resource in the given process...
		if(requestMatrix[resource][process] > availableVector[resource]) 								// ...check if what we're asking for is more than what we have
			return false; 																				// return false if we find a resource for which there is an insufficiency in the available vector
	}
	return true; 																						// if each is sufficient then they are all collectively sufficient
}

/***************************************************************************
* void increment
* Author: Dylan Kreth
* Date: 3/8/2020
* Description: add values of each resource of given process in allocationMatrix to w
* Parameters:
* w I/O matrix with available values of each resource
* allocationMatrix I/P matrix with allocation values of each process-resource pair
* process I/P index of process we want to check on
* numResources I/P total number of different resources in the system
**************************************************************************/
void increment(vector<int> &w, vector <vector <int> > allocationMatrix, int process, int numResources){
	for(int resource = 0; resource < numResources; resource++){ 										//for each resource in the given process
		w[resource] = allocationMatrix[resource][process] + w[resource]; 								// increase the value (of that resource) in w by the value of the same resource in the allocation matrix
	}
}
