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

#ifndef COCO_POSITION_H__
#define COCO_POSITION_H__

namespace Coco
{

/*---------------------------------------------------------------------------*\
                          Class Position Declaration
\*---------------------------------------------------------------------------*/

//! position of source code stretch (e.g. semantic action, resolver expressions)
class Position
{
public:
	int beg;     //!< start relative to the beginning of the file
	int end;     //!< end of stretch
	int col;     //!< column number of start position
	int line;    //!< line number of beginning of source code stretch

	//! Construct null
	Position()
	:
		beg(0),
		end(0),
		col(0),
		line(0)
	{}

	//! Construct from components
	Position(int begPos, int endPos, int colNr, int lineNr)
	:
		beg(begPos),
		end(endPos),
		col(colNr),
		line(lineNr)
	{}

	//! Return true if Position is empty
	bool empty() const
	{
		return end <= beg;
	}

	//! Return the size of the region, zero for invalid regions
	int size() const
	{
		return end <= beg ? 0 : (end - beg);
	}

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
