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

#ifndef COCO_SORTEDLIST_H__
#define COCO_SORTEDLIST_H__

namespace Coco
{

// forward declarations
class Symbol;

/*---------------------------------------------------------------------------*\
	                     Class SortedList Declaration
\*---------------------------------------------------------------------------*/

//! Key/value pairs that are sorted by the keys, accessible by key and by index.
template<class Type>
class SortedList
{
	struct Entry
	{
		Symbol* Key;
		Type* Value;
		Entry* next;

		Entry(Symbol* k, Type* v);
	};

	static int Compare(Symbol* x, Symbol* y);
	bool Find(Symbol* key);

	Entry *Data_;

public:
	int Count;

	SortedList();
	virtual ~SortedList();

	void  Set(Symbol *key, Type *value);
	Type* Get(Symbol* key) const;        //!< return Value
	Symbol* GetKey(int index) const ;    //!< return Key
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SortedList.tcc"

#endif

// ************************************************************************* //
