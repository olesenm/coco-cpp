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

#ifndef COCO_CHARSET_H__
#define COCO_CHARSET_H__

namespace Coco
{

/*---------------------------------------------------------------------------*\
                           Class CharSet Declaration
\*---------------------------------------------------------------------------*/

//! A set containing ranges of characters
class CharSet
{
	// Private Member Functions

		//! Disallow default bitwise assignment
		void operator=(const CharSet&);

public:
	//! A character range within a character set
	class Range
	{
	public:
		int from;         //<! The start character in range
		int to;           //<! The end character in range
		Range *next;      //<! Linked list element

		Range(int fromCh, int toCh)
		:
			from(fromCh),
			to(toCh),
			next(0)
		{}
	};

	//! Linked list of character ranges
	Range *head;


	// Constructors

		//! Construct null
		CharSet();

		//! Construct by copying
		CharSet(const CharSet&);

	//! Destructor
	virtual ~CharSet();

	// Member Functions

		//! Remove all ranges
		void Clear();

		//! Replace existing ranges with a range from 0 to Buffer::MaxChar
		void Fill();

		//! Return true if the index I is within a range
		bool Get(int i) const;

		//! Set the index I to be included in a range
		void Set(int i);

		//! Return the number of elements (characters) addressed by the ranges
		int Elements() const;

		//! Return the first character in the set
		int First() const;

		//! Return true if ranges are identical
		bool Equals(const CharSet& b) const;

		//! Logical 'and' with the ranges in b
		void And(const CharSet& b);

		//! Logical 'or' with the ranges in b
		void Or(const CharSet& b);

		//! With ranges in b removed
		void Subtract(const CharSet& b);

		//! Return true if 'b' is contained with the ranges
		bool Includes(const CharSet& b) const;

		//! Return true if 'b' intersects with the ranges
		bool Intersects(const CharSet& b) const;

	// Member Functions with pointers

		//! Construct as a copy
		inline CharSet* Clone() const
		{
			return new CharSet(*this);
		}

		//! Return true if ranges are identical
		inline bool Equals(CharSet *b) const
		{
			return this->Equals(*b);
		}

		//! Logical 'and' with the ranges in b
		inline void And(const CharSet *b)
		{
			this->And(*b);
		}

		//! Logical 'or' with the ranges in b
		inline void Or(const CharSet *b)
		{
			this->Or(*b);
		}

		//! With ranges in b removed
		inline void Subtract(const CharSet *b)
		{
			this->Subtract(*b);
		}

		//! Return true if 'b' is contained with the ranges
		inline bool Includes(const CharSet *b) const
		{
			return this->Includes(*b);
		}

		//! Return true if 'b' intersects with the ranges
		inline bool Intersects(const CharSet *b) const
		{
			return this->Intersects(*b);
		}

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
