/*
 * Main Linker Function
 *
 */

#include <iostream>
#include <map>
#include <iomanip>
#include "Tokenizer.h"
#include "Symbol.h"
using namespace std;

int main(int argc, char *argv[]) {
	Tokenizer * tok = new Tokenizer(argv[1]);
	map<string, Symbol *> symbols;	//map for storing symbol information
	map<string, Symbol *>::iterator sItr;
	map<int, int> modules;	//map for storing cumulative module sizes
	int totalInstCount = 0;	//count total number of instructions
	int moduleCount = 0;	//count current module number
	modules[0] = 0;

	//Starting Pass 1

	while (1) {
		try {
			int defCount = tok->readInt(true);	//read number of definitions
			if (defCount > 16) //check if there is more than 16 symbol definitions
				throw 4;

			for (int i = 0; i < defCount; i++) {
				char * symbol = tok->readSymbol();	//reading symbol
				int val = tok->readInt();			//reading symbol address

				if (symbols.find(symbol) == symbols.end()) {	//if symbol is not found in the map add it
					symbols[symbol] = new Symbol(symbol, modules[moduleCount] + val);
				} else {	//if symbol if found set multiDef as true;
					symbols[symbol]->setDef();
				}
			}
			int useCount = tok->readInt();	//read use list count
			if (useCount > 16)	//check if there is more than 16 symbol definitions
				throw 5;

			for (int i = 0; i < useCount; i++) {	//at Pass 1 just read use list symbols and skip it
				tok->readSymbol();
			}

			int instCount = tok->readInt();	//read number of instructions
			totalInstCount += instCount;
			if (totalInstCount > 512)	//if cumulative definitions are over 512 throw and error
				throw 6;

			for (int i = 0; i < instCount; i++) {
				tok->readIAER();	//read instruction type
				tok->readInt();		//read intruction
			}

			modules[moduleCount + 1] = modules[moduleCount] + instCount;	//calculate cumulative module address
			moduleCount++;

			for (sItr = symbols.begin(); sItr != symbols.end(); sItr++) {
				if ( symbols[sItr->first]->getVal() >= modules[moduleCount] ) {	//if symbols absolute address is larger than modules culumative address file an warning
					cout << "Warning: Module " << moduleCount << ": " << sItr->first << " too big " << symbols[sItr->first]->getVal() << " (max=" << instCount - 1 << ") assume zero relative" << endl;
					symbols[sItr->first]->setVal(modules[moduleCount-1]);	//set value of symbol as modules base absolute address
				}
			}
		} catch (int errCode) {
			if (errCode < 0 ) {
				break;	//this error occurs only at the beginning of module to break the while loop by setting parameter of readInt as true
			} else {
				tok->parseError(errCode);	//other errors are parse errors thus after error filing exit the progrma
				return 0;
			}
		}
	}

	//rest file pointer to the beginning and reinitialize class variables
	tok->resetFile();

	cout << "Symbol Table" << endl;

	for (sItr = symbols.begin(); sItr != symbols.end(); sItr++) {
		cout << sItr->first << "=" << symbols[sItr->first]->getVal();	//printing symbol and its absolute address
		if (symbols[sItr->first]->getDef())	//if symbol has been defined multiple times print warning
			tok->instError(4);
		else
			cout << endl;
	}

	cout << "\nMemory Map" << endl;

	moduleCount = 0;
	totalInstCount = 0;

	int operand;
	int opcode;

	//Pass 2 starts here

	while (1) {
		try {
			int defCount = tok->readInt(true);

			for (int i = 0; i < defCount; i++) {	//at Pass 2 skip definitions
				tok->readSymbol();
				tok->readInt();
			}

			int useCount = tok->readInt();
			map<int, char *> useMap;	//useMap is map to store use list position to symbol string
			map<int, bool> useUse;		//useUse is to check if symbols in use list has been used or not
			map<int, bool>::iterator useItr;
			for (int i = 0; i < useCount; i++) {	//storing symbol information to the maps
				useMap[i] = tok->readSymbol();
				useUse[i] = false;
			}

			int instCount = tok->readInt();

			for (int i = 0; i < instCount; i++) {
				try {
					char addressMode = tok->readIAER();
					operand = tok->readInt();	//reading instruction and separating opcode and operand
					opcode = operand/1000;
					operand %= 1000;

					cout << setw(3) << setfill('0') << i + modules[moduleCount] <<": ";	//writing instruction numbers
					cout << setw(4) << setfill('0');

					if (opcode >= 10) {	//if instruction is over 5 digits
						opcode = 9999;
						if (addressMode == 'I')	//rule 10
							throw 5;
						else	//rule 11
							throw 6;
					} else {
						opcode *= 1000;

						switch (addressMode) {
						case 'A' :
							if (operand >= 512)	//rule 8
								throw 0;
							else
								opcode += operand;

							break;
						case 'R' :
							if (operand > instCount) {	//rule 9
								opcode += modules[moduleCount];
								throw 1;
							} else {
								opcode += operand + modules[moduleCount];
							}
							break;
						case 'E' :
							if (operand >= useCount) {	//rule 6
								opcode += operand;
								throw 2;
							} else if (symbols.find(useMap[operand]) == symbols.end()) { //rule 3
								useUse[operand] = true;
								throw useMap[operand];
							} else {
								useUse[operand] = true;
								symbols[useMap[operand]]->setUsed();
								opcode += symbols[useMap[operand]]->getVal();
							}
							break;
						case 'I' :
							opcode += operand;
							break;
						}
						cout << opcode << endl;	//printing memory address
					}
				} catch (int errCode) {
					if (errCode < 0 ) {
						break;
					} else {
						cout << opcode;
						tok->instError(errCode);
					}
				} catch (char * symbol) {	//this is only for rule 3 in order to pass the symbol name
					cout << opcode;
					tok->instError(3, symbol);
				}
			}
			moduleCount++;
			for (useItr = useUse.begin(); useItr != useUse.end(); useItr++) {
				if (!useItr->second)	//check if all the symbols in the use list has been used
					cout << "Warning: Module "<< moduleCount <<": "<< useMap[useItr->first] <<" appeared in the uselist but was not actually used" << endl;

			}
		} catch (int errCode) { //this is to break the while loop
			if (errCode < 0 )
				break;
		}
	}

	cout << endl;
	moduleCount = 1;

	for (sItr = symbols.begin(); sItr != symbols.end(); sItr++) {
		while (modules[moduleCount] <= symbols[sItr->first]->getVal())	//this is to get the module number based on symbol's absolute addresss
			moduleCount++;
		if (!symbols[sItr->first]->isUsed())	//check if defined symbol has been used
			cout << "Warning: Module "<< moduleCount <<": "<< sItr->first <<" was defined but never used" << endl;
	}
	return 0;
}
