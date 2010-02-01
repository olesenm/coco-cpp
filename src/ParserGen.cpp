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

#include <ctype.h>

#include "ArrayList.h"
#include "ParserGen.h"
#include "Parser.h"
#include "BitArray.h"
#include "Utils.h"

namespace Coco
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void ParserGen::Indent(int n)
{
	for (int i=0; i < n; ++i) fwprintf(gen, L"\t");
}


// AW: use a switch if more than 5 alternatives and none starts with a resolver
bool ParserGen::UseSwitch(Node *p)
{
	if (p->typ != Node::alt) return false;
	int nAlts = 0;
	while (p != NULL)
	{
		++nAlts;
		// must not optimize with switch-statement, if alt uses a resolver expression
		if (p->sub->typ == Node::rslv) return false;
		p = p->down;
	}
	return nAlts > 5;
}


void ParserGen::CopyFramePart(const std::string& stop, const bool doOutput)
{
	bool ok = tab->CopyFramePart(gen, fram, stop, doOutput);
	if (!ok)
	{
		errors->Exception(L" -- incomplete or corrupt parser frame file");
	}
}


void ParserGen::CopySourcePart(Position *pos, int indent)
{
	// Copy text described by pos from atg to gen
	tab->CopySourcePart(gen, pos, indent);
}


void ParserGen::GenErrorMsg(ParserGen::errorType errTyp, Symbol *sym)
{
	errorNr++;
	wchar_t format[32];
	coco_swprintf(format, 32, L"\t\tcase %d:", errorNr);
	err += format;
	err += L" return L\"";
	if (errTyp == tErr)
	{
		if (sym->name[0] == '"')
		{
			err += Tab::Escape(sym->name);
		}
		else
		{
			err += sym->name;
		}
		err += L" expected";
	}
	else if (errTyp == altErr)
	{
		err += L"invalid ";
		err += sym->name;
	}
	else if (errTyp == syncErr)
	{
		err += L"this symbol not expected in ";
		err += sym->name;
	}
	err += L"\"; break;\n";
}


int ParserGen::NewCondSet(BitArray *s)
{
	for (int i = 1; i < symSet.Count; i++) // skip symSet[0] (reserved for union of SYNC sets)
		if (Sets::Equals(s, symSet[i])) return i;
	symSet.Add(s->Clone());
	return symSet.Count-1;
}


void ParserGen::GenCond(BitArray *s, Node *p)
{
	if (p->typ == Node::rslv) CopySourcePart(p->pos, 0);
	else {
		int n = Sets::Elements(s);
		if (n == 0) fwprintf(gen, L"false"); // happens if an ANY set matches no symbol
		else if (n <= maxTerm)
		{
			for (int i=0; i < tab->terminals.Count; i++)
			{
				Symbol *sym = tab->terminals[i];
				if ((*s)[sym->n])
				{
					fwprintf(gen, L"la->kind == %d", sym->n);
					--n;
					if (n > 0) fwprintf(gen, L" || ");
				}
			}
		}
		else
		{
			fwprintf(gen, L"StartOf(%d)", NewCondSet(s));
		}
	}
}


void ParserGen::PutCaseLabels(BitArray *s)
{
	for (int i=0; i < tab->terminals.Count; i++)
	{
		Symbol *sym = tab->terminals[i];
		if ((*s)[sym->n]) fwprintf(gen, L"case %d: ", sym->n);
	}
}


