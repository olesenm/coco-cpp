/*-------------------------------------------------------------------------
DFA -- Generation of the Scanner Automaton
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

#ifndef COCO_DFA_H__
#define COCO_DFA_H__

#include <stddef.h>
#include "Action.h"
#include "Comment.h"
#include "State.h"
#include "Symbol.h"
#include "Melted.h"
#include "Node.h"
#include "Target.h"

namespace Coco {

// forward declarations
class Parser;
class Tab;
class BitArray;

/*---------------------------------------------------------------------------*\
                             Class DFA Declaration
\*---------------------------------------------------------------------------*/
class DFA
{
public:

	//! End-of-file? ... unused?
	static const int eoF = -1;

	int maxStates;

	//! highest state number
	int lastStateNr;

	State *firstState;

	//! last allocated state
	State *lastState;

	//! last non melted state
	int lastSimState;

	//! scanner frame input
	FILE* fram;

	//! generated scanner file
	FILE* gen;

	//! current token to be recognized (in FindTrans)
	Symbol *curSy;

	//! start of graph for current token (in FindTrans)
	Node *curGraph;

	//! true if input should be treated case-insensitively
	bool ignoreCase;

	//! DFA may become nondeterministic in MatchLiteral
	bool dirtyDFA;

	//! DFA has context transitions
	bool hasCtxMoves;

	//! checking the Labels (in order to avoid the warning messages)
	bool *existLabel;

	//! other Coco objects
	Parser     *parser;
	Tab        *tab;
	Errors     *errors;

	//! trace file
	FILE* trace;

	//! head of melted state list
	Melted *firstMelted;

	//! list of comments
	Comment *firstComment;

	//---------- Output primitives
	wchar_t* Ch(wchar_t ch);
	wchar_t* ChCond(wchar_t ch);
	void  PutRange(CharSet *s);

	//---------- State handling
	State* NewState();
	void NewTransition(State *from, State *to, int typ, int sym, int tc);
	void CombineShifts();
	void FindUsedStates(State *state, BitArray *used);
	void DeleteRedundantStates();
	State* TheState(Node *p);
	void Step(State *from, Node *p, BitArray *stepped);
	void NumberNodes(Node *p, State *state, bool renumIter);
	void FindTrans (Node *p, bool start, BitArray *marked);
	void ConvertToStates(Node *p, Symbol *sym);

	// match string against current automaton; store it either as a fixedToken or as a litToken
	void MatchLiteral(wchar_t* s, Symbol *sym);
	void SplitActions(State *state, Action *a, Action *b);
	bool Overlap(Action *a, Action *b);
	bool MakeUnique(State *state); // return true if actions were split
	void MeltStates(State *state);
	void FindCtxStates();
	void MakeDeterministic();
	void PrintStates();
	void CheckLabels();

	//---------------------------- actions --------------------------------
	Action* FindAction(State *state, wchar_t ch);
	void GetTargetStates(Action *a, BitArray* &targets, Symbol* &endOf, bool &ctx);

	//------------------------- melted states ------------------------------
	Melted* NewMelted(BitArray *set, State *state);
	BitArray* MeltedSet(int nr);
	Melted* StateWithSet(BitArray *s);

	//------------------------ comments --------------------------------
	wchar_t* CommentStr(Node *p);
	void NewComment(Node *from, Node *to, bool nested);

	//------------------------ scanner generation ----------------------
	void GenComBody(Comment *com);
	void GenCommentHeader(Comment *com, int i);
	void GenComment(Comment *com, int i);
	void CopyFramePart(const wchar_t* stop);
	wchar_t* SymName(Symbol *sym); // real name value is stored in Tab.literals
	void GenLiterals ();
	int GenNamespaceOpen(const wchar_t* nsName);
	void GenNamespaceClose(int nrOfNs);
	void WriteState(State *state);
	void WriteStartTab();
	void OpenGen(const wchar_t* genName, bool backUp); /* pdt */
	void WriteScanner();
	DFA(Parser *parser);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
