#include <string>
#include <fstream>
#include <cassert>
#include<iostream>
#include<string.h>
#include<stdlib.h>      //exit(),malloc()
using namespace std;
//#pragma warning(disable:4996)
#ifndef IOTXT_H
#define IOTXT_H

using namespace std;

class IoTxt
{
public:
	IoTxt(char* str);
	//Provide an interface to input the absolute path 
	//of txt to the interface in the program

	int txtLine();
	//Get the total number of lines in a txt file, which is 
	//convenient for creating a vector array interface

	void ioOut();
	//Input the txt file to the interface in the program

	//char **store stringArrarCopy();
	//Interface to output private ioOutFile
	char** stringArrayCopy();

private:
	char* strPass;    //The txt fileName

	int line;   //The first line of the txt file

    char **store;   //To store the file content
};


#endif