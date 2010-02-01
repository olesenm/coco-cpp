/*---------------------------------*- C++ -*---------------------------------*\
    Compiler Generator Coco/R,
    Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
    extended by M. Loeberbauer & A. Woess, Univ. of Linz
    ported to C++ by Csaba Balazs, University of Szeged
    with improvements by Pat Terry, Rhodes University
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
/*!

@mainpage The Compiler Generator Coco/R, C++ version.

http://www.ssw.uni-linz.ac.at/coco/

@section usage Program Usage

@verbatim
Coco Grammar.atg {Option}
Options:
  -namespace <Name>      eg, My::Name::Space
  -prefix    <Name>      for unique Parser/Scanner file names
  -frames    <Dir>       for frames not in the source directory
  -trace     <String>    trace with output to trace.txt
  -trace2    <String>    trace with output on stderr
  -o         <Dir>       output directory
  -lines                 include #line pragmas in the generated code
  -single                include the Scanner code in the Parser file
  -bak                   save existing Parser/Scanner files as .bak
  -help                  print this usage
@endverbatim
The valid trace string values are listed below.

The Scanner.frame and Parser.frame must be located in one of these
directories:
-# In the specified -frames directory.
-# The current directory.
-# The same directory as the atg grammar.

Unless specified with the @em -o option, the generated scanner/parser
files are written in the same directory as the atg grammar!


@section trace Trace output options

 - 0 | A: prints the states of the scanner automaton
 - 1 | F: prints the First and Follow sets of all nonterminals
 - 2 | G: prints the syntax graph of the productions
 - 3 | I: traces the computation of the First sets
 - 4 | J: prints the sets associated with ANYs and synchronisation sets
 - 6 | S: prints the symbol table (terminals, nonterminals, pragmas)
 - 7 | X: prints a cross reference list of all syntax symbols
 - 8 | P: prints statistics about the Coco run
 - 9    : unused

The trace output can be switched on as a command-line option or by
the pragma:
@verbatim
    $ { digit | letter }
@endverbatim
in the attributed grammar.

The extended directive format may also be used in the attributed grammar:
@verbatim
    $trace=(digit | letter){ digit | letter }
@endverbatim

@section Compiler Directives (Extended Pragmas)

To improve the reliability of builds in complex environments, it is
possible to specify the desired namespace and/or file prefix as a
directive within the grammar. For example, when compiling the
Coco-cpp.atg itself, it can be compiled within the 'Coco' namespace
as specified on the command-line. For example,

@verbatim
    Coco -namespace Coco Coco-cpp.atg
@endverbatim

As an alternative, it can be specified within the Coco-cpp.atg file:
@verbatim
    COMPILER Coco
    $namespace=Coco
@endverbatim

The @em -prefix command-line option can be similarly specified. For
example,

@verbatim
    COMPILER GramA
    $namespace=MyProj:GramA
    $prefix=GramA

    COMPILER GramB
    $namespace=MyProj:GramB
    $prefix=GramB
@endverbatim

would generate the scanner/parser files in the respective namespaces,
but with these names:

@verbatim
    GramAParser.{h,cpp}
    GramAScanner.{h,cpp}

    GramBParser.{h,cpp}
    GramBScanner.{h,cpp}
@endverbatim

The resulting scanner/parsers are separated not only in the C++
namespace, but also on the filesystem. Even if the resulting files are
to be located in separate directories, using a prefix can be useful to
avoid confusion. For example, when the compiler -I includes both
directories, an include "Parser.h" will only be able to address one of
the parsers.

For completeness, it is also possible to add in trace string parameters
with the same syntax. For example,

@verbatim
    COMPILER GramC
    $trace=ags
@endverbatim

*/
/*-------------------------------------------------------------------------*/

#include <stdio.h>
#include "CocoInfo.h"
#include "Scanner.h"
#include "Parser.h"
#include "Utils.h"
#include "Tab.h"

