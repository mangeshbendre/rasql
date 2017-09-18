/*
 * UTFConverter.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: mangesh
 */

#include "UTFConverter.h"
#include <stdio.h>
#include <stdlib.h>

string wstrtostr(const wstring wstr)
{
    char *buf;
    string resultstring;
    size_t wsize=wstr.length();
    resultstring.resize(wsize, '\0');

    buf = (char *) resultstring.data();
    int ret=wcstombs(buf, wstr.c_str(), wsize);
    buf[ret]='\0';
    return resultstring;
}
