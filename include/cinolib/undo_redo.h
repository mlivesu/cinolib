/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#ifndef CINO_UNDO_REDO_H
#define CINO_UNDO_REDO_H

#include <stack>

namespace cinolib
{

/* This class implements a very basic UNDO/REDO paradigm for general objects.
 * It is an abstract class, hence cannot be used directly. To use it, create
 * a new class that inherits from AbstractUndoRedo and implement set_state()
*/

template<class Object, class State>
class AbstractUndoRedo
{
    public:

        explicit AbstractUndoRedo(const State & initial_state);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_state(const State & state); // pushes a state into the undo stack

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Implement this in your derived class. It should force Object "obj" to
        // assume the State "state" (whatever it means in your application)
        //
        virtual void set_state(const State & state, Object & obj) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool undo (Object & obj);
        bool redo (Object & obj);
        void reset(Object & obj);

    private:

        std::stack<State> undo_stack;
        std::stack<State> redo_stack;
};

}

#ifndef  CINO_STATIC_LIB
#include "undo_redo.cpp"
#endif

#endif // CINO_UNDO_REDO_H
