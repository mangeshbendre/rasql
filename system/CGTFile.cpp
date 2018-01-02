/***************************************************************************
                          CGTFile.cpp  -
                          Encapsulates the Compiled Grammar Table File
                          read operations and structures.
                             -------------------
    begin                : Fri May 31 00:53:11 CEST 2002
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

//inline void initMyResource() { Q_INIT_RESOURCE(ra); }

#include "CGTFile.h"
#include <QDir>
#include <QFile>
#include <QDataStream>



 using namespace Astudillo;

 CGTFile::CGTFile () {

   gInfo = NULL;

   errorString = "";

   ruleTable = NULL;
   symbolTable = NULL;
   stateTable = NULL;
   LALRTable = NULL;
   characterSetTable = NULL;

   theDFA = NULL;
   theLALR = NULL;
 }

 CGTFile::~CGTFile () {

	 delete gInfo;
     delete symbolTable;
     delete stateTable;
     delete LALRTable;
     delete ruleTable;
     delete characterSetTable;
	 delete theDFA;
	 delete theLALR;
 }

 bool CGTFile:: load (char *filename) {
    ifstream cgtStream;

    //initMyResource();

    QFile file(":/ra.cgt");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    //cgtStream.open (filename, ifstream::in | ifstream::binary);

    bool result = load (&in);
    cgtStream.close();
    return result;
 }

 bool CGTFile::load (QDataStream *myStream)
 {
   int i;	

   EntryStruct entry;

   theStream = myStream;

   // Read Header
   wstring header = readUnicodeString();

   // Read Records
   unsigned char recordType;
   UBYTE2 nbrEntries;
   UBYTE2 index;

   while (!theStream->atEnd()) {
     // Read record type & number of entries
     theStream->readRawData ((char*)&recordType, 1);
     if (theStream->status()!=QDataStream::Ok) {
        if (theStream->atEnd()) {
			break;
        } else {
			return false;
        }
     }

    theStream->readRawData ((char*) &nbrEntries, 2);
    //Convert to little endian if needed.
    nbrEntries = EndianConversion(nbrEntries);

     if (recordType != 77) {
       errorString = "Record type is not supported\n";
       return false;
     }

     // Read the type of content
     unsigned char contentType;
     if (!readEntry(&entry)) return false;
     contentType = entry.vByte;

     switch (contentType) {

       // Parameters Record	
       case 'P':
	   delete gInfo;
	   gInfo = new GrammarInfo ();

       // read name
       if (!readEntry(&entry)) return false;
        
       gInfo->name = entry.vString;

       // read version
       if (!readEntry(&entry)) return false;
       gInfo->version = entry.vString;

       // read Author
       if (!readEntry(&entry)) return false;
       gInfo->author = entry.vString;

       // read About
       if (!readEntry(&entry)) return false;
       gInfo->about = entry.vString;

       // Case
       if (!readEntry(&entry)) return false;
       caseSensitive = entry.vBool;

       // start symbol
       if (!readEntry (&entry)) return false;
       startSymbol = entry.vInteger;

       break;

       // Table Counts
       case 'T' :
       if (!readEntry(&entry)) return false;
       nbrSymbolTable = entry.vInteger;

       // Delete & Create a Symbol Table
       delete symbolTable;
       symbolTable = new SymbolTable (nbrSymbolTable);

       if (!readEntry(&entry)) return false;
       nbrCharacterSets = entry.vInteger;

       // Delete & Create a Character Sets Table
       delete characterSetTable;
       characterSetTable = new CharacterSetTable (nbrCharacterSets);

       // Delete & Create a Rule Table
       if (!readEntry(&entry)) return false;
       nbrRuleTable = entry.vInteger;
       delete ruleTable;
       ruleTable = new RuleTable (nbrRuleTable);

       // Delete & Create a DFAStateTable
       if (!readEntry(&entry)) return false;
       nbrDFATable = entry.vInteger;
       delete stateTable;
       stateTable = new DFAStateTable (nbrDFATable);

        // Delete & Create a LALR Table
       if (!readEntry(&entry)) return false;
       nbrLALRTable = entry.vInteger;
       delete LALRTable;
       LALRTable = new LALRStateTable (nbrLALRTable);

       break;

       // Character Set Table Entry
       case 'C' :
       if (!readEntry(&entry)) return false;
       index = entry.vInteger;

       if (!readEntry(&entry)) return false;
       characterSetTable->characters[index] =  entry.vString;
       break;

       // Symbol Table Entry
       case 'S' :
       if (!readEntry(&entry)) return false;
       index = entry.vInteger;

       if (!readEntry(&entry)) return false;
       symbolTable->symbols[index].name = entry.vString;

       if (!readEntry(&entry)) return false;
       symbolTable->symbols[index].kind = (SymbolType) entry.vInteger;
       break;

       // Rule
       case 'R' :
       readEntry (&entry);
       index = entry.vInteger;
       ruleTable->rules[index].ruleIndex = index;
      
       readEntry (&entry);
       ruleTable->rules[index].symbolIndex = entry.vInteger;
      
       // Read empty field
       readEntry (&entry);

       // Read symbols for this rule (nonTerminal -> symbol0 symbol1 ... symboln)
       for (i=0; i < nbrEntries-4; i++) {
         if (!readEntry(&entry)) return false;
        // ruleTable->rules[index].symbols.push_back (entry.vInteger);
		 RuleStruct *rst = &ruleTable->rules[index];
		 
		 vector <UBYTE2> *s = &rst->symbols;
		 s->push_back (entry.vInteger);
       }
       break;

       // Initial States
       case 'I' :
       if (!readEntry (&entry)) return false;
       DFAInit = entry.vInteger;

       if (!readEntry(&entry)) return false;
       LALRInit = entry.vInteger;
       break;


       // DFA State Entry
       case 'D':
       if (!readEntry(&entry)) return false;
       index = entry.vInteger;

       // create a new State an insert it in the table
       if (!readEntry(&entry)) return false;
       stateTable->states[index].accept = entry.vBool;

       if (!readEntry (&entry)) return false;
       stateTable->states[index].acceptIndex = entry.vInteger;

       if (!readEntry (&entry)) return false;

       Edge edge;
       for (i=0; i < nbrEntries-5; i+=3) {

         if (!readEntry (&entry)) return false;
         edge.characterSetIndex = entry.vInteger;

         if (!readEntry (&entry)) return false;
         edge.targetIndex = entry.vInteger;

         if (!readEntry (&entry)) return false;

         stateTable->states[index].edges.push_back(edge);
       }
       break;

       // LALR State entry
       case 'L':
       if (!readEntry (&entry)) return false;
       index = entry.vInteger;

       if (!readEntry (&entry)) return false;

       Action action;
       for (i=0; i < nbrEntries-3; i+=4) {
         if (!readEntry (&entry)) return false;
         action.symbolIndex = entry.vInteger;

         if (!readEntry (&entry)) return false;
         action.action = entry.vInteger;

         if (!readEntry (&entry)) return false;
         action.target = entry.vInteger;

         if (!readEntry (&entry)) return false;

         LALRTable->states[index].actions.push_back(action);
       }

      break;

     }

   }
   return true;
 }

/*
Reads an entry in a record
*/
bool CGTFile::readEntry (EntryStruct *entry) 
{
	char tmpChar;
	char dataType;

    theStream->readRawData(&dataType,1);
    if (theStream->status()!=QDataStream::Ok)
	{
		errorString = "Error reading entry\n";
		return false;
	} 
	else 
	{
		switch (dataType) 
		{
		case 'E': break;
		case 'B':
            theStream->readRawData(&tmpChar,1);
			if (tmpChar)
				entry->vBool = true;
			else
				entry->vBool = false;
			break;
		case 'b':
            theStream->readRawData ((char*) &entry->vByte, 1);
			break;
		case 'I':
            theStream->readRawData ((char*) &entry->vInteger, 2);
			entry->vInteger = EndianConversion(entry->vInteger); 
			break;
		case 'S':
			entry->vString = readUnicodeString();
			break;
		}
	}
	return true;
}

 wstring CGTFile::readUnicodeString() 
 {
   wchar_t readChar = 0;
   wstring sTemp;
   sTemp.reserve(65536); // all unicode characters must fit

   while (true) 
   {
       theStream->readRawData((char*)&readChar, 2);

       if (readChar == 0 || theStream->atEnd())
		   break;

	   sTemp.append(1, EndianConversion(readChar));
   }
   return sTemp;
 }


 GrammarInfo *CGTFile::getInfo () {
	return gInfo;
 }

 DFA *CGTFile::getScanner () {
     delete theDFA;
	 theDFA = new DFA (stateTable, symbolTable,
     characterSetTable, DFAInit, caseSensitive);
     return theDFA;
 }


 LALR *CGTFile::getParser () {
	 delete theLALR;
     theLALR = new LALR (LALRTable, symbolTable, ruleTable, LALRInit);
     return theLALR;
 }

char* CGTFile::getError ()
{
	return errorString;
}
