/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
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

        explicit AbstractUndoRedo(){}
        explicit AbstractUndoRedo(const State & initial_state);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ~AbstractUndoRedo() {}

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
