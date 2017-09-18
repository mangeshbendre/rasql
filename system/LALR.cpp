/***************************************************************************
                          LALR.cpp  -  description
                          ------------------------
    begin                : Fri Jun 14 2002
    copyright            : (C) 2002 by Manuel Astudillo
    email                : manuel.astudillo@scalado.com
    linux support added  : 2003 by Pablo Garcia <pgarcia@web.de>
    several bugfixes     : 2010 by Elm� <elmue@gmx.de>
 ***************************************************************************/

 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

 #include "LALR.h"

 #include "defines.h"  // defines FACTORY_FILE
 #include FACTORY_FILE

 using namespace Astudillo; 

 LALR::LALR (const LALRStateTable *stateTable, 
             const SymbolTable *symbolTable,
             const RuleTable *ruleTable, 
             UBYTE2 startState) 
 {
   this->stateTable  = stateTable;
   this->symbolTable = symbolTable;
   this->ruleTable   = ruleTable;
   this->startState  = startState;
 }


 /*!
 Initializes the parser.
 Call this function before calling buildParseTree()

 /sa buildParseTree();

 */
 void LALR::init (const vector <Token*> &tokens) {

   // Copy the tokens vector (maybe just having a reference to it is enough...)
   this->tokens = tokens;

   Symbol *startReduction = new Symbol ();
   startReduction->state = startState;
   symbolStack.push (startReduction);

   currentState = startState;
   tokenIndex = 0;
   ErrTable.Clear();
   trim = false;
 }


 /*!
  Parse the tokens until it reduces a rule.

  /param trimReduction especifies trimming enable or disable. Trimming will
  simplify rules of the form: NonTerminal1 := Nonterminal2

  /return next reduction or NULL if error or test accepted

  /sa getResult (), buildParseTree ()
 */

 Symbol *LALR::nextReduction (bool trimReductions, bool reportOnlyOneError) 
 {
   Action *actObj;
   Token  *tok;
   m_trimReductions = trimReductions;

   for (;tokenIndex < tokens.size();tokenIndex++) 
   {
     // Save this tokens information for the error system.
     tok = tokens[tokenIndex];
     lastTerminal.symbol = tok->symbol;
     lastTerminal.image = tok->image;
     lastTerminal.line =  tok->line;
     lastTerminal.col = tok->col;
   
     // Get next action
     actObj = getNextAction (tokens[tokenIndex]->symbolIndex, currentState);
   
     // Test for errors
     if (actObj == NULL) 
	 {
       // Generate ERROR & recover pushing expected symbol in the stack
       // RECOVERING IS IN THE TODO LIST!
       // FOR THAT WE NEED A MECHANISM TO "ESTIMATE" THE NEXT TOKEN
       // Or use Burke-Fisher recovering algorithm
       
       // Create a symbol traceback vector.
       vector <Symbol*> traceback;
       vector <Symbol*> tmptokvector = symbolStack.get_vector();
       for (short k = tmptokvector.size()-1; k >= 0; k--) 
	   {
            traceback.push_back (tmptokvector[k]);
       }
       
       vector <wstring> expectedTokens = getPossibleTokens (currentState);
       // Add the error to the Error class.
       ErrTable.addError (ERROR_PARSE, UNEXPECTED_TOKEN, prevReduction, &lastTerminal,
                          expectedTokens, traceback, 
                          tokens[tokenIndex]->line,
                          tokens[tokenIndex]->col);

       if (reportOnlyOneError) 
	   {
          reductionResult = REDUCTION_ERROR;
          return NULL;
       }
     }
     else 
	 {
       Symbol *rdc = parseToken (actObj, symbolStack, tokenIndex, currentState);
       if (rdc != NULL) 
		   return rdc;
	   
	   if (reductionResult != REDUCTION_TOKEN_SHIFT) 
          return NULL;
     }
   }
   reductionResult = REDUCTION_ERROR;
   return NULL;
 }


 /*!
    Computes an Action object from the input parameters

    /param symbolIndex  the index in the symbol table that we want to match
    /param index the current state in the LALR state machine
    /return Action NULL if no action found for this symbol Index.
 */
 Action *LALR::getNextAction (UBYTE2 symbolIndex, UBYTE2 index) 
 {
   for (UBYTE2 i=0; i < stateTable->states[index].actions.size(); i++) 
   {
     if (stateTable->states[index].actions[i].symbolIndex == symbolIndex) 
       return &stateTable->states[index].actions[i];
   }
   return NULL;
 }

 vector<wstring> LALR::getPossibleTokens (UBYTE2 index) 
 {
   vector<wstring> tokenVector;
   for (UBYTE2 i=0; i < stateTable->states[index].actions.size(); i++) 
   {
     UBYTE2 j = stateTable->states[index].actions[i].symbolIndex;
     if (symbolTable->symbols[j].kind == TERMINAL) 
	 {
        wstring tokenName = symbolTable->symbols[j].name;
        tokenVector.push_back (tokenName);
     }
   }
   return tokenVector;
 }

 /*!
   Builds a parse tree with reductions as nodes.
   Sets the Error object with the possible compiling errors.

  /sa getError(), getNextReduction()
 */
 Symbol *LALR::parse (const vector <Token*> &tokens, 
                      bool trimReductions, 
                      bool reportOnlyOneError) 
 {
   init (tokens);

   Symbol *reduction;
   prevReduction = NULL;
   while (true) 
   {
     reduction = nextReduction(trimReductions, reportOnlyOneError);
     if ((reduction == NULL) && ((getResult() == REDUCTION_ERROR) ||
                                 (getResult() == REDUCTION_TEXT_ACCEPTED))) 
	 {
         break;
     } 
	 else if (reduction) 
	 {
         prevReduction = reduction;
     }
   }

   if (getResult() == REDUCTION_TEXT_ACCEPTED) 
   {
      return prevReduction;
   } 
   else 
   {
      return NULL;
   }
 }

 int LALR::getResult () 
 {
     return reductionResult;
 }


 ErrorTable *LALR::getErrors() 
 {
     return &ErrTable;
 }

 const SymbolTable *LALR::getSymbols() 
 {
     return symbolTable;
 }


 Symbol *LALR::parseToken (Action *actObj, SymbolStack &theStack, int tokenIndex, UBYTE2 &currentState) 
 {
     NonTerminal  *newNonTerminal;
     Terminal     *newTerminal;
     
     UBYTE2 index, i;
     
     int action = actObj->action;
     int target = actObj->target;
     
     switch (action) 
	 {
     // Pushes current token on the stack
     case ACTION_SHIFT:
         // Push current token on the stack
         currentState = target;
         
         currentLine = tokens[tokenIndex]->line;
         currentCol = tokens[tokenIndex]->col;
         
         tokens[tokenIndex]->state = target;
         
         // Create a terminal symbol and push it onto the stack
         newTerminal = createTerminal (tokens[tokenIndex]);
         
         theStack.push (newTerminal);
         reductionResult = REDUCTION_TOKEN_SHIFT;
         return NULL;
         
         /*
         Creates a new reduction. Pops all the terminals and non terminals
         for this rule and pushes the most left non terminal.
         */
     case ACTION_REDUCE:
		// Create a new Non Terminal (to represent this reduction)
		index = ruleTable->rules[target].symbolIndex;

		newNonTerminal = createNonTerminal (index, target);

		// If the rule has only a nonterminal then we dont create a reduction
		// node for this rule in the tree since its not usefull.
		// User can decide to simplify this by enabling the trimming
		if ((ruleTable->rules[target].symbols.size() == 1) &&
		    (symbolTable->symbols[ruleTable->rules[target].symbols[0]].kind == NON_TERMINAL) && 
			m_trimReductions) 
		{
			trim = true; 
			newNonTerminal->trimmed = true;
		} 
		else 
		{
			newNonTerminal->trimmed = false;
			trim = false;
		}
         
		// pop from the stack the tokens for the reduced rule
		// and store them in the NonTerminal as its children.
		for (i=0; i < ruleTable->rules[target].symbols.size(); i++) 
		{
			Symbol *Symb = theStack.top();

			// Added by Elm�: See comment in Factory.h
			if (Symb->mb_IsList && Symb->symbolIndex == newNonTerminal->symbolIndex)
			{
				Symb->trimmed = true;
			}

			// Trim the NonTerminal -> copy the children of NT, then delete NT
			if (Symb->type == NON_TERMINAL && Symb->trimmed && Symb->mb_AllowTrimming)
			{
				NonTerminal* NT = (NonTerminal*)Symb;

				while (NT->children.size())
				{
					newNonTerminal->children.push_front(NT->children.back());

					// The children must be removed otherwise they get deleted in the destructor of NT
					NT->children.pop_back();
				}
				delete NT; // otherwise memory leak!!!
			} 
			else 
			{
				newNonTerminal->children.push_front(Symb);
			}

			theStack.pop();
		}
         
		// Perform GOTO
		actObj = getNextAction (newNonTerminal->symbolIndex, theStack.top()->state);

		if ((actObj != NULL) && (actObj->action == ACTION_GOTO)) 
		{
			currentState = actObj->target;
			newNonTerminal->state = currentState;

			// Push the reduced nonterminal in the stack
			theStack.push (newNonTerminal);
		} 
		else 
		{
			reductionResult = REDUCTION_ERROR;
			return NULL;
		}

		reductionResult = REDUCTION_COMPLETED;
		return newNonTerminal;
         
         // This Action should never happen...
     case ACTION_GOTO:
         currentState = target;
         return NULL;
         
     case ACTION_ACCEPT:
         reductionResult = REDUCTION_TEXT_ACCEPTED;
         return NULL;
     }
	 return NULL;
 }
 
 Terminal *LALR::createTerminal (Token *tok) 
 {
     Terminal *newTerminal;
     newTerminal = new Terminal();
     newTerminal->symbol = tok->symbol;
     newTerminal->image = tok->image;
     newTerminal->symbolIndex = tok->symbolIndex;
     newTerminal->state = tok->state;
     newTerminal->line = tok->line;
     newTerminal->col = tok->col;
     return newTerminal;
 }
 
 NonTerminal *LALR::createNonTerminal (int index, int target) 
 {
     NonTerminal *newNonTerminal = Factory::CreateNewClass(index);
     
     newNonTerminal->symbolIndex = index;
     newNonTerminal->symbol = symbolTable->symbols [index].name;
     
     newNonTerminal->ruleIndex = ruleTable->rules[target].ruleIndex;
     newNonTerminal->line = currentLine;
     newNonTerminal->col = currentCol;
     return newNonTerminal;
 }
 

