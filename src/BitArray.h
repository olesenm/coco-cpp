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

#ifndef COCO_BITARRAY_H__
#define COCO_BITARRAY_H__

namespace Coco
{

/*---------------------------------------------------------------------------*\
                          Class BitArray Declaration
\*---------------------------------------------------------------------------*/

//! Compact storage of bits using byte-wise storage
class BitArray
{
public:
	BitArray(int length = 0, bool defaultValue = false);
	BitArray(const BitArray &copy);
	virtual ~BitArray();

	int getCount() const;

	bool Get(const int index) const;
	void Set(const int index, const bool value);
	void SetAll(const bool value);
	bool Equal(const BitArray *right) const;
	bool operator[](const int index) const { return Get(index); };

	const BitArray& operator=(const BitArray &right);

	void Not();
	void And(const BitArray *value);
	void Or(const BitArray *value);
	void Xor(const BitArray *value);

	BitArray* Clone() const;

private:
	int Count;
	unsigned char* Data;
	void setMem(int length, bool value);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