void ParserGen::GenCode(Node *p, int indent, BitArray *isChecked)
{
	Node *p2;
	BitArray *s1, *s2;
	while (p != NULL)
	{
		if (p->typ == Node::nt)
		{
			Indent(indent);
			fwprintf(gen, L"%ls(", p->sym->name.c_str());
			CopySourcePart(p->pos, 0);
			fwprintf(gen, L");\n");
		}
		else if (p->typ == Node::t)
		{
			Indent(indent);
			// assert: if isChecked[p->sym->n] is true, then isChecked contains only p->sym->n
			if ((*isChecked)[p->sym->n]) fwprintf(gen, L"Get();\n");
			else fwprintf(gen, L"Expect(%d);\n", p->sym->n);
		}
		if (p->typ == Node::wt)
		{
			Indent(indent);
			s1 = tab->Expected(p->next, curSy);
			s1->Or(tab->allSyncSets);
			fwprintf(gen, L"ExpectWeak(%d, %d);\n", p->sym->n, NewCondSet(s1));
		}
		if (p->typ == Node::any)
		{
			Indent(indent);
			int acc = Sets::Elements(p->set);
			if
			(
			    tab->terminals.Count == (acc + 1)
			 || (acc > 0 && Sets::Equals(p->set, isChecked))
			)
			{
				// either this ANY accepts any terminal (the + 1 = end of file), or exactly what's allowed here
				fwprintf(gen, L"Get();\n");
			}
			else
			{
				GenErrorMsg(altErr, curSy);
				if (acc > 0)
				{
					fwprintf(gen, L"if ("); GenCond(p->set, p); fwprintf(gen, L") Get(); else SynErr(%d);\n", errorNr);
				}
				else
				{
					fwprintf(gen, L"SynErr(%d); // ANY node that matches no symbol\n", errorNr);
				}
			}
		}

		if (p->typ == Node::eps)     // nothing
		{}
		if (p->typ == Node::rslv)   // nothing
		{}
		if (p->typ == Node::sem)
		{
			CopySourcePart(p->pos, indent);
		}
		if (p->typ == Node::sync)
		{
			Indent(indent);
			GenErrorMsg(syncErr, curSy);
			s1 = p->set->Clone();
			fwprintf(gen, L"while (!("); GenCond(s1, p); fwprintf(gen, L")) {");
			fwprintf(gen, L"SynErr(%d); Get();", errorNr); fwprintf(gen, L"}\n");
		}
		if (p->typ == Node::alt)
		{
			s1 = tab->First(p);
			bool equal = Sets::Equals(s1, isChecked);
			bool useSwitch = UseSwitch(p);
			if (useSwitch)
			{
				Indent(indent); fwprintf(gen, L"switch (la->kind) {\n");
			}
			p2 = p;
			while (p2 != NULL)
			{
				s1 = tab->Expected(p2->sub, curSy);
				Indent(indent);
				if (useSwitch)
				{
					PutCaseLabels(s1); fwprintf(gen, L"{\n");
				}
				else if (p2 == p)
				{
					fwprintf(gen, L"if ("); GenCond(s1, p2->sub); fwprintf(gen, L") {\n");
				}
				else if (p2->down == NULL && equal)
				{
					fwprintf(gen, L"} else {\n");
				}
				else
				{
					fwprintf(gen, L"} else if (");  GenCond(s1, p2->sub); fwprintf(gen, L") {\n");
				}
				GenCode(p2->sub, indent + 1, s1);
				if (useSwitch)
				{
					Indent(indent); fwprintf(gen, L"\tbreak;\n");
					Indent(indent); fwprintf(gen, L"}\n");
				}
				p2 = p2->down;
			}
			Indent(indent);
			if (equal)
			{
				fwprintf(gen, L"}\n");
			}
			else
			{
				GenErrorMsg(altErr, curSy);
				if (useSwitch)
				{
					fwprintf(gen, L"default: SynErr(%d); break;\n", errorNr);
					Indent(indent); fwprintf(gen, L"}\n");
				}
				else
				{
					fwprintf(gen, L"} "); fwprintf(gen, L"else SynErr(%d);\n", errorNr);
				}
			}
		}
		if (p->typ == Node::iter)
		{
			Indent(indent);
			p2 = p->sub;
			fwprintf(gen, L"while (");
			if (p2->typ == Node::wt)
			{
				s1 = tab->Expected(p2->next, curSy);
				s2 = tab->Expected(p->next, curSy);
				fwprintf(gen, L"WeakSeparator(%d,%d,%d) ", p2->sym->n, NewCondSet(s1), NewCondSet(s2));
				s1 = new BitArray(tab->terminals.Count);  // for inner structure
				if (p2->up || p2->next == NULL) p2 = NULL; else p2 = p2->next;
			}
			else
			{
				s1 = tab->First(p2);
				GenCond(s1, p2);
			}
			fwprintf(gen, L") {\n");
			GenCode(p2, indent + 1, s1);
			Indent(indent); fwprintf(gen, L"}\n");
		}
		if (p->typ == Node::opt)
		{
			s1 = tab->First(p->sub);
			Indent(indent);
			fwprintf(gen, L"if ("); GenCond(s1, p->sub); fwprintf(gen, L") {\n");
			GenCode(p->sub, indent + 1, s1);
			Indent(indent); fwprintf(gen, L"}\n");
		}
		if (p->typ != Node::eps && p->typ != Node::sem && p->typ != Node::sync)
			isChecked->SetAll(false);  // = new BitArray(Symbol.terminals.Count);
		if (p->up) break;
		p = p->next;
	}
}


