/*-------------------------------------------------------------------------
Tab -- Symbol Table Management
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

#ifndef COCO_TAB_H__
#define COCO_TAB_H__

#include "ArrayList.h"
#include "HashTable.h"
#include "StringBuilder.h"
#include "Utils.h"
#include "Position.h"
#include "Symbol.h"
#include "Node.h"
#include "Graph.h"
#include "Sets.h"
#include "CharClass.h"


namespace Coco {

// forward declarations
class Errors;
class Parser;
class BitArray;

/*---------------------------------------------------------------------------*\
                             Class Tab Declaration
\*---------------------------------------------------------------------------*/
//! Symbol Table Management
class Tab {
public:
	// Constants

	static const char CR = '\r';    //!< carriage-return character
	static const char LF = '\n';    //!< line-feed character

	//! String representations for the node types (in Node class)
	//! Corresponds to Node::nodeType
	static const char* nTyp[];

	//! String representations for the token kinds (in Symbol class)
	//! Corresponds to Symbol::kind
	static const char* tKind[];

	//! String for replacing the file prefix name
	static const wchar_t* prefixMacro;

	// Static data members

	// miscellaneous generation controls
	static bool emitLines;         //!< emit line directives in generated parser
	static bool singleOutput;      //!< include the Scanner code in the Parser file
	static bool makeBackup;        //!< create .bak files for generated parser/scanner
	static bool explicitEOF;       //!< user must explicitly add EOF in grammar
	static bool ddt[10];           //!< debug and test switches

	static std::string nsName;     //!< namespace for generated files
	static std::string prefixName; //!< prefix for generated files

	static wchar_t* srcDir;        //!< directory path of the atg file
	static wchar_t* frameDir;      //!< directory containing the frame files
	static wchar_t* outDir;        //!< directory for generated files

	static FILE* trace;            //!< trace file

	// Data members

	wchar_t* srcName;       //!< name of the atg file (including path)

	Position *copyPos;      //!< position of verbatim copy (eg, copyright headers) in atg

	CharSet *ignored;       //!< characters ignored by the scanner
	Symbol *gramSy;         //!< root nonterminal; filled by ATG
	Symbol *eofSy;          //!< end of file symbol
	Symbol *noSym;          //!< used in case of an error
	BitArray *allSyncSets;  //!< union of all synchronisation sets
	HashTable<Symbol> literals;    //!< symbols that are used as literals

	BitArray *visited;      //!< mark list for graph traversals
	Symbol *curSy;          //!< current symbol in computation of sets

	Parser *parser;         //!< other Coco objects
	Errors *errors;
	Buffer *buffer;

	ArrayList<Symbol> terminals;
	ArrayList<Symbol> pragmas;
	ArrayList<Symbol> nonterminals;
	ArrayList<Node> nodes;
	ArrayList<CharClass> classes;

	Node *dummyNode;
	int dummyName;


	Tab(Parser *parser);
	~Tab();

	//---------------------------------------------------------------------
	//  Symbol list management
	//---------------------------------------------------------------------

	//! Create a new symbol (terminal, nonterminal or pragma)
	Symbol* NewSym(Node::nodeType typ, const wchar_t* name, int line);

	//! Find symbol by name in terminals and nonterminals
	Symbol* FindSym(const wchar_t* name);

	//! Return node number
	static int Num(Node *p);

	void PrintSym(Symbol *sym) const;
	void PrintSymbolTable();
	void PrintSet(BitArray *s, int indent);
	void PrintStatistics() const;

	//---------------------------------------------------------------------
	//  Syntax graph management
	//---------------------------------------------------------------------

	Node* NewNode(Node::nodeType typ, Symbol *sym, int line);
	Node* NewNode(Node::nodeType typ, Node* sub);
	Node* NewNode(Node::nodeType typ, int val, int line);
	void MakeFirstAlt(Graph *g);
	void MakeAlternative(Graph *g1, Graph *g2);
	static void MakeSequence(Graph *g1, Graph *g2);
	void MakeIteration(Graph *g);
	void MakeOption(Graph *g);
	static void Finish(Graph *g);
	void DeleteNodes();
	Graph* StrToGraph(const wchar_t* str);

	//! set transition code in the graph rooted at p
	static void SetContextTrans(Node *p);

	//------------ graph deletability check -----------------

	static bool DelGraph(Node* p);
	static bool DelSubGraph(Node* p);
	static bool DelNode(Node* p);

