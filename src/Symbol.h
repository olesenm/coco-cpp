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

#ifndef COCO_SYMBOL_H__
#define COCO_SYMBOL_H__

#include "Node.h"
#include "BitArray.h"
#include "Position.h"
#include "Utils.h"

#include <string>

namespace Coco
{

// forward declarations
class Node;

/*---------------------------------------------------------------------------*\
                           Class Symbol Declaration
\*---------------------------------------------------------------------------*/

//! Grammar Symbol
class Symbol
{
public:
	//! Enumeration for the kind of tokens
	enum symbolType
	{
		fixedToken    = 0,  //!< eg, 'a' ('b' | 'c') (structure of literals)
		classToken    = 1,  //!< eg, digit {digit}   (at least one char class)
		litToken      = 2,  //!< eg, "while"
		classLitToken = 3   //!< eg, letter {letter} but without literals that have the same structure*/
	};

	int      n;           //!< symbol number
	const Node::nodeType typ;  //!< node type: t, nt, pr, unknown, rslv
	const std::wstring name;   //!< symbol name
	Node     *graph;      //!< nt: to first node of syntax graph
	symbolType tokenKind; //!< t:  token kind (fixedToken, classToken, ...)
	bool     deletable;   //!< nt: true if nonterminal is deletable
	bool     firstReady;  //!< nt: true if terminal start symbols have already been computed
	BitArray first;       //!< nt: terminal start symbols
	BitArray follow;      //!< nt: terminal followers
	BitArray nts;         //!< nt: nonterminals whose followers have to be added to this sym
	int      line;        //!< source text line number of item in this node
	Position *attrPos;    //!< nt: position of attributes in source text (or null)
	Position *semPos;     //!< pr: pos of semantic action in source text (or null)
	                      //!< nt: pos of local declarations in source text (or null)

	//! Construct from components
	Symbol(Node::nodeType nodeTyp, const std::wstring& symName, int lineNr)
	:
		n(0),
		typ(nodeTyp),
		name(symName),
		graph(0),
		tokenKind(fixedToken),
		deletable(false),
		firstReady(false),
		first(0),
		follow(0),
		nts(0),
		line(lineNr),
		attrPos(0),
		semPos(0)
	{}

	//! Destructor frees name
	virtual ~Symbol()
	{
		if (attrPos) { delete attrPos; }
		if (semPos)  { delete semPos; }
	}

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
