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

#include <string.h>
#include "BitArray.h"

namespace Coco
{


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const BitArray BitArray::null(0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

BitArray::BitArray(const int length, const bool value)
:
	size_(length),
	data_(0)
{
	const int storedSize = (size_+7) >> 3;
	if (storedSize)
	{
		data_ = new unsigned char[storedSize];
		memset(data_, (value ? ~0 : 0), storedSize);
	}
}


BitArray::BitArray(const BitArray &copy)
:
	size_(copy.size_),
	data_(0)
{
	const int storedSize = (size_+7) >> 3;
	data_ = new unsigned char[storedSize];

	memcpy(data_, copy.data_, storedSize);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

BitArray::~BitArray()
{
	delete[] data_;
	data_ = 0;
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

int BitArray::size() const
{
	return size_;
}


void BitArray::reset(const int length, const bool value)
{
	// the new size
	const int storedSize = (length+7) >> 3;

	if (storedSize != (size_+7) >> 3)
	{
		if (data_)
		{
			delete[] data_;
		}
		data_ = new unsigned char[storedSize];
	}

	size_ = length;
	memset(data_, (value ? ~0 : 0), storedSize);
}


bool BitArray::Get(const int index) const
{
	return (data_[(index>>3)] & (1 << (index&7))) != 0;
}


void BitArray::Set(const int index, const bool value)
{
	if (value)
	{
		data_[(index>>3)] |= (1 << (index&7));
	}
	else
	{
		data_[(index>>3)] &= ~(1 << (index&7));
	}
}


void BitArray::SetAll(const bool value)
{
	memset(data_, (value ? ~0 : 0), (size_+7)>>3);
}


BitArray& BitArray::Not()
{
	const int storedSize = (size_+7) >> 3;
	for (int i=0; i < storedSize; ++i)
	{
		data_[i] ^= ~0;
	}
	return *this;   // enable cascaded operations
}

BitArray& BitArray::And(const BitArray& b)
{
	const int storedSize  = (size_+7) >> 3;
	const int storedSizeB = (b.size_+7) >> 3;

	for (int i=0; i < storedSize && i < storedSizeB; ++i)
	{
		data_[i] &= b.data_[i];
	}
	return *this;   // enable cascaded operations
}


BitArray& BitArray::Or(const BitArray& b)
{
	const int storedSize  = (size_+7) >> 3;
	const int storedSizeB = (b.size_+7) >> 3;

	for (int i=0; i < storedSize && i < storedSizeB; ++i)
	{
		data_[i] |= b.data_[i];
	}
	return *this;   // enable cascaded operations
}


BitArray& BitArray::Xor(const BitArray& b)
{
	const int storedSize  = (size_+7) >> 3;
	const int storedSizeB = (b.size_+7) >> 3;

	for (int i=0; i < storedSize && i < storedSizeB; ++i)
	{
		data_[i] ^= b.data_[i];
	}
	return *this;   // enable cascaded operations
}


bool BitArray::Equal(const BitArray& b) const
{
	if (size_ != b.size_)
	{
		return false;
	}

	// NOTE: could simply use byte-wise instead of bit-wise comparison
	for (int i = 0; i < size_; ++i)
	{
		if
		(
		    (data_[(i>>3)] & (1<<(i&7)))
		 != (b.data_[(i>>3)] & (1<<(i&7)))
		)
		{
			return false;
		}
	}
	return true;
}


// * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * * //

BitArray& BitArray::operator=(const BitArray& rhs)
{
	if (this != &rhs)            // avoid self assignment
	{
		delete[] data_;          // prevent memory leak
		size_ = rhs.size_;
		const int storedSize = (size_+7) >> 3;

		data_ = new unsigned char[storedSize];
		memcpy(data_, rhs.data_, storedSize);
	}
	return *this;   // enable cascaded operations
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// ************************************************************************* //
