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

#ifndef COCO_ARRAYLIST_H__
#define COCO_ARRAYLIST_H__

namespace Coco
{

/*---------------------------------------------------------------------------*\
                          Class ArrayList Declaration
\*---------------------------------------------------------------------------*/

//! An array of pointers
template<class Type>
class ArrayList
{
	int capacity_;
	Type** data_;

public:
	int Count;

	ArrayList(int size=10);     //!< Construct with default capacity
	virtual ~ArrayList();

	void Add(Type *ptr);        //!< Add pointer to the list
	bool Remove(Type *ptr);     //!< Remove pointer from the list

	void Delete();              //!< Delete all pointers in the list
	void Clear();               //!< Reduce size to zero, does not delete pointers

	//! Return pointer corresponding to the index, return NULL for out-of-range
	Type* operator[](int index);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ArrayList.tcc"

#endif

// ************************************************************************* //
