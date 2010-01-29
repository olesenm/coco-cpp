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

#ifndef COCO_ACTION_H__
#define COCO_ACTION_H__

#include "Node.h"

namespace Coco
{

// forward declarations
class CharSet;
class Tab;
class Target;

/*---------------------------------------------------------------------------*\
                           Class Action Declaration
\*---------------------------------------------------------------------------*/

//! action of finite automaton
class Action
{
public:
	Node::nodeType typ;   //!< type of action symbol: clas, chr
	int sym;              //!< action symbol
	Node::transitionType tc;  //!< transition code: normalTrans, contextTrans

	Target *target;       //!< states reached from this action
	Action *next;         //!< the next Action


	Action(Node::nodeType typ, int symNr, Node::transitionType tc);

	void AddTarget(Target *t);  //!< add t to the action.targets
	void AddTargets(Action *a); //!< add copy of a.targets to action.targets

	CharSet* Symbols(Tab *tab) const;
	void ShiftWith(CharSet *s, Tab *tab);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
