/***************************************************************************
                          DFA.cpp  -  Deterministic Finite Automata
                          -------------------
    begin                : Sun Jun 2 2002
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


#include "DFA.h"
#include <string.h>
#include <stdlib.h>

 using namespace Astudillo;

DFA::DFA (const DFAStateTable *stateTable, 
          const SymbolTable *symbolTable,
          const CharacterSetTable *characterSetTable, 
          UBYTE2 startState, 
          bool caseSensitive) 
{
   this->startState = startState;
   this->stateTable = stateTable;
   this->symbolTable = symbolTable;
   this->characterSetTable = characterSetTable;
   this->caseSensitive = caseSensitive;
}

DFA::~DFA () 
{
	for (UBYTE2 j = 0; j < tokens.size(); j++) 
	{
		delete tokens[j];
	}
}


bool DFA::scan (const char *text) 
{
	wchar_t *wtext = new wchar_t [strlen(text)+1];
	mbstowcs (wtext, text, strlen(text)+1);

	bool res = scan (wtext);
	delete [] wtext;

	return res;
}


bool DFA::scan (const wchar_t *text) 
{
   // Skip Unicode file identifier
   if (text[0] == 0xFEFF) text++;

   bool commentBlock = false;
   bool commentLine = false;
   UBYTE2 currentLine = 1, currentCol = 1, tokenBeginCol = 1;

   Token *t = NULL;
   wstring tmpImage;
   tmpImage.reserve(256); // Tokens have normally less than 20 characters
   wstring tokenSymbol, tokenImage;
   wchar_t currentChar;
   UBYTE2 currentState = startState;
   int i=0;
   UBYTE2 j;

   // Free the memory for every token that might be stored in this vector
   for (j = 0; j < tokens.size(); j++) 
   {
		delete tokens[j];
   }	

   tokens.clear();

   ErrTable.Clear();

   bool run = true;

   while (run) {
	 if (text[i] == 0) {
         // Check if Comment Block not ended
         if (commentBlock) {
		    // Set error and return
            ErrTable.addError (ERROR_SCAN, END_COMMENT_NOT_FOUND, currentLine, tokenBeginCol);
            currentState = startState;
            tmpImage = L"";

            return false;
         } else {
            run = false;
         }
	 }
     if (text[i] == L'\n') {
       currentLine++;
       currentCol = 1;
       if (commentLine) {
         commentLine = false;
 
         if (text[i] == 0) {
           break;
         }
       }
     }

     if (!caseSensitive) {
       currentChar = (wchar_t) towlower (text[i]);
     } else {
       currentChar = (wchar_t) text[i];
     }

     if (!commentLine) {

       tmpImage.append (1,text[i]);

       // Check which is the next state
       UBYTE2 e;
       bool changedState = false;
       for (e = 0; e < stateTable->states[currentState].edges.size(); e++) {

         UBYTE2 index = stateTable->states[currentState].
         edges[e].characterSetIndex;
         // Check if the current character matchs one of the edge string
         wstring edgeString = characterSetTable->characters[index];
         UBYTE2 s = 0;
         while (edgeString[s] != 0) {
           if (currentChar == edgeString[s]) {
             currentState = stateTable->states[currentState].edges[e].targetIndex;
             changedState = true;
             break;
           }
           s++;
         }
         if (changedState) break;

       }

       if (!changedState) {
         
		 // Check if it is an Accept state
         if (stateTable->states[currentState].accept) {
           i--;
		   currentCol--;
           // If the current character was a \n we have to decrement the line counter
           if (text[i+1] == L'\n') {
             currentLine--;
           }

           // Check for type of symbol
           UBYTE2 index2 = stateTable->states[currentState].acceptIndex;
           switch (symbolTable->symbols[index2].kind) {
             // Non terminal symbol
             case 0:
             // Cannot be
             break;

             // Terminal symbol
             case 1 :
             if (!commentBlock) {
               tokenSymbol = symbolTable->symbols[index2].name;
               tokenImage = tmpImage.substr (0, tmpImage.length()-1);

               t = new Token();
               t->symbol = tokenSymbol;
               t->image = tokenImage;
               t->symbolIndex = index2;
               t->kind = TERMINAL;

               t->line = currentLine;
               t->col = tokenBeginCol;

               tokens.push_back(t);
             }
			   tokenBeginCol = currentCol;
             break;

             // Whitespaces
             case 2:
             // Just dont do anything or generate error for token not accepted
				tokenBeginCol = currentCol;
             break;

             // End symbol
             case 3:
             break;

             // Comment Start
             case 4:
             commentBlock = true;
             break;
             
             // Comment End
             case 5:
             commentBlock = false;
             break;

             case 6:
             commentLine = true;
             break;
           }
           currentState = startState;
           tmpImage = L"";
         } else {
           if (!commentBlock) {
             // Set error and return
             ErrTable.addError (ERROR_SCAN, UNKNOWN_TOKEN, currentLine, tokenBeginCol);
             currentState = startState;
             tmpImage = L"";
             return false;
           }
           tokenBeginCol = currentCol;
         }
       }

     }
     i++;
     currentCol++;
   }

   // Generate EOF token
   t = new Token ();
   t->line = currentLine;
   t->col = currentCol;

   t->symbol = L"EOF";
   t->image = L"EOF";
   tokens.push_back(t);

   return true;
 }

 vector <Token*> &DFA::getTokens () {
    return tokens;

 }

 ErrorTable *DFA::getErrors () {
   return &ErrTable;
 }


