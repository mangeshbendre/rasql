/***************************************************************************
                          LALR.h  -  LALR automata
                          ------------------------
    begin                : Thu Jun 13 2002
    copyright            : (C) 2002 by Manuel Astudillo
    email                : manuel.astudillo@scalado.com
    linux support added  : 2003 by Pablo Garcia <pgarcia@web.de>
    several bugfixes     : 2010 by Elmü <elmue@gmx.de>
 ***************************************************************************/

 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

 #ifndef _GP_LALR_H
 #define _GP_LALR_H

 #include "Misc.h" 
 
 namespace Astudillo
 {
 
	 enum Actions {ACTION_SHIFT = 1, ACTION_REDUCE = 2, ACTION_GOTO = 3, ACTION_ACCEPT = 4};
	 enum Reductions {REDUCTION_OK, REDUCTION_ERROR, REDUCTION_TEXT_ACCEPTED,
	                  REDUCTION_TOKEN_SHIFT, REDUCTION_COMPLETED, REDUCTION_SIMPLIFIED};
	
	
	 class LALR 
	 {
	 public:
	   LALR (const LALRStateTable *stateTable, const SymbolTable *symbolTable,
	   const RuleTable *ruleTable, UBYTE2 startState);
	
	    /*!
	     setup the parsing engine.
	   */
	   void init (const vector <Token*> &tokens);
	
	   /*
	     parses just until ONE reduction is performed
	   */
	   Symbol *nextReduction (bool trimReductions, bool reportOnlyOneError);
	
	   /*!
	     Gets the result constant for the last reduction attempt.
	   */
	   int getResult();
	
	   /*!
	     builds a parse tree with Reductions as nodes.
	     This tree is supposed to be the input to some abstract tree creator.
	     Every Reduction node has a list of tokens, and every token a pointer to
	     its correspondent reduction node.
	   */
	   Symbol *parse (const vector <Token*> &tokens, bool trimReductions, bool reportOnlyOneError);
	
	   void printRule (UBYTE2 rule);
	
	   ErrorTable *getErrors ();
	   const SymbolTable* getSymbols();
	
	 private:
	   Action *getNextAction (UBYTE2 symbolIndex, UBYTE2 index);
	
	   vector<wstring> getPossibleTokens (UBYTE2 index);
	
	   Symbol *parseToken (Action *actObj, SymbolStack &theStack, int tokenIndex, 
	                       UBYTE2 &currentState);
	   Terminal *createTerminal (Token *tok);
	   NonTerminal *createNonTerminal (int index, int target);
	
	   // Member variables
	   const LALRStateTable *stateTable;
	   const RuleTable *ruleTable;
	   const SymbolTable *symbolTable;
	
	   UBYTE2 startState;
	   UBYTE2 currentState;
	
	   bool m_trimReductions;
	
	   UBYTE2 currentLine, currentCol;
	
	   UBYTE2 tokenIndex;
	   vector<Token*> tokens;
	
	   SymbolStack symbolStack;
	
	   Symbol   *prevReduction; // last correctly reduced non terminal
	   Terminal  lastTerminal;  // last parsed symbol
	
	   int reductionResult;
	
	   ErrorTable ErrTable;
	
	   bool trim;
	 };
 
 } // namespace

 #endif

