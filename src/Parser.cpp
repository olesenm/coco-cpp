/*---------------------------------------------------------------------------*\
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

License
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <sstream>

#include "Parser.h"

namespace Coco {


const wchar_t* Parser::noString = L"-none-";



//! @cond fileScope
//

//
//  Create by copying str - only used locally
inline static wchar_t* coco_string_create(const wchar_t* str)
{
	const int len = wcslen(str);
	wchar_t* dest = new wchar_t[len + 1];
	wcsncpy(dest, str, len);
	dest[len] = 0;
	return dest;
}


// Free storage and nullify the argument
inline static void coco_string_delete(wchar_t* &str)
{
	delete[] str;
	str = NULL;
}
//
//! @endcond fileScope


// ----------------------------------------------------------------------------
// Parser Implementation
// ----------------------------------------------------------------------------

void Parser::SynErr(int n)
{
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}


void Parser::SemErr(const std::wstring& msg)
{
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}


bool Parser::isUTF8() const
{
	return scanner && scanner->buffer && scanner->buffer->isUTF8();
}


void Parser::Get()
{
	for (;;)
	{
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT)
		{
			++errDist;
			break;
		}
		if (la->kind == 50) {
				Tab::SetDDT(coco_stdString(la->val+1));
		}
		if (la->kind == 51) {
				tab->DispatchDirective(coco_stdString(la->val+1));
		}
		if (dummyToken != t)
		{
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}


void Parser::Expect(int n)
{
	if (la->kind == n)
	{
		Get();
	}
	else
	{
		SynErr(n);
	}
}


void Parser::ExpectWeak(int n, int follow)
{
	if (la->kind == n)
	{
		Get();
	}
	else
	{
		SynErr(n);
		while (!StartOf(follow))
		{
			Get();
		}
	}
}


bool Parser::WeakSeparator(int n, int syFol, int repFol)
{
	if (la->kind == n)
	{
		Get();
		return true;
	}
	else if (StartOf(repFol))
	{
		return false;
	}
	else
	{
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0)))
		{
			Get();
		}
		return StartOf(syFol);
	}
}


void Parser::Coco()
{
	Symbol *sym; Graph *g; std::wstring grammarName; CharSet *s;
	if (la->kind == 6) {
		Get();
		int beg = t->pos + t->length(); int line = t->line;
		while (StartOf(1)) {
			Get();
		}
		tab->copyPos = new Position(beg, la->pos, 0, line);
		Expect(7);
	}
	if (StartOf(2)) {
		Get();
		int beg = t->pos; int line = t->line;
		while (StartOf(3)) {
			Get();
		}
		pgen->preamblePos = new Position(beg, la->pos, 0, line);
	}
	Expect(8);
	genScanner = true;
	Expect(1);
	grammarName = t->val;
	if (StartOf(4)) {
		Get();
		int beg = t->pos; int line = t->line;
		while (StartOf(4)) {
			Get();
		}
		pgen->semDeclPos = new Position(beg, la->pos, 0, line);
	}
	if (la->kind == 9) {
		Get();
		if (StartOf(5)) {
			Get();
			int beg = t->pos; int line = t->line;
			while (StartOf(5)) {
				Get();
			}
			pgen->initCodePos = new Position(beg, la->pos, 0, line);
		}
		Expect(10);
	}
	if (la->kind == 11) {
		Get();
		if (StartOf(6)) {
			Get();
			int beg = t->pos; int line = t->line;
			while (StartOf(6)) {
				Get();
			}
			pgen->deinitCodePos = new Position(beg, la->pos, 0, line);
		}
		Expect(12);
	}
	if (la->kind == 13) {
		Get();
		if (StartOf(7)) {
			Get();
			int beg = t->pos; int line = t->line;
			while (StartOf(7)) {
				Get();
			}
			pgen->extraCodePos = new Position(beg, la->pos, 0, line);
		}
		Expect(14);
	}
	if (la->kind == 15) {
		Get();
		dfa->ignoreCase = true;
	}
	if (la->kind == 16) {
		Get();
		while (la->kind == 1) {
			SetDecl();
		}
	}
	if (la->kind == 17) {
		Get();
		while (la->kind == 1 || la->kind == 3 || la->kind == 5) {
			TokenDecl(Node::t);
		}
	}
	if (la->kind == 18) {
		Get();
		while (la->kind == 1 || la->kind == 3 || la->kind == 5) {
			TokenDecl(Node::pr);
		}
	}
	while (la->kind == 19) {
		Get();
		Graph *g1, *g2; bool nested = false;
		Expect(20);
		TokenExpr(g1);
		Expect(21);
		TokenExpr(g2);
		if (la->kind == 22) {
			Get();
			nested = true;
		}
		dfa->NewComment(g1->l, g2->l, nested);
	}
	while (la->kind == 23) {
		Get();
		Set(s);
		tab->ignored->Or(s);
	}
	while (!(la->kind == 0 || la->kind == 24)) {SynErr(50); Get();}
	Expect(24);
	if (genScanner) dfa->MakeDeterministic();
	tab->DeleteNodes();

	while (la->kind == 1) {
		Get();
		sym = tab->FindSym(t->val);
		bool undef = (sym == NULL);
		if (undef) sym = tab->NewSym(Node::nt, t->val, t->line);
		else {
		  if (sym->typ == Node::nt) {
		    if (sym->graph != NULL)
		      SemErr(L"name declared twice");
		  } else {
		      SemErr(L"this symbol kind not allowed on left side of production");
		  }
		  sym->line = t->line;
		}
		bool noAttrs = (sym->attrPos == NULL);
		sym->attrPos = NULL;

		if (la->kind == 32 || la->kind == 34) {
			AttrDecl(sym);
		}
		if (!undef && noAttrs != (sym->attrPos == NULL))
		 SemErr(L"attribute mismatch between declaration and use of this symbol");

		if (la->kind == 47) {
			SemText(sym->semPos);
		}
		ExpectWeak(25, 8);
		Expression(g);
		sym->graph = g->l;
		tab->Finish(g);

		ExpectWeak(26, 9);
	}
	Expect(27);
	Expect(1);
	if (grammarName != t->val)
	 SemErr(L"name does not match grammar name");
	tab->gramSy = tab->FindSym(grammarName);
	if (tab->gramSy == NULL)
	  SemErr(L"missing production for grammar name");
	else {
	  sym = tab->gramSy;
	  if (sym->attrPos != NULL)
	    SemErr(L"grammar symbol must not have attributes");
	}
	tab->noSym = tab->NewSym(Node::t, L"???", 0); // noSym gets highest number
	tab->SetupAnys();
	tab->RenumberPragmas();
	if (tab->ddt[2]) tab->PrintNodes();
	if (errors->count == 0) {
	  wprintf(L"checking\n");
	  tab->CompSymbolSets();
	  if (tab->ddt[7]) tab->XRef();
	  if (tab->GrammarOk()) {
	    wprintf(L"parser");
	    pgen->WriteParser();
	    if (genScanner) {
	      wprintf(L" + scanner");
	      dfa->WriteScanner();
	      if (tab->ddt[0]) dfa->PrintStates();
	    }
	    wprintf(L" generated\n");
	    if (tab->ddt[8]) {
	        tab->PrintStatistics();
	        pgen->PrintStatistics();
	        dfa->PrintStatistics();
	    }
	  }
	}
	if (tab->ddt[6]) tab->PrintSymbolTable();

	Expect(26);
}


void Parser::SetDecl()
{
	CharSet *s;
	Expect(1);
	std::wstring name = t->val;
	CharClass *c = tab->FindCharClass(name);
	if (c != NULL) {
	  SemErr(L"name declared twice");
	}

	Expect(25);
	Set(s);
	if (s->Elements() == 0) {
	 SemErr(L"character set must not be empty");
	}
	tab->NewCharClass(name, s);

	Expect(26);
}


void Parser::TokenDecl(Node::nodeType typ)
{
	std::wstring name; int kind; Graph *g;
	Sym(name, kind);
	Symbol *sym = tab->FindSym(name);
	if (sym != NULL) SemErr(L"name declared twice");
	else {
	  sym = tab->NewSym(typ, name, t->line);
	  sym->tokenKind = Symbol::fixedToken;
	}
	tokenString.clear();

	while (!(StartOf(10))) {SynErr(51); Get();}
	if (la->kind == 25) {
		Get();
		TokenExpr(g);
		Expect(26);
		if (kind == isLiteral) SemErr(L"a literal must not be declared with a structure");
		tab->Finish(g);
		if (tokenString.empty() || tokenString == noString)
		  dfa->ConvertToStates(g->l, sym);
		else { // TokenExpr is a single string
		  if (tab->literals[tokenString] != NULL)
		    SemErr(L"token string declared twice");
		  tab->literals.Set(tokenString, sym);
		  dfa->MatchLiteral(tokenString, sym);
		}

	} else if (StartOf(11)) {
		if (kind == isIdent) genScanner = false;
		else dfa->MatchLiteral(sym->name, sym);

	} else SynErr(52);
	if (la->kind == 47) {
		SemText(sym->semPos);
		if (typ != Node::pr) SemErr(L"semantic action not allowed here");
	}
}


void Parser::TokenExpr(Graph* &g)
{
	Graph *g2;
	TokenTerm(g);
	bool first = true;
	while (WeakSeparator(36,13,12) ) {
		TokenTerm(g2);
		if (first) { tab->MakeFirstAlt(g); first = false; }
		tab->MakeAlternative(g, g2);

	}
}


void Parser::Set(CharSet* &s)
{
	CharSet *s2;
	SimSet(s);
	while (la->kind == 28 || la->kind == 29) {
		if (la->kind == 28) {
			Get();
			SimSet(s2);
			s->Or(s2);
		} else {
			Get();
			SimSet(s2);
			s->Subtract(s2);
		}
	}
}


void Parser::AttrDecl(Symbol *sym)
{
	if (la->kind == 32) {
		Get();
		int beg = la->pos; int col = la->col; int line = la->line;
		while (StartOf(14)) {
			if (StartOf(15)) {
				Get();
			} else {
				Get();
				SemErr(L"bad string in attributes");
			}
		}
		Expect(33);
		if (t->pos > beg)
		 sym->attrPos = new Position(beg, t->pos, col, line);
	} else if (la->kind == 34) {
		Get();
		int beg = la->pos; int col = la->col; int line = la->line;
		while (StartOf(16)) {
			if (StartOf(17)) {
				Get();
			} else {
				Get();
				SemErr(L"bad string in attributes");
			}
		}
		Expect(35);
		if (t->pos > beg)
		 sym->attrPos = new Position(beg, t->pos, col, line);
	} else SynErr(53);
}


void Parser::SemText(Position* &pos)
{
	Expect(47);
	int beg = la->pos; int col = la->col; int line = t->line;
	while (StartOf(18)) {
		if (StartOf(19)) {
			Get();
		} else if (la->kind == 4) {
			Get();
			SemErr(L"bad string in semantic action");
		} else {
			Get();
			SemErr(L"missing end of previous semantic action");
		}
	}
	Expect(48);
	pos = new Position(beg, t->pos, col, line);
}


void Parser::Expression(Graph* &g)
{
	Graph *g2;
	Term(g);
	bool first = true;
	while (WeakSeparator(36,21,20) ) {
		Term(g2);
		if (first) { tab->MakeFirstAlt(g); first = false; }
		tab->MakeAlternative(g, g2);

	}
}


void Parser::SimSet(CharSet* &s)
{
	int n1, n2; s = new CharSet();
	if (la->kind == 1) {
		Get();
		CharClass *c = tab->FindCharClass(t->val);
		if (c == NULL) SemErr(L"undefined name"); else s->Or(c->set);

	} else if (la->kind == 3) {
		Get();
		std::wstring name = tab->Unescape
		(
		    std::wstring(t->val, 1, t->length()-2)
		);
		for
		(
		    std::wstring::const_iterator iter = name.begin();
		    iter != name.end();
		    ++iter
		)
		{
		    wchar_t ch = *iter;
		    if (dfa->ignoreCase && ch >= 'A' && ch <= 'Z')
		    {
		        ch += ('a' - 'A'); // ch.ToLower()
		    }
		    s->Set(ch);
		}

	} else if (la->kind == 5) {
		Char(n1);
		s->Set(n1);
		if (la->kind == 30) {
			Get();
			Char(n2);
			for (int i = n1; i <= n2; i++) s->Set(i);
		}
	} else if (la->kind == 31) {
		Get();
		s = new CharSet(); s->Fill();
	} else SynErr(54);
}


void Parser::Char(int &n)
{
	Expect(5);
	n = 0;
	std::wstring name = tab->Unescape
	(
	    std::wstring(t->val, 1, t->length()-2)
	);
	// "<= 1" instead of "== 1" to allow the escape sequence '\0' in C++
	if (name.size() == 1)
	{
	    n = name[0];
	}
	else if (!name.empty())
	{
	    SemErr(L"unacceptable character value");
	}
	// n is int, we can create lowercase directly
	if (dfa->ignoreCase && n >= 'A' && n <= 'Z')
	{
	    n += ('a' - 'A');
	}

}


void Parser::Sym(std::wstring &name, int &kind)
{
	name = L"???"; kind = isIdent;
	if (la->kind == 1) {
		Get();
		kind = isIdent; name = t->val;
	} else if (la->kind == 3 || la->kind == 5) {
		if (la->kind == 3) {
			Get();
			name = t->val;
		} else {
			Get();
			name = t->val;
			// change surrounding single quotes to double quotes (does not escape '"')
			name[0] = name[name.size()-1] = '"';

		}
		kind = isLiteral;
		if (dfa->ignoreCase) {
		  for
		  (
		      std::wstring::iterator iter = name.begin();
		      iter != name.end();
		      ++iter
		  )
		  {
		    if (*iter >= 'A' && *iter <= 'Z') {
		      *iter += ('a' - 'A'); // ToLower()
		    }
		  }
		}
		if (name.find_first_of(L"\t\r\n ") != std::wstring::npos)
		  SemErr(L"literal tokens must not contain blanks");

	} else SynErr(55);
}


void Parser::Term(Graph* &g)
{
	Graph *g2; Node *rslv = NULL; g = NULL;
	if (StartOf(22)) {
		if (la->kind == 45) {
			rslv = tab->NewNode(Node::rslv, NULL, la->line);
			Resolver(rslv->pos);
			g = new Graph(rslv);
		}
		Factor(g2);
		if (rslv != NULL) tab->MakeSequence(g, g2);
		else g = g2;
		while (StartOf(23)) {
			Factor(g2);
			tab->MakeSequence(g, g2);
		}
	} else if (StartOf(24)) {
		g = new Graph(tab->NewNode(Node::eps));
	} else SynErr(56);
	if (g == NULL) // invalid start of Term
	 g = new Graph(tab->NewNode(Node::eps));
}


void Parser::Resolver(Position* &pos)
{
	Expect(45);
	Expect(38);
	int beg = la->pos; int col = la->col; int line = la->line;
	Condition();
	pos = new Position(beg, t->pos, col, line);
}


void Parser::Factor(Graph* &g)
{
	std::wstring name; int kind; Position *pos; bool weak = false;
	g = NULL;

	switch (la->kind) {
	case 1: case 3: case 5: case 37: {
		if (la->kind == 37) {
			Get();
			weak = true;
		}
		Sym(name, kind);
		Symbol *sym = tab->FindSym(name);
		if (sym == NULL && kind == isLiteral)
		  sym = tab->literals[name];
		bool undef = (sym == NULL);
		if (undef) {
		  if (kind == isIdent)
		    sym = tab->NewSym(Node::nt, name, 0);  // forward nt
		  else if (genScanner) {
		    sym = tab->NewSym(Node::t, name, t->line);
		    dfa->MatchLiteral(sym->name, sym);
		  } else {  // undefined string in production
		    SemErr(L"undefined string in production");
		    sym = tab->eofSy;  // dummy
		  }
		}
		Node::nodeType typ = sym->typ;
		if (typ != Node::t && typ != Node::nt)
		  SemErr(L"this symbol kind is not allowed in a production");
		if (weak) {
		  if (typ == Node::t) typ = Node::wt;
		  else SemErr(L"only terminals may be weak");
		}
		Node *p = tab->NewNode(typ, sym, t->line);
		g = new Graph(p);

		if (la->kind == 32 || la->kind == 34) {
			Attribs(p);
			if (kind == isLiteral) SemErr(L"a literal must not have attributes");
		}
		if (undef)
		 sym->attrPos = p->pos;  // dummy
		else if ((p->pos == NULL) != (sym->attrPos == NULL))
		  SemErr(L"attribute mismatch between declaration and use of this symbol");

		break;
	}
	case 38: {
		Get();
		Expression(g);
		Expect(39);
		break;
	}
	case 40: {
		Get();
		Expression(g);
		Expect(41);
		tab->MakeOption(g);
		break;
	}
	case 42: {
		Get();
		Expression(g);
		Expect(43);
		tab->MakeIteration(g);
		break;
	}
	case 47: {
		SemText(pos);
		Node *p = tab->NewNode(Node::sem);
		p->pos = pos;
		g = new Graph(p);

		break;
	}
	case 31: {
		Get();
		Node *p = tab->NewNode(Node::any);  // p.set is set in tab->SetupAnys
		g = new Graph(p);

		break;
	}
	case 44: {
		Get();
		Node *p = tab->NewNode(Node::sync);
		g = new Graph(p);

		break;
	}
	default: SynErr(57); break;
	}
	if (g == NULL) // invalid start of Factor
	 g = new Graph(tab->NewNode(Node::eps));

}


void Parser::Attribs(Node *p)
{
	if (la->kind == 32) {
		Get();
		int beg = la->pos; int col = la->col; int line = la->line;
		while (StartOf(14)) {
			if (StartOf(15)) {
				Get();
			} else {
				Get();
				SemErr(L"bad string in attributes");
			}
		}
		Expect(33);
		if (t->pos > beg) p->pos = new Position(beg, t->pos, col, line);
	} else if (la->kind == 34) {
		Get();
		int beg = la->pos; int col = la->col; int line = la->line;
		while (StartOf(16)) {
			if (StartOf(17)) {
				Get();
			} else {
				Get();
				SemErr(L"bad string in attributes");
			}
		}
		Expect(35);
		if (t->pos > beg) p->pos = new Position(beg, t->pos, col, line);
	} else SynErr(58);
}


void Parser::Condition()
{
	while (StartOf(25)) {
		if (la->kind == 38) {
			Get();
			Condition();
		} else {
			Get();
		}
	}
	Expect(39);
}


void Parser::TokenTerm(Graph* &g)
{
	Graph *g2;
	TokenFactor(g);
	while (StartOf(13)) {
		TokenFactor(g2);
		tab->MakeSequence(g, g2);
	}
	if (la->kind == 46) {
		Get();
		Expect(38);
		TokenExpr(g2);
		tab->SetContextTrans(g2->l);
		dfa->hasCtxMoves = true;
		tab->MakeSequence(g, g2);
		Expect(39);
	}
}


void Parser::TokenFactor(Graph* &g)
{
	std::wstring name; int kind; g = NULL;
	if (la->kind == 1 || la->kind == 3 || la->kind == 5) {
		Sym(name, kind);
		if (kind == isIdent) {
		 CharClass *c = tab->FindCharClass(name);
		 if (c == NULL) {
		   SemErr(L"undefined name");
		   c = tab->NewCharClass(name, new CharSet());
		 }
		 Node *p = tab->NewNode(Node::clas); p->val = c->n;
		 g = new Graph(p);
		 tokenString = noString;
		} else { // str
		  g = tab->StrToGraph(name);
		  if (tokenString.empty()) tokenString = name;
		  else tokenString = noString;
		}

	} else if (la->kind == 38) {
		Get();
		TokenExpr(g);
		Expect(39);
	} else if (la->kind == 40) {
		Get();
		TokenExpr(g);
		Expect(41);
		tab->MakeOption(g);
	} else if (la->kind == 42) {
		Get();
		TokenExpr(g);
		Expect(43);
		tab->MakeIteration(g);
	} else SynErr(59);
	if (g == NULL) // invalid start of TokenFactor
	 g = new Graph(tab->NewNode(Node::eps));
}



void Parser::Parse()
{
	t = NULL;
	// might call Parse() twice
	if (dummyToken) {
		coco_string_delete(dummyToken->val);
		delete dummyToken;
	}
	dummyToken = new Token(coco_string_create(L"Dummy Token"));
	la = dummyToken;
	Get();
	Coco();
	Expect(0); // expect end-of-file automatically added
}


Parser::Parser(Scanner* scan, Errors* err)
:
	dummyToken(NULL),
	deleteErrorsDestruct_(!err),
	errDist(minErrDist),
	scanner(scan),
	errors(err),
	t(NULL),
	la(NULL)
{
	if (!errors)    // add in default error handling
	{
		errors = new Errors();
	}
	// user-defined initializations:
genScanner = false;
	tab = NULL;
	dfa = NULL;
	pgen = NULL;
}


bool Parser::StartOf(int s)
{
	const bool T = true;
	const bool x = false;

	static const bool set[26][51] =
	{
		{T,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x},
		{x,T,T,T, T,T,T,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,x,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,x,T,x, T,x,T,x, x,x,x,x, T,T,T,x, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{T,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,T, T,T,T,x, x,x,x,T, x,x,x,x, T,T,T,x, T,x,T,x, T,T,x,T, x,x,x},
		{T,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,T, T,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x},
		{T,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x},
		{x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,T,T, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,T, x,x,x,x, x,x,x},
		{x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, T,x,T,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,T,x},
		{x,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,T, x,x,x,x, x,x,x},
		{x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,T, x,x,x,x, T,T,T,T, T,T,T,T, T,T,x,T, x,x,x},
		{x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,T,T,x, T,x,T,x, T,T,x,T, x,x,x},
		{x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,T,T,x, T,x,T,x, T,x,x,T, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x,T, x,T,x,T, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, T,T,T,T, T,T,T,T, T,T,x}
	};

	return set[s][la->kind];
}


Parser::~Parser()
{
	if (deleteErrorsDestruct_) { delete errors; } // delete default error handling
	if (dummyToken) {
		coco_string_delete(dummyToken->val);
		delete dummyToken;
	}
	// user-defined destruction:
}


// ----------------------------------------------------------------------------
// Errors Implementation
// ----------------------------------------------------------------------------

Errors::Errors()
:
	count(0)
{}


Errors::~Errors()
{}


void Errors::clear()
{
	count = 0;
}


std::wstring Errors::strerror(int n)
{
	switch (n) {
		case 0: return L"EOF expected"; break;
		case 1: return L"ident expected"; break;
		case 2: return L"number expected"; break;
		case 3: return L"string expected"; break;
		case 4: return L"badString expected"; break;
		case 5: return L"char expected"; break;
		case 6: return L"\"[copy]\" expected"; break;
		case 7: return L"\"[/copy]\" expected"; break;
		case 8: return L"\"COMPILER\" expected"; break;
		case 9: return L"\"[initialize]\" expected"; break;
		case 10: return L"\"[/initialize]\" expected"; break;
		case 11: return L"\"[destroy]\" expected"; break;
		case 12: return L"\"[/destroy]\" expected"; break;
		case 13: return L"\"[code]\" expected"; break;
		case 14: return L"\"[/code]\" expected"; break;
		case 15: return L"\"IGNORECASE\" expected"; break;
		case 16: return L"\"CHARACTERS\" expected"; break;
		case 17: return L"\"TOKENS\" expected"; break;
		case 18: return L"\"PRAGMAS\" expected"; break;
		case 19: return L"\"COMMENTS\" expected"; break;
		case 20: return L"\"FROM\" expected"; break;
		case 21: return L"\"TO\" expected"; break;
		case 22: return L"\"NESTED\" expected"; break;
		case 23: return L"\"IGNORE\" expected"; break;
		case 24: return L"\"PRODUCTIONS\" expected"; break;
		case 25: return L"\"=\" expected"; break;
		case 26: return L"\".\" expected"; break;
		case 27: return L"\"END\" expected"; break;
		case 28: return L"\"+\" expected"; break;
		case 29: return L"\"-\" expected"; break;
		case 30: return L"\"..\" expected"; break;
		case 31: return L"\"ANY\" expected"; break;
		case 32: return L"\"<\" expected"; break;
		case 33: return L"\">\" expected"; break;
		case 34: return L"\"<.\" expected"; break;
		case 35: return L"\".>\" expected"; break;
		case 36: return L"\"|\" expected"; break;
		case 37: return L"\"WEAK\" expected"; break;
		case 38: return L"\"(\" expected"; break;
		case 39: return L"\")\" expected"; break;
		case 40: return L"\"[\" expected"; break;
		case 41: return L"\"]\" expected"; break;
		case 42: return L"\"{\" expected"; break;
		case 43: return L"\"}\" expected"; break;
		case 44: return L"\"SYNC\" expected"; break;
		case 45: return L"\"IF\" expected"; break;
		case 46: return L"\"CONTEXT\" expected"; break;
		case 47: return L"\"(.\" expected"; break;
		case 48: return L"\".)\" expected"; break;
		case 49: return L"??? expected"; break;
		case 50: return L"this symbol not expected in Coco"; break;
		case 51: return L"this symbol not expected in TokenDecl"; break;
		case 52: return L"invalid TokenDecl"; break;
		case 53: return L"invalid AttrDecl"; break;
		case 54: return L"invalid SimSet"; break;
		case 55: return L"invalid Sym"; break;
		case 56: return L"invalid Term"; break;
		case 57: return L"invalid Factor"; break;
		case 58: return L"invalid Attribs"; break;
		case 59: return L"invalid TokenFactor"; break;
		default:
		{
			// std::wostringstream buf;  (this typedef might be missing)
			std::basic_ostringstream<wchar_t> buf;
			buf << "error " << n;
			return buf.str();
		}
		break;
	}
}


void Errors::Warning(const std::wstring& msg)
{
	wprintf(L"%ls\n", msg.c_str());
}


void Errors::Warning(int line, int col, const std::wstring& msg)
{
	wprintf(L"-- line %d col %d: %ls\n", line, col, msg.c_str());
}


void Errors::Error(int line, int col, const std::wstring& msg)
{
	wprintf(L"-- line %d col %d: %ls\n", line, col, msg.c_str());
	count++;
}


void Errors::SynErr(int line, int col, int n)
{
	this->Error(line, col, this->strerror(n));
}


void Errors::Exception(const std::wstring& msg)
{
	wprintf(L"%ls", msg.c_str());
	::exit(1);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace

// ************************************************************************* //
