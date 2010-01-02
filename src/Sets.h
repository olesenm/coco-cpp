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

#ifndef COCO_SETS_H__
#define COCO_SETS_H__

#include "BitArray.h"

namespace Coco {

/*---------------------------------------------------------------------------*\
                            Class Sets Declaration
\*---------------------------------------------------------------------------*/

class Sets {
public:
	static int First(BitArray *s) {
		int max = s->getCount();
		for (int i=0; i<max; i++)
			if ((*s)[i]) return i;
		return -1;
	}

	static int Elements(BitArray *s) {
		int max = s->getCount();
		int n = 0;
		for (int i=0; i<max; i++)
			if ((*s)[i]) n++;
		return n;
	}

	//! Check: (a == b)?
	static bool Equals(BitArray *a, BitArray *b) {
		int max = a->getCount();
		for (int i=0; i<max; i++)
			if ((*a)[i] != (*b)[i]) return false;
		return true;
	}

	//! Check: (a > b)?
	static bool Includes(BitArray *a, BitArray *b) {
		int max = a->getCount();
		for (int i=0; i<max; i++)
			if ((*b)[i] && ! (*a)[i]) return false;
		return true;
	}

	//! Check: (a * b != {})
	static bool Intersect(BitArray *a, BitArray *b) {
		int max = a->getCount();
		for (int i=0; i<max; i++)
			if ((*a)[i] && (*b)[i]) return true;
		return false;
	}

	//! Operation: a = a - b
	static void Subtract(BitArray *a, BitArray *b) {
		BitArray *c = b->Clone();
		c->Not();
		a->And(c);
		delete c;
	}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //