/*---------------------------------*- C++ -*---------------------------------*\
    Compiler Generator Coco/R,
    Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
    extended by M. Loeberbauer & A. Woess, Univ. of Linz
    ported to C++ by Csaba Balazs, University of Szeged
    with improvements
        - Pat Terry, Rhodes University
        - 2010 Mark Olesen
-------------------------------------------------------------------------------
License
    This file is part of Compiler Generator Coco/R

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2, or (at your option) any
    later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    As an exception, it is allowed to write an extension of Coco/R that is
    used as a plugin in non-free software.

    If not otherwise stated, any source code generated by Coco/R (other than
    Coco/R itself) does not fall under the GNU General Public License.
\*---------------------------------------------------------------------------*/

#include "Utils.h"

namespace Coco
{

// * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * * //

std::wstring coco_stdWString(const std::string& str)
{
    std::wstring dst;
    dst.reserve(str.size());

    for
    (
        std::string::const_iterator iter = str.begin();
        iter != str.end();
        ++iter
    )
    {
        dst += *iter;
    }

    return dst;
}


bool coco_string_equal(const char* strn, const wchar_t* strw)
{
    const int strnLen = strn ? strlen(strn) : 0;
    const int strwLen = strw ? wcslen(strw) : 0;

    if (strnLen != strwLen)
    {
        return false;
    }

    for (int i = 0; i < strnLen; ++i)
    {
        if (wchar_t(strn[i]) != strw[i])
        {
            return false;
        }
    }

    return true;
}


bool getLine(FILE* is, std::string& line)
{
    line.clear();
    line.reserve(256);  // enough to avoid resizing in most cases

    if (is == NULL || feof(is))
    {
        return false;
    }

    int ch;
    while ((ch = fgetc(is)) != EOF && ch != '\n')
    {
        line += char(ch);
    }

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


} // End namespace Coco

// ************************************************************************* //
