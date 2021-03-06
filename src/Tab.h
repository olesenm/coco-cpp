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

#ifndef COCO_TAB_H__
#define COCO_TAB_H__

#include "ArrayList.h"
#include "BitArray.h"
#include "HashTable.h"

#include "Utils.h"
#include "Position.h"
#include "Symbol.h"
#include "Node.h"
#include "Graph.h"
#include "Sets.h"
#include "CharClass.h"
#include "Preproc.h"


namespace Coco
{

// forward declarations
class Errors;
class Parser;

/*---------------------------------------------------------------------------*\
                             Class Tab Declaration
\*---------------------------------------------------------------------------*/
//! Symbol Table Management
class Tab
{
public:
	// Constants
	//! String representations for the node types (in Node class)
	//! Corresponds to Node::nodeType
	static const char* nTyp[];

	//! String representations for the token kinds (in Symbol class)
	//! Corresponds to Symbol::kind
	static const char* tKind[];

	//! String for replacing the file prefix name
	static const std::string prefixMacro;

	// Static data members

	// miscellaneous generation controls
	static bool emitLines;         //!< emit line directives in generated parser
	static bool singleOutput;      //!< include the Scanner code in the Parser file
	static bool makeBackup;        //!< create .bak files for generated parser/scanner
	static bool ddt[10];           //!< debug and test switches

	static FILE* trace;            //!< trace file

	static std::string nsName;     //!< namespace for generated files
	static std::string prefixName; //!< prefix for generated files

#ifdef _WIN32
	static std::wstring srcDir;    //!< directory path of the atg file
	static std::wstring frameDir;  //!< directory containing the frame files
	static std::wstring outDir;    //!< directory for generated files

	// Data member
	std::wstring srcName;          //!< name of the atg file (including path)
#else
	static std::string srcDir;     //!< directory path of the atg file
	static std::string frameDir;   //!< directory containing the frame files
	static std::string shareDir;   //!< directory containing the frame files
	static std::string outDir;     //!< directory for generated files

	// Data member
	std::string srcName;           //!< name of the atg file (including path)
#endif

	// Data members

	Position *copyPos;      //!< position of verbatim copy (eg, copyright headers) in atg

	CharSet *ignored;       //!< characters ignored by the scanner
	Symbol *gramSy;         //!< root nonterminal; filled by ATG
	Symbol *eofSy;          //!< end of file symbol
	Symbol *noSym;          //!< used in case of an error
	BitArray allSyncSets;   //!< union of all synchronisation sets
	HashTable<Symbol> literals;    //!< symbols that are used as literals

	BitArray visited;       //!< mark list for graph traversals
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

	mutable Preproc preproc_;

	//! Construct and attach to Parser
	Tab(Parser *parser);
	~Tab();

	//---------------------------------------------------------------------
	//  Symbol list management
	//---------------------------------------------------------------------

	//! Create a new symbol (terminal, nonterminal or pragma)
	Symbol* NewSym(Node::nodeType typ, const std::wstring& name, int line);

	//! Find symbol by name in terminals and nonterminals
	Symbol* FindSym(const std::wstring& name);

	//! Return node number
	static int Num(Node *p);

	void PrintSym(const Symbol *sym) const;
	void PrintSymbolTable();
	void PrintSet(const BitArray& s, int indent);
	void PrintStatistics() const;

	//---------------------------------------------------------------------
	//  Syntax graph management
	//---------------------------------------------------------------------

	//! Add new node for a symbol on a line
	Node* NewNode(Node::nodeType typ, Symbol *sym, int line);
	//! Add new node for a null symbol (on line 0)
	Node* NewNode(Node::nodeType typ);
	//! Add new node with a null symbol (on line 0) and a sub-node
	Node* NewNode(Node::nodeType typ, Node* sub);
	//! Add new node with a null symbol (on line 0) and a value val
	Node* NewNode(Node::nodeType typ, int val);

	void MakeFirstAlt(Graph *g);
	void MakeAlternative(Graph *g1, Graph *g2);
	static void MakeSequence(Graph *g1, Graph *g2);
	void MakeIteration(Graph *g);
	void MakeOption(Graph *g);
	static void Finish(Graph *g);
	void DeleteNodes();
	Graph* StrToGraph(const std::wstring& str);

