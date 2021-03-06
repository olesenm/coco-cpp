/*---------------------------------------------------------------------------*\
    This file is part of the Compiler Generator Coco/R,

    Copyright (C) 2010 Mark Olesen.
-------------------------------------------------------------------------------
License
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

Description
    Need slightly special handling for Windows vs. other system.

    On Windows, using main(int argc, char* argv[]) will mangle any
    unicode filenames. The wmain(int argc, wchar_t* argv[]) is used
    exactly for this purpose. The argv values are then 16bit unicode,
    which can be saved nicely as std::wstring values.

    The only problem with all of this is that we must use _wfopen()
    instead of fopen() for having unicode filename support.

    To complicate things slightly more, MINGW doesn't have wmain() so we
    we need to make our own, which we'll call from main().

    For other systems (eg, Linux, Unix) it is common to use UTF8
    encoding, in which case we can simply use std::string for the
    storage and the normal fopen() etc. as usual.


    This file can only be included once per project, and only in
    particular places (immediately before a main body) - see Coco.cpp

\*---------------------------------------------------------------------------*/

// forward declaration
//! uniform main for 16-bit unicode command-line (windows)
int __cdecl CocoWin32Main(int argc, wchar_t *argv[]);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// this might apply to other compilers too:
#ifdef __MINGW32__
# include <stdio.h>
# include <windows.h>
# include <shellapi.h>

// MINGW doesn't have wmain(), we need to make our own
// this might even be okay for most WIN32 builds

int __cdecl main()
{
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (!argv)
    {
        wprintf(L"CommandLineToArgvW failed\n");
        return 1;
    }

    int exitCode = CocoWin32Main(argc, argv);
    LocalFree(argv);
    return exitCode;
}
# else    // MINGW
// assume that other windows compilers have wmain()
int wmain(int argc, wchar_t *argv[])
{
    return CocoWin32Main(argc, argv);
}
# endif   // MINGW


// This is our replacement for 'main()' and/or 'wmain()'
// The code body follows immediately after this include.
int __cdecl CocoWin32Main(int argc, wchar_t *argv[])

// ************************************************************************* //
