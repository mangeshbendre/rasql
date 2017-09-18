/*
 * RAParser.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: mangesh
 */

#include "RAParser.h"
#include "UTFConverter.h"

RAParser::RAParser() {
	init = false;
	// Load grammar file
	if (cgtFile.load((char *)"ra.cgt")) {
		name = cgtFile.getInfo()->name;
	} else {
		return;
	}

	// Get DFA (Deterministic Finite Automata) from the cgt file
	dfa = cgtFile.getScanner();
	init = true;
}

map<wstring, list<wstring> > RelationAttributes::relationMap;
bool RelationAttributes::haveSchema=false;
list<wstring> RelationAttributes::emptyList;


int RAParser::parseRA(string ra_string) {
	if (!init)
	{
		ErrorMessage = "Parser not initialized";
		return -1;
	}

	// Scan the source in search of tokens
	dfa->scan(ra_string.c_str());

	// Get the error table
	dfaErrors = dfa->getErrors();

	// If there are errors report them
	ErrorMessage = "";
	if (dfaErrors->errors.size() > 0) {

		for (unsigned int i = 0; i < dfaErrors->errors.size(); i++) {
			ErrorMessage += myReporter.composeErrorMsg(*dfaErrors->errors[i]);
			ErrorMessage += "\n";
		}
		return -1;
	}

	// Get the tokens to feed the LALR machine with them
	vector<Token*> tokens = dfa->getTokens();

	// Get the LALR (Look Ahead, Left-to-right parser, Rightmost-derivation)
	LALR* lalr = cgtFile.getParser();

	// Parse the tokens
	Symbol* firstReduction = lalr->parse(tokens, true, true);

	ErrorTable* lalrErrors = lalr->getErrors();
	if (lalrErrors->errors.size() > 0) {
		for (unsigned int i = 0; i < lalrErrors->errors.size(); i++) {
			ErrorMessage += myReporter.composeErrorMsg(*lalrErrors->errors[i]);
			ErrorMessage += "\n";
		}
		return -1;
	}

	ExecuteValue kResult;
	MyExecuteData kData;

	if (((SymbolBase*) firstReduction)->Execute(&kResult, &kData))
	{
		Result = kResult.String;
		return 0;
	}
	else
	{
		ErrorMessage += kData.Message;
		return -1;
	}
}

int RAParser::addRelationAttributes(wstring relation, list<wstring> attributes)
{
	RelationAttributes::addRelationAttributes(relation, attributes);
	return 0;
}

int RAParser::clearRelationAttributes()
{
	RelationAttributes::clearRelationAttributes();
	return 0;
}


string RAParser::getResult()
{
	return wstrtostr(Result);
}

RAParser::~RAParser()
{

}

