/***************************************************************************
                          SymbolBase.h  -  description
                             -------------------
    begin                : 15.3.2010
    copyright            : (C) 2010 Elm�
    email                : elmue@gmx.de
 ***************************************************************************/

  /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _SYMB_BASE_H
#define _SYMB_BASE_H

#include "Misc.h"

namespace Astudillo
{
	enum eVarType
	{
		VarInvalid,
		VarNull,
		VarBool,
		VarInteger,
		VarString,
		VarDouble,
		VarObject,
	};

	// Extend this struct to your needs!!
	// Add all the value types here that your script language allows
	struct ExecuteValue
	{
		int       Int;    // Calculator Demo only supports integer
		bool      Bool;   // not used in Calculator example
		double    Double; // not used in Calculator example
		wstring   String; // not used in Calculator example
		eVarType  Type;   // not used in Calculator example

		ExecuteValue()
		{
			Int    = 0;
			Bool   = false;
			Double = 0.0;
			Type   = VarInvalid;
		}
	};

	// -------------------------------------------

	// The Execute() function in any of the derived classes may set one of these flags and return false.
	// The "event" will bubble up until a class checks why a "false" has been returned 
	// and takes the corresponding action (e.g. a while statement aborts the loop and resets the event to None)
	enum eEvent
	{
		EvtNone,
		EvtError,     // set when an error has occurred 
		EvtException, // set in a throw statement,   recognized in a try statement
		EvtBreak,     // set in a break statement,   recognized in switch, for, foreach, while statements
		EvtContinue,  // set in a continue statment, recognized in for, foreach, while... statements
		EvtReturn,    // set in a return statement,  recognized in the function decalaration statement
		EvtUserAbort, // may be set from outside in another thread to abort the script
	};

	// Extend this struct to your needs!!
	// It passes ALL required data to all the SymbolBase derived classes
	// and retruns their state flags after calling Execute()
	struct ExecuteData
	{
		ExecuteData()
		{
			Event = EvtNone; // not used in this demo
			Line  = -1;
			Col   = -1;
		}

		bool SetError(Symbol* pSymbol, string sMsg)
		{
			Event   = EvtError;
			Message = sMsg;
			Line    = pSymbol->line;
			Col     = pSymbol->col;
			return false;
		}
		bool ThrowException(Symbol* pSymbol, string sMsg)
		{
			Event   = EvtException;
			Message = sMsg;
			Line    = pSymbol->line;
			Col     = pSymbol->col;
			return false;
		}
		// if anything extraordinary has happened -> return false
		bool OK()
		{
			return Event == EvtNone;
		}

		eEvent Event;
		string Message;
		int    Line;
		int    Col;

		// map<string, ExecuteValue> GlobalVariables;
		// vector<SymbolFunction*>   CallStack;
	};

	// -------------------------------------------

	class SymbolBase : public NonTerminal
	{
	public:
		// This function must be overridden in derived classes
		virtual bool Execute(ExecuteValue* pResult, ExecuteData* pData)
		{
			assert(0); // This must never happen
			return false;
		}

		// returns the child at childIndex that must be a Terminal
		bool GetChildTerminal(int childIndex, Terminal** ppTerminal, ExecuteData* pData)
		{
			return GetChildSymbol(childIndex, TERMINAL, (Symbol**)ppTerminal, pData);
		}

		// returns the child at childIndex that must be a NonTerminal
		bool GetChildNonTerminal(int childIndex, SymbolBase** ppNonTerminal, ExecuteData* pData)
		{
			return GetChildSymbol(childIndex, NON_TERMINAL, (Symbol**)ppNonTerminal, pData);
		}

		// executes the child at childIndex that must be a NonTerminal
		bool ExecuteChild(int childIndex, ExecuteValue* pResult, ExecuteData* pData)
		{
			SymbolBase* pChild;
			if (!GetChildNonTerminal(childIndex, &pChild, pData))
				return false;

			if (!pChild->Execute(pResult, pData))
				return false;

			return pData->OK();
		}

		// executes all children that are NonTerminal's in the order in the deque children
		// You can use this for example for Statement Lists where all statements must be executed.
		bool ExecuteAllChildren(ExecuteValue* pResult, ExecuteData* pData)
		{
			int s32_Children = children.size();
			for (int i=0; i<s32_Children; i++) 
			{
				if (children[i]->type != NON_TERMINAL)
					continue;

				if (!((SymbolBase*)children[i])->Execute(pResult, pData))
					return false;
			}
			return pData->OK();
		}

	private:			
		// returns the child at childIndex that may be a Terminal or a NonTerminal
		// ATTENTION:
		// ----------------------------------------------------------------------------------------------------
		// With intention this funcion is private.
		// If you don't know if a child is a Terminal or a NonTerminal, there is something wrong in your Grammar!
		// Normally it should NOT be necessary to use this function to determine the type of a child!!
		// ----------------------------------------------------------------------------------------------------
		bool GetChildSymbol(int childIndex, SymbolType Type, Symbol** ppSymbol, ExecuteData* pData)
		{
			if (childIndex >= (int)children.size())
			{
				assert(0);
				return pData->SetError(this, "Internal Error: Invalid Index");
			}
			if (children[childIndex]->type != Type)
			{
				assert(0);
				return pData->SetError(children[childIndex], "Internal Error: Unexpected Child");
			}
			*ppSymbol = children[childIndex];
			return pData->OK();
		}
	}; // class
}; // namespace

#endif

