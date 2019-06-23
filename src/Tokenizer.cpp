#include <iostream>
#include "Tokenizer.h"
using namespace std;

Tokenizer::Tokenizer(char * filename) {
	myfile.open(filename);
	init();
}
void Tokenizer::init() { //initializing class variables
	lineNum = 1;
	lineOffset = 1;
	getline(myfile, lineStr);
	curLine = new char[lineStr.length() + 1]; //char array to use strtok
	strcpy(curLine, lineStr.c_str());	//copying string to cstring to use strtok
	curToken = NULL;
	nextToken = strtok(curLine, "\t \n");	//getting next token in the line
}
void Tokenizer::resetFile() { //resetting file stream pointer to the beginning and reinitializing class variables
	myfile.clear();
	myfile.seekg(myfile.beg);	//set file pointer to beginning
	init();
}
char * Tokenizer::getToken() {	//getting token from the file stream
	if (nextToken != NULL) {
		curToken = nextToken;
	} else if (!myfile.eof()){	//if it's not eof
		if (curToken != NULL) //this is exception case for counting line offset when parse error occurs at eof
			lineOffset = lineStr.length() + 1; //

		getline(myfile, lineStr);
		lineNum++;
		while (lineStr.find_first_not_of(' ') - string::npos == 0 && !myfile.eof()) {	//get a new line until it isn't a empty line and not eof
			getline(myfile, lineStr);
			lineNum++;
			lineOffset = 1;
		}
		curLine = new char[lineStr.length() + 1];
		strcpy(curLine, lineStr.c_str());
		curToken = strtok(curLine, "\t \n");
		if (lineStr.length() != 0)	//sometimes there is line with no token but spaces
			lineOffset = lineStr.length() + 1;
		if (myfile.eof() && lineStr.length() == 0) {	//if we have read eof reset line number
			lineNum--;
		}
	} else {	//if line ends with \n set off the to end of line
		curToken = NULL;
		lineOffset = lineStr.length();
	}
	nextToken = strtok(NULL, "\t \n");	//getting the next token in the line
	if (curToken != NULL)
		lineOffset = curToken - curLine + 1;	//counting line offset from the difference between line pointer and token pointer
	return curToken;
}
int Tokenizer::readInt(bool def) {	//reading integer value. init parameter is to find if it is eof when trying to read defcount
	char * token = getToken();
	if (token == NULL) {
		if (myfile.eof() && def)	//this is to check if there is no more def list and has reach eof to break the while loop
			throw -1;
		else
			throw 0;	//can't find number
	}
	string str(token);
	string::iterator it;

	for (it = str.begin(); it != str.end(); it++)
		if(!isdigit(*it)) throw 0;	//check if there is any character in the token before converting it to integer

	return atoi(token);
}
char * Tokenizer::readSymbol() {	//reading character for symbol
	char * token = getToken();
	if (token == NULL)	//SYMBOL EXPECTED
		throw 1;

	int charCount = 0;
	string str(token);
	string::iterator it = str.begin();

	if(!isalpha(*it))	//check if symbol starts with [a-Z]
		throw 1;

	it++;
	charCount++;

	while (it != str.end()) {
		if (!isdigit(*it) && !isalpha(*it))	//checks if symbol is followed by [a-Z0-9]
			throw 1;
		else if (++charCount > 16)			//checks if symbol is longer than 16 characters
			throw 3;
		it++;
	}
	return token;
}
char Tokenizer::readIAER() {	//reading instruction
	char * addr = getToken();
	if (addr == NULL || ( *addr != 'I' && *addr != 'E' && *addr != 'A' && *addr != 'R'))
		throw 2;
	return *addr;
}
void Tokenizer::parseError(int errCode) {	//parse error messages
	static string perrstr[] = {
		"NUM_EXPECTED", // Number expect
		"SYM_EXPECTED", // Symbol Expected
		"ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
		"SYM_TOO_LONG", // Symbol Name is too long
		"TOO_MANY_DEF_IN_MODULE", // > 16
		"TOO_MANY_USE_IN_MODULE", // > 16
		"TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
	};

	cout << "Parse Error line " << lineNum << " offset " << lineOffset << ": " << perrstr[errCode] << endl;
}
void Tokenizer::instError(int errcode, string symbol) { //instruction error messages. symbol parameter for undefined value symbol name
	static string ierrstr[] = {
		"Absolute address exceeds machine size; zero used",
		"Relative address exceeds module size; zero used",
		"External address exceeds length of uselist; treated as immediate",
		" is not defined; zero used",
		"This variable is multiple times defined; first value used",
		"Illegal immediate value; treated as 9999",
		"Illegal opcode; treated as 9999"
	};
	cout << " Error: " << symbol << ierrstr[errcode] << endl;
}