	//! set transition code in the graph rooted at p
	static void SetContextTrans(Node *p);

	//------------ graph deletability check -----------------

	static bool DelGraph(Node* p);
	static bool DelSubGraph(Node* p);
	static bool DelNode(Node* p);

	//----------------- graph printing ----------------------

	static int Ptr(const Node *p, bool up);
	static std::string Pos(const Position *pos);
	void PrintNodes();

	//---------------------------------------------------------------------
	//  Character class management
	//---------------------------------------------------------------------

	CharClass* NewCharClass(const std::wstring& name, CharSet *s);
	CharClass* FindCharClass(const std::wstring& name);
	CharClass* FindCharClass(CharSet *s);
	CharSet* CharClassSet(int i);

	//----------- character class printing

	static std::string Ch(const wchar_t ch);
	void WriteCharSet(const CharSet *s);
	void WriteCharClasses();

	//---------------------------------------------------------------------
	//  Symbol set computations
	//---------------------------------------------------------------------

	//! Computes the first set for the given Node
	BitArray First0(Node *p, BitArray& mark);
	BitArray First(Node *p);
	void CompFirstSets();
	void CompFollow(Node *p);
	void Complete(Symbol *sym);
	void CompFollowSets();
	static Node* LeadingAny(Node *p);
	void FindAS(Node *p); // find ANY sets
	void CompAnySets();
	BitArray Expected(Node *p, Symbol *curSy);
	//! does not look behind resolvers; only called during LL(1) test and in CheckRes
	BitArray Expected0(Node *p, Symbol *curSy);
	void CompSync(Node *p);
	void CompSyncSets();
	void SetupAnys();
	void CompDeletableSymbols();
	void RenumberPragmas();
	void CompSymbolSets();

	//---------------------------------------------------------------------
	//  String handling
	//---------------------------------------------------------------------

	wchar_t  Hex2Char(const wchar_t* s, const int len);
	static std::wstring Char2Hex(const wchar_t ch);

	//! Replace escape sequences by their Unicode values.
	std::wstring Unescape(const std::wstring&);

	//! Replace non-ASCII Unicode values by an escape sequence.
	static std::wstring Escape(const std::wstring&);

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
	void CheckOverlap(const BitArray& s1, const BitArray& s2, int cond);
	void CheckAlts(Node *p);
	void CheckLL1();

	//------------- check if resolvers are legal  --------------------

	void ResErr(const Node *p, const wchar_t* msg);
	void CheckRes(Node *p, bool rslvAllowed);
	void CheckResolvers();

	//------------- check if every nts has a production --------------------

	bool NtsComplete();

	//-------------- check if every nts can be reached  -----------------

	void MarkReachedNts(Node *p);
	bool AllNtReached();

	//--------- check if every nts can be derived to terminals  ------------

	//! true if graph can be derived to terminals
	bool IsTerm(const Node *p, const BitArray& mark) const;
	bool AllNtToTerm();

	//---------------------------------------------------------------------
	//  Cross reference list
	//---------------------------------------------------------------------

	void XRef();

	//! Dispatch a directive of the type name=value
	void DispatchDirective(const std::string& str);

	//! Set trace flags
	static void SetDDT(const std::string& str);

	//---------------------------------------------------------------------
	//  Output file generation, common to DFA and ParserGen
	//---------------------------------------------------------------------

	//! Generate begin of namespace enclosure
	int GenNamespaceOpen(FILE* dst) const;
	//! Generate end of namespace enclosure
	static void GenNamespaceClose(FILE* dst, int nrOfNs);

	//! Open a frame file for reading, return NULL on failure
	FILE* OpenFrameFile(const std::string& frameName) const;

	//! Open a generated file (.h or .cpp), return NULL on failure
	FILE* OpenGenFile(const std::string& genName) const;

	//! Copy frame part from src to dst until the stop mark is seen.
	//! Returns true on success. Optionally possible to suppress output.
	bool CopyFramePart
	(
		FILE* dst,
		FILE* src,
		const std::string& stop,
		const bool doOutput = true
	) const;

	void CopySourcePart
	(
		FILE* dest,
		Position *pos,
		int indent,
		bool allowLines=true
	);

	//! add notice about generating program etc.
	void AddNotice(FILE* dst) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