void ParserGen::GenTokensHeader()
{
	fwprintf(gen, L"\tenum {\n");

	// tokens
	for (int i=0; i < tab->terminals.Count; i++)
	{
		Symbol* sym = tab->terminals[i];
		if (!isalpha(sym->name[0]))
		{
			continue;
		}
		fwprintf(gen , L"\t\t_%ls=%d,\n", sym->name.c_str(), sym->n);
	}

	// pragmas
	for (int i=0; i < tab->pragmas.Count; i++)
	{
		Symbol* sym = tab->pragmas[i];
		fwprintf(gen , L"\t\t_%ls=%d,\n", sym->name.c_str(), sym->n);
	}

	fwprintf(gen, L"\t\tmaxT = %d    //<! max term (w/o pragmas)\n", tab->terminals.Count-1);
	fwprintf(gen, L"\t};\n");
}


void ParserGen::GenCodePragmas()
{
	for (int i=0; i < tab->pragmas.Count; i++)
	{
		Symbol* sym = tab->pragmas[i];
		fwprintf(gen, L"\t\tif (la->kind == %d) {\n", sym->n);
		CopySourcePart(sym->semPos, 4);
		fwprintf(gen, L"\t\t}\n");
	}
}


void ParserGen::GenProductionsHeader()
{
	for (int i=0; i < tab->nonterminals.Count; i++)
	{
		Symbol* sym = tab->nonterminals[i];
		curSy = sym;
		fwprintf(gen, L"\tvoid %ls(", sym->name.c_str());
		CopySourcePart(sym->attrPos, 0);
		fwprintf(gen, L");\n");
	}
}


void ParserGen::GenProductions()
{
	for (int i=0; i < tab->nonterminals.Count; i++)
	{
		Symbol* sym = tab->nonterminals[i];
		curSy = sym;
		fwprintf(gen, L"void Parser::%ls(", sym->name.c_str());
		CopySourcePart(sym->attrPos, 0);
		fwprintf(gen, L")\n{\n");
		CopySourcePart(sym->semPos, 1);
		GenCode(sym->graph, 1, new BitArray(tab->terminals.Count));
		fwprintf(gen, L"}\n\n\n");
	}
}


void ParserGen::InitSets()
{
	fwprintf(gen, L"\tstatic const bool set[%d][%d] =\n\t{\n", symSet.Count, tab->terminals.Count+1);

	for (int i=0; i < symSet.Count; i++)
	{
		BitArray *s = symSet[i];
		fwprintf(gen, L"\t\t{");
		int j = 0;
		for (int k=0; k<tab->terminals.Count; k++)
		{
			Symbol *sym = tab->terminals[k];
			if ((*s)[sym->n]) fwprintf(gen, L"T,"); else fwprintf(gen, L"x,");
			++j;
			if (j % 4 == 0) fwprintf(gen, L" ");
		}
		if (i == symSet.Count-1)
			fwprintf(gen, L"x}\n"); else fwprintf(gen, L"x},\n");
	}
	fwprintf(gen, L"\t};\n\n");
}


