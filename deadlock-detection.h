/*
 * Filename       deadlock-detection.h
 * Date           3/8/2020
 * Author         Dylan Kreth
 * Email          dylan.kreth@utdallas.edu
 * Course         CE 4348.001 Spring 2020
 * Copyright      2020, All Rights Reserved
 */

#include <string.h>
#include <fstream>
#include <vector>

#ifndef __DEADLOCK_H__
#define __DEADLOCK_H__

#define DEBUG 0 //switch to 1 for DEBUG output to stdout
#define DEBUG2 0
#define DEBUG3 0

std::string getFileName(std::string defaultName);
int customOpen(std::ifstream& filestream, std::string formalName, std::string defaultFileName);
void clearcin();
std::vector<std::vector<int>> parseMatrixFile(std::ifstream& file, int numProcesses, int numResources);
std::vector<int> parseVectorFile(std::ifstream& file, int size);
void performStep1(std::vector <std::vector <int>> allocationMatrix, bool processMarked[], int numProcesses, int numResources);


#endif /* __DEADLOCK_H__ */