//! @namespace Coco
//! @brief Compiler Generator Coco/R, C++ version
using namespace Coco;


void printUsage(const char* message = NULL)
{
	if (message)
	{
		wprintf(L"\nError: %s\n\n", message);
	}

	wprintf(L"Usage: Coco Grammar.atg {Option}\n");
	wprintf(L"Options:\n");
	wprintf(L"  -namespace <Name>      eg, My::Name::Space\n");
	wprintf(L"  -prefix    <Name>      for unique Parser/Scanner file names\n");
	wprintf(L"  -frames    <Dir>       for frames not in the source directory\n");
	wprintf(L"  -trace     <String>    trace with output to trace.txt\n");
	wprintf(L"  -trace2    <String>    trace with output on stderr\n");
	wprintf(L"  -o         <Dir>       output directory\n");
	wprintf(L"  -lines                 include #line pragmas in the generated code\n");
	wprintf(L"  -single                include the Scanner code in the Parser file\n");
	wprintf(L"  -bak                   save existing Parser/Scanner files as .bak\n");
	wprintf(L"  -help                  print this usage\n");
	wprintf(L"\nValid characters in the trace string:\n");
	wprintf(L"  A  trace automaton\n");
	wprintf(L"  F  list first/follow sets\n");
	wprintf(L"  G  print syntax graph\n");
	wprintf(L"  I  trace computation of first sets\n");
	wprintf(L"  J  list ANY and SYNC sets\n");
	wprintf(L"  P  print statistics\n");
	wprintf(L"  S  list symbol table\n");
	wprintf(L"  X  list cross reference table\n");
	wprintf(L"Scanner.frame and Parser.frame must be located in one of these directories:\n");
	wprintf(L"  1. In the specified -frames directory.\n");
	wprintf(L"  2. The current directory.\n");
	wprintf(L"  3. The same directory as the atg grammar.\n\n");
	wprintf(L"%s\n\n", PACKAGE_URL);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef _WIN32
// Special handling for Windows - see notes in CocoWin32Main.h
# include "CocoWin32Main.h"
#else
int main(int argc, char *argv[])
#endif
{
	wprintf(L"Coco/R C++ (%s)\n", PACKAGE_DATE);

#ifdef _WIN32
	std::wstring srcName;
#else
	std::string srcName;
#endif
	bool traceToFile = true;

	for (int i = 1; i < argc; i++)
	{
		if (coco_string_equal(argv[i], "-help"))
		{
			printUsage();
			return 0;
		}
		else if (coco_string_equal(argv[i], "-namespace"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -namespace");
				return 1;
			}
#ifdef _WIN32
			Tab::nsName = coco_stdString(argv[i]);
#else
			Tab::nsName = argv[i];
#endif
		}
		else if (coco_string_equal(argv[i], "-prefix"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -prefix");
				return 1;
			}
#ifdef _WIN32
			Tab::prefixName = coco_stdString(argv[i]);
#else
			Tab::prefixName = argv[i];
#endif
		}
		else if (coco_string_equal(argv[i], "-frames"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -frames");
				return 1;
			}
			Tab::frameDir = argv[i];
			Tab::frameDir += '/';
		}
		else if (coco_string_equal(argv[i], "-trace"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -trace");
				return 1;
			}
			traceToFile = true;
#ifdef _WIN32
			Tab::SetDDT(coco_stdString(argv[i]));
#else
			Tab::SetDDT(argv[i]);
#endif
		}
		else if (coco_string_equal(argv[i], "-trace2"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -trace2");
				return 1;
			}
			traceToFile = false;
#ifdef _WIN32
			Tab::SetDDT(coco_stdString(argv[i]));
#else
			Tab::SetDDT(argv[i]);
#endif
		}
		else if (coco_string_equal(argv[i], "-o"))
		{
			if (++i == argc)
			{
				printUsage("missing parameter on -o");
				return 1;
			}
			Tab::outDir = argv[i];
			Tab::outDir += '/';
		}
		else if (coco_string_equal(argv[i], "-lines"))
		{
			Tab::emitLines = true;
		}
		else if (coco_string_equal(argv[i], "-single"))
		{
			Tab::singleOutput = true;
		}
		else if (coco_string_equal(argv[i], "-bak"))
		{
			Tab::makeBackup = true;
		}
		else if (argv[i][0] == '-')
		{
#ifdef _WIN32
			wprintf(L"\nError: unknown option: '%ls'\n\n", argv[i]);
#else
			wprintf(L"\nError: unknown option: '%s'\n\n", argv[i]);
#endif
			printUsage();
			return 1;
		}
		else if (!srcName.empty())
		{
			printUsage("grammar can only be specified once");
			return 1;
		}
		else
		{
			srcName = argv[i];
		}
	}


	if (srcName.empty())
	{
		printUsage();
	}
	else
	{
		// find last '/' - path separator
		int pos = srcName.find_last_of('/');
#ifdef _WIN32
		// try '\\' separator (windows)
		if (pos < 0) pos = srcName.find_last_of('\\');
#endif
		if (pos >= 0)
		{
			Tab::srcDir = srcName.substr(0, pos+1);
		}
		if (Tab::outDir.empty())
		{
			Tab::outDir = Tab::srcDir;
		}

		// create Scanner/Parser and attach Tab, DFA and ParserGen
		Coco::Scanner scanner(srcName);
		Coco::Parser  parser(&scanner);

#ifdef _WIN32
		std::wstring traceFileName = Tab::outDir;
		traceFileName += L"trace.txt";
		if
		(
		    traceToFile
		 && (Tab::trace = _wfopen(traceFileName.c_str(), L"w")) == NULL
		)
		{
			wprintf(L"-- cannot write trace file to %ls\n", traceFileName.c_str());
			return 1;
		}
#else
		std::string traceFileName = Tab::outDir;
		traceFileName += "trace.txt";
		if
		(
		    traceToFile
		 && (Tab::trace = fopen(traceFileName.c_str(), "w")) == NULL
		)
		{
			wprintf(L"-- cannot write trace file to %s\n", traceFileName.c_str());
			return 1;
		}
#endif

		// attach Tab before creating Scanner/Parser generators
		// Note: the parser takes care of deleting the tab, dfa, pgen
		parser.tab  = new Coco::Tab(&parser);
		parser.dfa  = new Coco::DFA(&parser);
		parser.pgen = new Coco::ParserGen(&parser);

		// srcName is need when emitting #line
		parser.tab->srcName = srcName;

		parser.Parse();

		// see if anything was written
		if (traceToFile)
		{
			fclose(Tab::trace);

#ifdef _WIN32
			// obtain the FileSize
			Tab::trace = _wfopen(traceFileName.c_str(), L"r");
			fseek(Tab::trace, 0, SEEK_END);
			long fileSize = ftell(Tab::trace);
			fclose(Tab::trace);
			if (fileSize == 0)
			{
				_wremove(traceFileName.c_str());
			}
			else
			{
				wprintf(L"trace output is in %ls\n", traceFileName.c_str());
			}
#else
			// obtain the FileSize
			Tab::trace = fopen(traceFileName.c_str(), "r");
			fseek(Tab::trace, 0, SEEK_END);
			long fileSize = ftell(Tab::trace);
			fclose(Tab::trace);
			if (fileSize == 0)
			{
				remove(traceFileName.c_str());
			}
			else
			{
				wprintf(L"trace output is in %s\n", traceFileName.c_str());
			}
#endif
		}

		wprintf(L"%d errors detected\n", parser.errors->count);
		if (parser.errors->count != 0)
		{
			return 1;
		}
	}

	return 0;
}


// ************************************************************************* //
