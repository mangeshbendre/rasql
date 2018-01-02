/***************************************************************************
                          CGTFile.h  -
                          Encapsulates the Compiled Grammar Table File
                          read operations and structures.
                             -------------------
    begin                : Sat Jun 1 2002
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
 
 #ifndef _GP_CGTFILE_H
 #define _GP_CGTFILE_H

 #include "Misc.h"
 #include "DFA.h"
 #include "LALR.h"
 #include "ErrorReporter.h"
 #include "SymbolBase.h"
#include <QDataStream>
 
namespace Astudillo
{
	 class CGTFile 
	 {
	 public:
	
	   CGTFile ();
	   ~CGTFile ();
	
	    /*
	   Loads a CGT File.
	   Returns true if succesfull, false if error.
	   */
	   bool load (char *filename);
       bool load (QDataStream *myStream);
	
	   GrammarInfo *getInfo ();	
	
	   DFA*  getScanner ();
	   LALR* getParser ();
	   char* getError ();
	
	private:
	
		bool readEntry (EntryStruct *entry);
	    wstring        readUnicodeString ();
	
	   // Info
	   GrammarInfo *gInfo;
	   bool caseSensitive;
	   UBYTE2 startSymbol;
	
	   char* errorString;
	
       QDataStream *theStream;
	
	   // Character Table
	   UBYTE2 nbrCharacterSets;
	   CharacterSetTable *characterSetTable;
	
	   // Symbol Table
	   UBYTE2 nbrSymbolTable;
	   SymbolTable *symbolTable;
	
	   // DFA Table
	   UBYTE2 nbrDFATable;
	   DFAStateTable *stateTable;
	
	   // Rules Table
	   UBYTE2 nbrRuleTable;
	   RuleTable *ruleTable;
	
	   // LALR Table
	   UBYTE2 nbrLALRTable;
	   LALRStateTable *LALRTable;
	
	   // Init States
	   UBYTE2 DFAInit;
	   UBYTE2 LALRInit;
	
	   DFA *theDFA;
	   LALR *theLALR;
	
	 };

}; // namespace

 #endif

