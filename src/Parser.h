/*-------------------------------*- C++ -*---------------------------------
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



#ifndef COCO_PARSER_H__
#define COCO_PARSER_H__

#include "Tab.h"
#include "DFA.h"
#include "ParserGen.h"


#include "Scanner.h"

namespace Coco {


//! Parser error handing
class Errors {
public:
	int count;      //!< The number of errors detected

	//! Allocate and return a string describing the given error code.
	/** It is the responsibility of the caller to free this string,
	 *  eg, with coco_string_delete()
	 */
	static wchar_t* strerror(int n);

	Errors();               //!< Construct null - start with no errors
	virtual ~Errors();      //!< Destructor
	virtual void clear();   //!< Clear the error count

	//! Handle a general warning 'msg'
	virtual void Warning(const wchar_t* msg);
	//! Handle a general warning 'msg'
	virtual void Warning(int line, int col, const wchar_t* msg);
	//! Handle general error 'msg' (eg, a semantic error)
	virtual void Error(int line, int col, const wchar_t* msg);
	//! Handle syntax error 'n', uses strerror for the message, calls Error()
	virtual void SynErr(int line, int col, int n);
	//! Handle a general exception 'msg'
	virtual void Exception(const wchar_t* msg);

}; // Errors


//! A Coco/R Parser
class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_string=3,
		_badString=4,
		_char=5,
		_ddtSym=44,
		_directive=45,
	};
	static const int maxT = 43;

	static const int minErrDist = 2; //!< min. distance before reporting errors

	Token *dummyToken;
	bool deleteErrorsDestruct_; //!< delete the 'errors' member in destructor
	int  errDist;

	void SynErr(int n);         //!< Handle syntax error 'n'
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;                   //!< last recognized token
	Token *la;                  //!< lookahead token

private:
	static const int id = 0;
	static const int str = 1;
	bool genScanner;
	wchar_t* tokenString;   //!< used in declarations of literal tokens
	wchar_t* noString;      //!< used in declarations of literal tokens

public:
	FILE* trace;            //!< trace file
	Tab *tab;               //!< other Coco objects referenced in this ATG
	DFA *dfa;
	ParserGen *pgen;

/*-------------------------------------------------------------------------*/



	//! Construct for the specified scanner
	/**
	 *  Use the default error handling, or optionally provide an error
	 *  handler, which will not be deleted upon destruction.
	 */
	Parser(Scanner* scan, Errors* err = 0);
	~Parser();
	void SemErr(const wchar_t* msg);    //!< Handle semantic error

	void Coco();
	void SetDecl();
	void TokenDecl(int typ);
	void TokenExpr(Graph* &g);
	void Set(CharSet* &s);
	void AttrDecl(Symbol *sym);
	void SemText(Position* &pos);
	void Expression(Graph* &g);
	void SimSet(CharSet* &s);
	void Char(int &n);
	void Sym(wchar_t* &name, int &kind);
	void Term(Graph* &g);
	void Resolver(Position* &pos);
	void Factor(Graph* &g);
	void Attribs(Node *p);
	void Condition();
	void TokenTerm(Graph* &g);
	void TokenFactor(Graph* &g);

	void Parse();                       //!< Execute the parse operation

}; // end Parser

} // namespace


#endif // COCO_PARSER_H__
