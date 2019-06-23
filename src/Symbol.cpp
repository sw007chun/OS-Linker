#include "Symbol.h"

Symbol::Symbol (char * symbol, int value) {
	sym = symbol;
	val = value;
	multiDef = false;
	used = false;
}
int Symbol::getVal () {
	return val;
}
void Symbol::setVal (int setValue) {
	val = setValue;
}
int Symbol::getDef () {
	return multiDef;
}
void Symbol::setDef () {
	multiDef = true;
}
bool Symbol::isUsed () {
	return used;
}
void Symbol::setUsed() {
	used = true;
}
