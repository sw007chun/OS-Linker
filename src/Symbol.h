/*
 * Symbol.h
 *
 * Symbol class
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <iostream>
using namespace std;

class Symbol {
private:
	char * sym;	//symbol
	int val;	//symbol's absolute address
	bool multiDef;	//if symbol has been defined multiple times
	bool used;		//if defined symbol has beend used
public:
	Symbol(char * symbol, int val);	//constructor
	int getVal();	//get the symbol's address
	void setVal(int setValue);	//set the symbol's address if there is an error
	int getDef();	//check if symbol has been defined
	void setDef();	//set true is symbol has been defiend already
	bool isUsed();	//check if defined symbol has been used
	void setUsed();	//set true is symbol has been sued
};
#endif /* SYMBOL_H_ */
