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

#ifndef COCO_STATE_H__
#define COCO_STATE_H__

namespace Coco
{

// forward declarations
class Action;
class Symbol;

/*---------------------------------------------------------------------------*\
                            Class State Declaration
\*---------------------------------------------------------------------------*/

//! state of finite automaton
class State
{
public:
	int nr;                 //!< state number
	Action *firstAction;    //!< to first action of this state
	Symbol *endOf;          //!< recognized token if state is final
	bool ctx;               //!< true if state is reached via contextTrans

	State *next;

	//! Construct null
	State();

	void AddAction(Action *);
	void DetachAction(Action *);
	void MeltWith(State *);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
