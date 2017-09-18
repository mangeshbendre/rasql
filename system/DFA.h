/***************************************************************************
                          DFA.h  -  description
                             -------------------
    begin                : Sun Jun 2 2002
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

 #ifndef _GP_DFA_H
 #define _GP_DFA_H

 #include "Misc.h"

namespace Astudillo
{
 
	 class DFA 
	 {
	 public:
	   DFA (const DFAStateTable *stateTable, const SymbolTable *symbolTable,
	        const CharacterSetTable *characterSetTable, 
	        UBYTE2 startState, bool caseSensitive);
	   
	   ~DFA ();
	
	   bool scan (const wchar_t *text);
	   bool scan (const char *text);
	
	   vector <Token*> &getTokens ();
	
	   ErrorTable *getErrors();
	 
	 private:
	   const DFAStateTable *stateTable;
	   const SymbolTable *symbolTable;
	   const CharacterSetTable *characterSetTable;
	   UBYTE2 startState;
	   bool caseSensitive;
	
	   vector<Token*> tokens;
	
	   ErrorTable ErrTable;
	 };

 }; // namespace

 #endif

