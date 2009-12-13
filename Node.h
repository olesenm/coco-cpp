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

#include <stdio.h>
#include "Position.h"
#include "State.h"
#include "Scanner.h"

namespace Coco {

// forward declarations
class Symbol;
class BitArray;

/*---------------------------------------------------------------------------*\
                            Class Node Declaration
\*---------------------------------------------------------------------------*/
class Node {
public:
	// enum constants for node kinds
	static int t;      //!< terminal symbol
	static int pr;     //!< pragma
	static int nt;     //!< nonterminal symbol
	static int clas;   //!< character class
	static int chr;    //!< character
	static int wt;     //!< weak terminal symbol
	static int any;    //!< any character
	static int eps;    //!< empty
	static int sync;   //!< synchronization symbol
	static int sem;    //!< semantic action: (. .)
	static int alt;    //!< alternative: |
	static int iter;   //!< iteration: { }
	static int opt;    //!< option: [ ]
	static int rslv;   //!< resolver expression

	//! normal transition
	static int normalTrans;

	//! context transition
	static int contextTrans;

	//! node number
	int      n;

	//! node type, one of t, nt, wt, chr, clas, any, eps, sem, sync, alt, iter, opt, rslv
	int      typ;

	//! to successor node
	Node     *next;

	//! alt: to next alternative
	Node     *down;

	//! alt, iter, opt: to first node of substructure
	Node     *sub;

	//! true: "next" leads to successor in enclosing structure
	bool     up;

	//! nt, t, wt: symbol represented by this node
	Symbol   *sym;

	//! chr:  ordinal character value, clas: index of character class
	int      val;

	//! chr, clas: transition code
	int      code;

	//! any, sync: the set represented by this node
	BitArray *set;

	//! nt, t, wt: pos of actual attributes
	//! sem:  pos of semantic action in source text
	//! rslv: pos of resolver in source text
	Position *pos;

	//! source text line number of item in this node
	int      line;

	//! DFA state corresponding to this node
	// (only used in DFA.ConvertToStates)
	State    *state;

	Node(int typ, Symbol *sym, int line);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
