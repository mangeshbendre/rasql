/***************************************************************************
                          ErrorReporter.h  -  
                          Reports the errors in a human readable 
                          way.
                          --------------------------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2002-2003 Manuel Astudillo
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

#ifndef _GP_ERROR_REPORTER_H
#define _GP_ERROR_REPORTER_H

#include "Misc.h"

namespace Astudillo
{
	
	// Default Messages. Should be overrided by custom messages.
	
	class ErrorReporter {
	protected:
		string endComment;
	    string unknownToken; 
		string unexpectedToken;
	
	    // multibyte error assembling functions
	    virtual string composeScanErrorMsg  (const GPError &err);
	    virtual string composeParseErrorMsg (const GPError &err);
	
	    // unicode error assembling functions
	    virtual wstring composeScanErrorMsgU  (const GPError &err);
	    virtual wstring composeParseErrorMsgU (const GPError &err);
	
	    // Help functions  needed to compose error messages
	    bool  in (wstring a, vector <wstring> expected, int *pos);
	    Symbol *in (wstring b, vector <Symbol*> traceback, int *pos);
	    string atoi (int i);
	    string composeLineCol (const GPError &err);
	
	public:
	    ErrorReporter ();
	    virtual ~ErrorReporter ();
	
	    string  composeErrorMsg   (const GPError &err);
	    wstring composeErrorMsgU  (const GPError &err);
	};

}; // namespace

#endif

