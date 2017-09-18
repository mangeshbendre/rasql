/***************************************************************************
                          Misc.h  -  description
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

#ifndef MISC_H
#define MISC_H

#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information

#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <assert.h>

using namespace std;

typedef unsigned short UBYTE2;

namespace Astudillo
{

	#if defined (_BIGENDIAN)
	    inline UBYTE2 EndianConversion (UBYTE2 input)
	    {
		    UBYTE2 result = 
			      ((input & 0x00FF) << 8)
		        | ((input & 0xFF00) >> 8);
		    return result;
	    }
	#else
	    inline UBYTE2 EndianConversion (UBYTE2 input)
		{
			return input;
		}
	#endif 
	
	
	class GrammarInfo 
	{
	public:
		wstring    name;
	    wstring    version;
	    wstring    author;
	    wstring    about;
	};
	
	
	enum SymbolType {TERMINAL = 1, NON_TERMINAL = 0};
	
	class Symbol 
	{
	public:
		Symbol ()
		{
			mb_AllowTrimming = true;
			mb_IsList = false;

			trimmed = false;
			static int DbgIndex = 0;
			debugID = DbgIndex++;
			// printf("Create Symbol ID=%d\n", debugID);
		}
		virtual ~Symbol()
		{
			// ----------------------------------------------------------
			// DO NOT REMOVE THIS EMPTY DESTRUCTOR !!!!
			// OTHERWISE THE DESTRUCTOR OF NonTerminal WILL NOT BE CALLED
			// AND YOU WILL HAVE A HUGE MEMORY LEAK !!!!
			// ----------------------------------------------------------

			// printf("Delete Symbol ID=%d\n", debugID);
		}

		int        debugID;
		wstring    symbol;
	    SymbolType type;
	    UBYTE2    symbolIndex;
	    UBYTE2    state;
	    UBYTE2    line, col;
		bool       trimmed;
		// Added by Elmü: See comment in Factory.h
		bool       mb_AllowTrimming;
		bool       mb_IsList;
	};
	
	
	
	class Terminal : public Symbol 
	{
	public:
	    Terminal () 
	    { 
	    	type = TERMINAL; 
	    }
		wstring image;
	};
	


	class NonTerminal : public Symbol 
	{
	public:
	    NonTerminal()
	    {
	    	type = NON_TERMINAL;
	    }
	    
	    ~NonTerminal()
	    {
		    for (unsigned int i=0; i < children.size(); i++) 
		    {
	        	delete children[i];
	    	}
	    }

	    UBYTE2 ruleIndex;
	    deque <Symbol*> children;
	};
	
	
	
	struct RuleStruct 
	{
	   UBYTE2         ruleIndex;
	   UBYTE2         symbolIndex;
	   vector<UBYTE2> symbols;
	};
	
	
	
	class RuleTable 
	{
	public:
	   RuleTable (UBYTE2 nbrEntries)
	   {
	      this->nbrEntries = nbrEntries;
	      rules = new RuleStruct [nbrEntries];
	   }
	   ~RuleTable()
	   {
	      delete [] rules;
	   }
	   UBYTE2 nbrEntries;
	   RuleStruct *rules;
	};
	
	
	
	
	struct Edge 
	{
	   UBYTE2 characterSetIndex;
	   UBYTE2 targetIndex;
	};
	
	
	
	struct DFAState 
	{
	   bool accept;
	   UBYTE2 acceptIndex;
	   vector<Edge> edges;
	};
	
	
	
	class DFAStateTable 
	{
	public:
	   DFAStateTable (UBYTE2 nbrEntries)
	   {
	      this->nbrEntries = nbrEntries;
	      states = new DFAState [nbrEntries];
	   }
	   ~DFAStateTable()
	   {
	      delete [] states;
	   }
	   UBYTE2 nbrEntries;
	   DFAState *states;
	};
	
	
	
	
	struct SymbolStruct 
	{
	   wstring     name;
	   SymbolType  kind;
	};
	
	
	
	
	class SymbolTable 
	{
	public:
	   SymbolTable(UBYTE2 nbrEntries)
	   {
	      m_nbrEntries = nbrEntries;
	      symbols = new SymbolStruct[nbrEntries];
	   }
	   ~SymbolTable()
	   {
	      delete[] symbols;
	   }
	
	   UBYTE2 m_nbrEntries;
	   SymbolStruct *symbols;
	};
	
	
	
	
	class CharacterSetTable 
	{
	public:
	   CharacterSetTable(UBYTE2 nbrEntries)
	   {
	      m_nbrEntries = nbrEntries;
	      characters.resize (nbrEntries);
	   }
	   UBYTE2 m_nbrEntries;
	   vector <wstring> characters;
	};
	
	
	
	struct Action 
	{
	   UBYTE2 symbolIndex;
	   UBYTE2 action;
	   UBYTE2 target;
	};
	
	
	
	struct LALRState 
	{
	   vector<Action> actions;
	};
	
	
	class LALRStateTable 
	{
	public:
	   LALRStateTable (UBYTE2 nbrEntries)
	   {
	     this->nbrEntries = nbrEntries;
	     states = new LALRState [nbrEntries];
	   }
	   ~LALRStateTable()
	   {
	     delete [] states;
	   }
	
	   UBYTE2 nbrEntries;
	   LALRState *states;
	};
	
	
	
	class Token 
	{
	public:
	   Token ()
	   {
	      symbolIndex = 0;
	      state = 0;
	      kind = 0;
	   }
	
	   wstring symbol;
	   wstring image;
	   UBYTE2 symbolIndex;
	   UBYTE2 line, col;
	   UBYTE2 state;
	   UBYTE2 kind;
	};
	
	
	
	class SymbolStack 
	{
	public:
		~SymbolStack()
		{ 
			while (size()) 
			{
				delete top(); pop();
			} 
		}
		bool empty() const
			{return (c.empty()); }
		int size() const
			{return (c.size()); }
		Symbol* top()
			{return (c.back()); }
		const Symbol* top() const
			{return (c.back()); }
		void push(Symbol* x)
			{c.push_back(x); }
		void pop()
			{c.pop_back(); }
		bool operator==(const SymbolStack& x) const
			{return (c == x.c); }
		bool operator!=(const SymbolStack& x) const
			{return (!(*this == x)); }
		bool operator<(const SymbolStack& x) const
			{return (c < x.c); }
		bool operator>(const SymbolStack& x) const
			{return (x < *this); }
		bool operator<=(const SymbolStack& x) const
			{return (!(x < *this)); }
		bool operator>=(const SymbolStack& x) const
			{return (!(*this < x)); }
	
	    vector <Symbol*> get_vector () 
	        {return c;}
	protected:
	    vector <Symbol*> c;
	};
	
	
	
    struct EntryStruct 
    {
       bool          vBool;
       UBYTE2       vInteger;
       unsigned char vByte;
       wstring       vString;
    };
	
	
	
	enum error_type 
	{
	    ERROR_PARSE, 
	    ERROR_SCAN
	};
	
	enum error_value 
	{
	    END_COMMENT_NOT_FOUND, // end of comment not found
	    UNKNOWN_TOKEN,
	    UNEXPECTED_TOKEN
	};
	
	
	class GPError  
	{
	public:
		GPError()
		{
		    type  = (error_type) -1;
		    value = (error_value)-1;
		    line  = -1;
		    col   = -1;
		    reduction    = NULL;
		    lastTerminal = NULL;
		}
	    error_type  type;
	    error_value value;
	    UBYTE2 line, col;
		
	    vector<wstring> expected; // deleted in the destructor

	    // ATTENTION: The following symbols will NOT be deleted in the destructor!
	    // They are already stored on the SymbolStack and will be deleted there
	    Symbol*   reduction;
	    Terminal* lastTerminal;
	    vector<Symbol*> traceback;
	};
	
	
	
	class ErrorTable 
	{
	public:
	   ~ErrorTable()
	   {
		   Clear();
	   }

	   void Clear()
	   {
	      for (unsigned short i = 0; i < errors.size(); i++) 
	      {
		     delete errors[i];
	      }
	      errors.clear();
	   }
	   
	   void addError (error_type type, error_value value , Symbol *rdc, 
	                  Terminal *lastTerminal, vector <wstring> expected, 
	                  vector <Symbol*> traceback, UBYTE2 line, UBYTE2 col)
	   {
	     GPError *err = new GPError ();
	     err->expected = expected;
	     err->traceback = traceback;
	     err->line = line;
	     err->col = col;
	     err->type = type;
	     err->value = value;
	     err->reduction = rdc;
	     err->lastTerminal = lastTerminal;
	     errors.push_back (err);
	   }
	
	   void addError (error_type type, error_value value, UBYTE2 line, UBYTE2 col)
	   {
	     GPError *err = new GPError ();
	     err->line = line;
	     err->col = col;
	     err->type = type;
	     err->value = value;
	     errors.push_back (err);
	   }
	
	   vector <GPError*> errors;
	};
	
}; // namespace

#endif

