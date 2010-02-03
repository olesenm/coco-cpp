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
#ifndef COCO_DFA_H__
#define COCO_DFA_H__

#include <stdlib.h>

#include "Comment.h"
#include "State.h"
#include "Symbol.h"
#include "Melted.h"
#include "Node.h"
#include "Tab.h"
#include "Target.h"

namespace Coco
{

// forward declarations
class Action;
class Parser;
class Tab;
class BitArray;

/*---------------------------------------------------------------------------*\
                             Class DFA Declaration
\*---------------------------------------------------------------------------*/
//! Generation of the Scanner Automaton
class DFA
{
	int maxStates;
	int lastStateNr;    //!< highest state number
	State *firstState;
	State *lastState;   //!< last allocated state
	int lastSimState;   //!< last non-melted state
	FILE* fram;         //!< scanner frame input
	FILE* gen;          //!< generated scanner file
	Symbol *curSy;      //!< current token to be recognized (in FindTrans)
	Node *curGraph;     //!< start of graph for current token (in FindTrans)
	bool dirtyDFA;      //!< DFA may become nondeterministic in MatchLiteral
	bool *existLabel;   //!< checking the Labels (to avoid the warning messages)

	Melted *firstMelted;    //!< head of melted state list
	Comment *firstComment;  //!< list of comments

	//! other Coco objects
	Parser     *parser;
	Tab        *tab;
	Errors     *errors;

public:
	bool ignoreCase;    //!< true if input should be treated case-insensitively
	bool hasCtxMoves;   //!< DFA has context transitions

	//---------- Output primitives
	static std::string Ch(wchar_t ch);
	static std::string ChCond(wchar_t ch);
	void  PutRange(CharSet *s);

	//---------- State handling
	State* NewState();
	void NewTransition
	(
		State *from, State *to,
		Node::nodeType typ, int sym, Node::transitionType tc
	);
	void CombineShifts();
	void FindUsedStates(State *state, BitArray *used);
	void DeleteRedundantStates();
	State* TheState(Node *p);
	void Step(State *from, Node *p, BitArray *stepped);
	void NumberNodes(Node *p, State *state, bool renumIter);
	void FindTrans(Node *p, bool start, BitArray *marked);
	void ConvertToStates(Node *p, Symbol *sym);

	// match string against current automaton; store it either as a fixedToken or as a litToken
	void MatchLiteral(const std::wstring& str, Symbol *sym);
	void SplitActions(State *state, Action *a, Action *b);
	bool Overlap(Action *a, Action *b);
	bool MakeUnique(State *state); // return true if actions were split
	void MeltStates(State *state);
	void FindCtxStates();
	void MakeDeterministic();
	void PrintStates();
	void CheckLabels();

	void PrintStatistics() const;

	//---------------------------- actions --------------------------------
	Action* FindAction(State *state, wchar_t ch);
	void GetTargetStates(Action *a, BitArray* &targets, Symbol* &endOf, bool &ctx);

	//------------------------- melted states ------------------------------
	Melted* NewMelted(BitArray *set, State *state);
	BitArray* MeltedSet(int nr);
	Melted* StateWithSet(BitArray *s);

	//------------------------ comments --------------------------------
	
	std::wstring CommentStr(Node *p);
	void NewComment(Node *from, Node *to, bool nested);

	//------------------------ scanner generation ----------------------
	void GenComBody(Comment *com);
	void GenCommentHeader(Comment *com, int i);
	void GenComment(Comment *com, int i);
	void CopyFramePart(const std::string& stop, const bool doOutput=true);
	std::wstring SymName(Symbol *sym); //!< real name value is stored in Tab.literals
	void GenLiterals();
	void WriteState(State *state);
	void WriteStartTab();
	void WriteScanner();
	DFA(Parser *parser);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
