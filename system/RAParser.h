/*
 * RAParser.h
 *
 *  Created on: Mar 27, 2010
 *      Author: mangesh
 */

#include <stdlib.h>
#include <list>
#include "CGTFile.h"
#include "Factory_RA.h"


#ifndef RAPARSER_H_
#define RAPARSER_H_

using namespace Astudillo;

class RAParser {
private:
	wstring name;
	CGTFile cgtFile;
	ErrorReporter myReporter;
	DFA* dfa;
	ErrorTable* dfaErrors;

	wstring Result;
	bool init;

public:
	string ErrorMessage;

	string getResult();
	RAParser();

	int addRelationAttributes(wstring relation, list<wstring> attributes);
	int clearRelationAttributes();
	int parseRA(string ra_string);
	~RAParser();
};

#endif /* RAPARSER_H_ */