void ParserGen::WriteParser()
{
	tab->preproc_.reset();               // reset pre-processor logic
	int oldPos = tab->buffer->GetPos();  // Pos is modified by CopySourcePart
	symSet.Add(tab->allSyncSets);

	fram = tab->OpenFrameFile("Parser.frame");
	if (fram == NULL)
	{
		errors->Exception(L"-- Cannot open Parser frame.\n");
	}

	//
	// Header
	//
	gen = tab->OpenGenFile("Parser.h");
	if (gen == NULL)
	{
		errors->Exception(L"-- Cannot generate Parser header\n");
	}

	for (int i=0; i < tab->terminals.Count; i++)
	{
		Symbol *sym = tab->terminals[i];
		GenErrorMsg(tErr, sym);
	}

	CopyFramePart("-->begin", false);                  // no output
	tab->CopySourcePart(gen, tab->copyPos, 0, false);  // copy without emitLines
	tab->AddNotice(gen);
	CopyFramePart("-->headerdef");

	if (preamblePos != NULL)
	{
		CopySourcePart(preamblePos, 0); fwprintf(gen, L"\n");
	}
	CopyFramePart("-->namespace_open");
	int nrOfNs = tab->GenNamespaceOpen(gen);

	CopyFramePart("-->constantsheader");
	GenTokensHeader();

	CopyFramePart("-->declarations");
	CopySourcePart(semDeclPos, 0);

	CopyFramePart("-->productionsheader");
	GenProductionsHeader();

	CopyFramePart("-->namespace_close");
	tab->GenNamespaceClose(gen, nrOfNs);

	CopyFramePart("-->implementation");
	fclose(gen);

	//
	// Source
	//
	gen = tab->OpenGenFile("Parser.cpp");
	if (gen == NULL)
	{
		errors->Exception(L"-- Cannot generate Parser source\n");
	}

	tab->CopySourcePart(gen, tab->copyPos, 0, false);  // copy without emitLines
	tab->AddNotice(gen);

	CopyFramePart("-->namespace_open");
	if (Tab::singleOutput)
	{
		const std::string scannerCpp = Tab::prefixName + "Scanner.cpp";
		fwprintf
		(
			gen,
			L"\n// quick hack to provide a single Make target:\n#include \"%s\"\n\n",
			scannerCpp.c_str()
		);
	}
	nrOfNs = tab->GenNamespaceOpen(gen);

	// insert extra user-defined code
	if (extraCodePos != NULL)
	{
		fwprintf(gen, L"\n\n");
		CopySourcePart(extraCodePos, 0);
		fwprintf(gen, L"\n");
	}

	CopyFramePart("-->pragmas");
	GenCodePragmas();

	CopyFramePart("-->productions");
	GenProductions();

	CopyFramePart("-->parseRoot");
	fwprintf(gen, L"\t%ls();\n", tab->gramSy->name.c_str());

	CopyFramePart("-->constructor");
	CopySourcePart(initCodePos, 0);

	CopyFramePart("-->initialization");
	InitSets();
	CopyFramePart("-->destructor");
	CopySourcePart(deinitCodePos, 0);

	CopyFramePart("-->errors");
	fwprintf(gen, L"%ls", err.c_str());

	CopyFramePart("-->namespace_close");
	tab->GenNamespaceClose(gen, nrOfNs);

	CopyFramePart("$$$");
	fclose(gen);
	tab->buffer->SetPos(oldPos);    // restore Pos
}


void ParserGen::PrintStatistics() const
{
	FILE* trace = Tab::trace;

	fwprintf(trace, L"%d sets\n", symSet.Count);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

ParserGen::ParserGen(Parser *parser)
:
	errorNr(-1),
	curSy(NULL),
	fram(NULL),
	gen(NULL),
	tab(parser->tab),
	errors(parser->errors),
	preamblePos(NULL),
	semDeclPos(NULL),
	initCodePos(NULL),
	deinitCodePos(NULL),
	extraCodePos(NULL)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

ParserGen::~ParserGen()
{
	if (preamblePos) { delete preamblePos; }
	if (semDeclPos) { delete semDeclPos; }
	if (initCodePos) { delete initCodePos; }
	if (deinitCodePos) { delete deinitCodePos; }
	if (extraCodePos) { delete extraCodePos; }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// ************************************************************************* //
