/*---------------------------------------------------------------------------*\
    This file was originally part of OpenFOAM and released under the same
    terms as the license below.

    Copyright (C) 2008-2009 OpenCFD Ltd.

    Modified namespace for inclusion in Coco/R
-------------------------------------------------------------------------------
License
    This file is part of Compiler Generator Coco/R

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Coco::StaticAssertFailed

Description
    Macros and classes to provide static (compile-time) assertions.

    Ideas from various sources
    (http://www.ddj.com/cpp/184401547, http://www.boost.org)
\*---------------------------------------------------------------------------*/

#ifndef COCO_STATICASSERT_H__
#define COCO_STATICASSERT_H__

namespace Coco
{

//! @brief Forward declaration of StaticAssertionFailed.
//! Leave as an incomplete class so that sizeof(..) fails
template<bool Truth> class StaticAssertionFailed;

/*---------------------------------------------------------------------------*\
                    Class StaticAssertionFailed Declaration
\*---------------------------------------------------------------------------*/

//! Specialization for successful assertions
template<>
class StaticAssertionFailed<true>
{};


//! Helper class for handling static assertions
template<unsigned Test>
class StaticAssertionTest {};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Coco

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// internal use:
// ~~~~~~~~~~~~~
// paste together strings, even if an argument is itself a macro
#define StaticAssertMacro(X,Y)  StaticAssertMacro1(X,Y)
#define StaticAssertMacro1(X,Y) StaticAssertMacro2(X,Y)
#define StaticAssertMacro2(X,Y) X##Y

// external use:
// ~~~~~~~~~~~~~
/*!
 * @def StaticAssert(Test)
 * Assert that some test is true at compile-time
 */
#define StaticAssert(Test)                                                   \
    typedef ::Coco::StaticAssertionTest                                      \
    <                                                                        \
        sizeof( ::Coco::StaticAssertionFailed< ((Test) ? true : false) > )   \
    > StaticAssertMacro(StaticAssertionTest, __LINE__)

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
