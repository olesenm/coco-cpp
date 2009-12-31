/*-------------------------------------------------------------------------
ParserGen -- Generation of the Recursive Descent Parser
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
ported to C++ by Csaba Balazs, University of Szeged
extended by M. Loeberbauer & A. Woess, Univ. of Linz
with improvements by Pat Terry, Rhodes University

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
-------------------------------------------------------------------------*/

#ifndef COCO_PARSERGEN_H__
#define COCO_PARSERGEN_H__

#include "Position.h"
#include "Tab.h"
#include "Symbol.h"
#include "Scanner.h"
#include "DFA.h"

namespace Coco {

// forward declarations
class Errors;
class Parser;
class BitArray;

/*---------------------------------------------------------------------------*\
                          Class ParserGen Declaration
\*---------------------------------------------------------------------------*/

//! Parser generator
class ParserGen
{
public:
	static const char CR = '\r';    //!< carriage-return character
	static const char LF = '\n';    //!< line-feed character

	// unused: static const int EOF = -1;

	//! error codes
	static const int tErr    = 0;   //!< terminal error
	static const int altErr  = 1;   //!< alt error
	static const int syncErr = 2;   //!< sync error
	static const int maxTerm = 3;   //!< sets of size < maxTerm are enumerated

	Position *usingPos;     //!< "using" definitions from the attributed grammar
	int errorNr;            //!< highest parser error number
	Symbol *curSy;          //!< symbol whose production is currently generated

	FILE* fram;             //!< parser frame file
	FILE* gen;              //!< generated parser source file
	wchar_t* err;           //!< generated parser error messages

	ArrayList<BitArray> symSet;

	Tab *tab;               //!< other Coco objects
	FILE* trace;            //!< trace file
	Errors *errors;
	Buffer *buffer;

	void Indent(int n);     //!< indent with tabs to the specified level

	bool UseSwitch(Node *p);
	void CopyFramePart(const wchar_t* stop, const bool doOutput = true);
	void CopySourcePart(Position *pos, int indent);
	void GenErrorMsg(int errTyp, Symbol *sym);
	int  NewCondSet(BitArray *s);
	void GenCond(BitArray *s, Node *p);
	void PutCaseLabels(BitArray *s);
	void GenCode(Node *p, int indent, BitArray *isChecked);
	void GenTokens();
	void GenTokensHeader();
	void GenPragmas();
	void GenPragmasHeader();
	void GenCodePragmas();
	void GenProductions();
	void GenProductionsHeader();
	void InitSets();
	void WriteParser();
	void WriteStatistics();

	ParserGen(Parser *parser);
	~ParserGen();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
