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
#ifndef COCO_PARSERGEN_H__
#define COCO_PARSERGEN_H__

#include "Position.h"
#include "Tab.h"
#include "Symbol.h"
#include "Utils.h"
#include "DFA.h"

namespace Coco
{

// forward declarations
class Errors;
class Parser;
class BitArray;

/*---------------------------------------------------------------------------*\
                          Class ParserGen Declaration
\*---------------------------------------------------------------------------*/

//! Parser generator -- Generation of the Recursive Descent Parser
class ParserGen
{
	//! Type of errors
	enum errorType
	{
		tErr    = 0,      //!< terminal error
		altErr  = 1,      //!< alt error
		syncErr = 2,      //!< sync error
		maxTerm = 3       //!< sets of size < maxTerm are enumerated
	};

	int errorNr;          //!< highest parser error number
	Symbol *curSy;        //!< symbol whose production is currently generated

	FILE* fram;           //!< parser frame file
	FILE* gen;            //!< generated parser source file
	std::wstring err;     //!< generated parser error messages

	ArrayList<BitArray> symSet;

	// other Coco objects
	Tab *tab;
	Errors *errors;

public:
	Position *preamblePos;   //!< position of preamble (eg, includes) in attributed grammar
	Position *semDeclPos;    //!< position of global semantic declarations
	Position *initCodePos;   //!< position of initialization code
	Position *deinitCodePos; //!< position of de-initialization (destructor) code
	Position *extraCodePos;  //!< position of extra code for the Parser.cpp


	ParserGen(Parser *parser);
	~ParserGen();

	void Indent(int n);     //!< indent with tabs to the specified level

	bool UseSwitch(Node *p);
	void CopyFramePart(const std::string& stop, const bool doOutput=true);
	void CopySourcePart(Position *pos, int indent);
	void GenErrorMsg(errorType errTyp, Symbol *sym);
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
	void PrintStatistics() const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
