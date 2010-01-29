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

#include <stdlib.h>

#include "CharSet.h"
#include "Scanner.h"

namespace Coco
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

bool CharSet::Get(int i) const
{
	for (Range *p = head; p != NULL; p = p->next)
	{
		// p.from <= i <= p.to
		if (i < p->from)
		{
			return false;
		}
		else if (i <= p->to)
		{
			return true;
		}
	}
	return false;
}


void CharSet::Set(int i)
{
	Range *cur = head, *prev = NULL;
	while (cur != NULL && i >= cur->from-1)
	{
		// (cur.from-1) <= i <= (cur.to+1)
		if (i <= cur->to + 1)
		{
			if (i == cur->from - 1)
			{
				cur->from--;
			}
			else if (i == cur->to + 1)
			{
				cur->to++;
				Range *next = cur->next;
				if (next != NULL && cur->to == next->from - 1)
				{
					cur->to = next->to;
					cur->next = next->next;
					delete next;
				};
			}
			return;
		}
		prev = cur; cur = cur->next;
	}
	Range *n = new Range(i, i);
	n->next = cur;
	if (prev == NULL) head = n; else prev->next = n;
}


CharSet* CharSet::Clone() const
{
	CharSet *s = new CharSet();
	Range *prev = NULL;
	for (Range *cur = head; cur != NULL; cur = cur->next)
	{
		Range *r = new Range(cur->from, cur->to);
		if (prev == NULL)
		{
			s->head = r;
		}
		else
		{
			prev->next = r;
		}
		prev = r;
	}
	return s;
}


bool CharSet::Equals(CharSet *s) const
{
	Range *p = head, *q = s->head;
	while (p != NULL && q != NULL)
	{
		if (p->from != q->from || p->to != q->to)
		{
			return false;
		}
		p = p->next; q = q->next;
	}
	return p == q;
}


int CharSet::Elements() const
{
	int n = 0;
	for (Range *p = head; p != NULL; p = p->next)
	{
		n += p->to - p->from + 1;
	}
	return n;
}


int CharSet::First() const
{
	if (head != NULL) return head->from;
	return -1;
}


void CharSet::Or(CharSet *s)
{
	for (Range *p = s->head; p != NULL; p = p->next)
	{
		for (int i = p->from; i <= p->to; i++)
		{
			Set(i);
		}
	}
}


void CharSet::And(CharSet *s)
{
	CharSet *x = new CharSet();
	Range *p = head;
	while (p != NULL)
	{
		for (int i = p->from; i <= p->to; i++)
		{
			if (s->Get(i))
			{
				x->Set(i);
			}
		}
		Range *del = p;
		p = p->next;
		delete del;
	}
	head = x->head;
	x->head = NULL;
	delete x;
}


void CharSet::Subtract(CharSet *s)
{
	CharSet *x = new CharSet();
	Range *p = head;
	while (p != NULL)
	{
		for (int i = p->from; i <= p->to; i++)
		{
			if (!s->Get(i))
			{
				x->Set(i);
			}
		}
		Range *del = p;
		p = p->next;
		delete del;
	}
	head = x->head;
	x->head = NULL;
	delete x;
}


bool CharSet::Includes(CharSet *s) const
{
	for (Range *p = s->head; p != NULL; p = p->next)
	{
		for (int i = p->from; i <= p->to; i++)
		{
			if (!Get(i))
			{
				return false;
			}
		}
	}
	return true;
}


bool CharSet::Intersects(CharSet *s) const
{
	for (Range *p = s->head; p != NULL; p = p->next)
	{
		for (int i = p->from; i <= p->to; i++)
		{
			if (Get(i))
			{
				return true;
			}
		}
	}
	return false;
}


void CharSet::Clear()
{
	while (head != NULL)
	{
		Range *del = head;
		head = head->next;
		delete del;
	}
}


void CharSet::Fill()
{
	Clear();
	head = new Range(0, Buffer::MaxChar);
}


CharSet::~CharSet()
{
	Clear();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// ************************************************************************* //