	//----------------- graph printing ----------------------

	static int Ptr(Node *p, bool up);
	static wchar_t* Pos(Position *pos);
	void PrintNodes();

	//---------------------------------------------------------------------
	//  Character class management
	//---------------------------------------------------------------------

	CharClass* NewCharClass(const wchar_t* name, CharSet *s);
	CharClass* FindCharClass(const wchar_t* name);
	CharClass* FindCharClass(CharSet *s);
	CharSet* CharClassSet(int i);

	//----------- character class printing

	static wchar_t* Ch(const wchar_t ch);
	void WriteCharSet(CharSet *s);
	void WriteCharClasses();

	//---------------------------------------------------------------------
	//  Symbol set computations
	//---------------------------------------------------------------------

	//! Computes the first set for the given Node
	BitArray* First0(Node *p, BitArray *mark);
	BitArray* First(Node *p);
	void CompFirstSets();
	void CompFollow(Node *p);
	void Complete(Symbol *sym);
	void CompFollowSets();
	static Node* LeadingAny(Node *p);
	void FindAS(Node *p); // find ANY sets
	void CompAnySets();
	BitArray* Expected(Node *p, Symbol *curSy);
	//! does not look behind resolvers; only called during LL(1) test and in CheckRes
	BitArray* Expected0(Node *p, Symbol *curSy);
	void CompSync(Node *p);
	void CompSyncSets();
	void SetupAnys();
	void CompDeletableSymbols();
	void RenumberPragmas();
	void CompSymbolSets();

	//---------------------------------------------------------------------
	//  String handling
	//---------------------------------------------------------------------

	wchar_t  Hex2Char(const wchar_t* s);
	static wchar_t* Char2Hex(const wchar_t ch);
	wchar_t* Unescape(const wchar_t* s);
	static wchar_t* Escape(const wchar_t* s);

	//---------------------------------------------------------------------
	//  Grammar checks
	//---------------------------------------------------------------------

	bool GrammarOk();

	//--------------- check for circular productions ----------------------

	//! node of list for finding circular productions
	class CNode {
	public:
		Symbol *left, *right;
		CNode(Symbol *l, Symbol *r) : left(l), right(r) {}
	};

	//! Check for circular productions
	void GetSingles(Node *p, ArrayList<Symbol>& singles);
	bool NoCircularProductions();

	//--------------- check for LL(1) errors ----------------------

	void LL1Error(int cond, Symbol *sym);
	void CheckOverlap(BitArray *s1, BitArray *s2, int cond);
	void CheckAlts(Node *p);
	void CheckLL1();

	//------------- check if resolvers are legal  --------------------

	void ResErr(Node *p, const wchar_t* msg);
	void CheckRes(Node *p, bool rslvAllowed);
	void CheckResolvers();

	//------------- check if every nts has a production --------------------

	bool NtsComplete();

	//-------------- check if every nts can be reached  -----------------

	void MarkReachedNts(Node *p);
	bool AllNtReached();

	//--------- check if every nts can be derived to terminals  ------------

	//! true if graph can be derived to terminals
	bool IsTerm(Node *p, BitArray *mark);
	bool AllNtToTerm();

	//---------------------------------------------------------------------
	//  Cross reference list
	//---------------------------------------------------------------------

	void XRef();

	//! Dispatch a directive of the type name=value
	void DispatchDirective(const wchar_t* str);

	//! Set trace flags
	static void SetDDT(const wchar_t* str);

	//---------------------------------------------------------------------
	//  Output file generation, common to DFA and ParserGen
	//---------------------------------------------------------------------

	//! Generate begin of namespace enclosure
	int GenNamespaceOpen(FILE*) const;
	//! Generate end of namespace enclosure
	static void GenNamespaceClose(FILE*, int nrOfNs);

	//! Open a frame file for reading, return NULL on failure
	FILE* OpenFrameFile(const wchar_t* frameName) const;

	//! Open a generated file (.h or .cpp), return NULL on failure
	FILE* OpenGenFile(const std::string& genName) const;

	//! Copy frame part from istr, to ostr until stop mark is seen
	//  Return true on success. Optionally possible to suppress output.
	bool CopyFramePart
	(
		FILE* ostr,
		FILE* istr,
		const wchar_t* stop,
		const bool doOutput = true
	) const;

	void CopySourcePart(FILE *dest, Position *pos, int indent, bool emitLines = false);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
