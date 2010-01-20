/*-------------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
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

#ifndef COCO_NODE_H__
#define COCO_NODE_H__

#include "Position.h"
#include "State.h"

namespace Coco
{

// forward declarations
class Symbol;
class BitArray;

/*---------------------------------------------------------------------------*\
                            Class Node Declaration
\*---------------------------------------------------------------------------*/

//! ...
class Node
{
public:
	//! Enumeration for the types of nodes
	enum nodeType
	{
		t    =  1,  //!< terminal symbol
		pr   =  2,  //!< pragma
		nt   =  3,  //!< nonterminal symbol
		clas =  4,  //!< character class
		chr  =  5,  //!< character
		wt   =  6,  //!< weak terminal symbol
		any  =  7,  //!< any character
		eps  =  8,  //!< empty
		sync =  9,  //!< synchronization symbol
		sem  = 10,  //!< semantic action: (. .)
		alt  = 11,  //!< alternative: |
		iter = 12,  //!< iteration: { }
		opt  = 13,  //!< option: [ ]
		rslv = 14,  //!< resolver expression
		invalid_ = 0  //!< internal use only
	};

	//! Enumeration for the types of transitions
	enum transitionType
	{
		normalTrans  = 0,  //!< normal transition
		contextTrans = 1   //!< context transition
	};

	int      n;     //!< node number
	nodeType typ;   //!< node type, one of t, nt, wt, chr, clas, any, eps, sem, sync, alt, iter, opt, rslv
	Symbol   *sym;  //!< nt, t, wt: symbol represented by this node
	int      val;   //!< chr:  ordinal character value, clas: index of character class
	transitionType code;  //!< chr, clas: transition code
	int      line;  //!< source text line number of item in this node

	BitArray *set;  //!< any, sync: the set represented by this node

	//! nt, t, wt: pos of actual attributes
	//! sem:  pos of semantic action in source text
	//! rslv: pos of resolver in source text
	Position *pos;

	//! DFA state corresponding to this node
	//! (only used in DFA.ConvertToStates)
	State    *state;

	bool     up;    //!< true: "next" leads to successor in enclosing structure
	Node     *next; //!< to successor node
	Node     *down; //!< alt: to next alternative
	Node     *sub;  //!< alt, iter, opt: to first node of substructure

	Node(nodeType theTyp, Symbol* theSym, int lineNr);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
