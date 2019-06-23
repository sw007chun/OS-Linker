/*
 * Tokenizer.h
 *
 *  Created on: Jun 19, 2019
 *      Author: sungw
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

class Tokenizer
{
private:
	ifstream myfile;	//file stream
	string lineStr;		//string of current line
	char *curToken;		//current token
	char *nextToken;	//next token
	char *curLine;		//current line
	int lineNum;		//current line number
	int lineOffset;		//front offset value of current token
public:
	Tokenizer(char * filename);
	void init(); //initializing class variables
	void resetFile(); //resetting file stream pointer to the beginning and reinitializing class variables
	char * getToken();	//getting token from the file stream
	int readInt(bool def = false);	//reading integer value. init parameter is to find if it is eof when trying to read defcount
	char * readSymbol();	//reading character for symbol
	char readIAER();	//reading instruction
	void parseError(int errCode);	//parse error messages
	void instError(int errcode, string symbol = ""); //instruction error messages. symbol parameter for undefined value symbol name
};
#endif /* TOKENIZER_H_ */
