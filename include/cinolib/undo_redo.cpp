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
#include <cinolib/undo_redo.h>
#include <cinolib/cino_inline.h>
#include <assert.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Object, class State>
CINO_INLINE
AbstractUndoRedo<Object,State>::AbstractUndoRedo(const State & initial_state)
{
    push_state(initial_state);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Object, class State>
CINO_INLINE
void AbstractUndoRedo<Object,State>::push_state(const State & state)
{
    undo_stack.push(state);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Object, class State>
CINO_INLINE
bool AbstractUndoRedo<Object,State>::undo(Object & obj)
{
    if(undo_stack.size()<2)
    {
        // the undo_stack should always contain at least the initial state
        return false;
    }
    redo_stack.push(undo_stack.top());
    undo_stack.pop();
    set_state(undo_stack.top(), obj);
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Object, class State>
CINO_INLINE
bool AbstractUndoRedo<Object,State>::redo(Object & obj)
{
    if(redo_stack.empty()) return false;
    undo_stack.push(redo_stack.top());
    redo_stack.pop();
    set_state(undo_stack.top(), obj);
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Object, class State>
CINO_INLINE
void AbstractUndoRedo<Object,State>::reset(Object & obj)
{
    assert(!undo_stack.empty());
    while(undo_stack.size()>1) undo_stack.pop();
    while(!redo_stack.empty()) redo_stack.pop();
    set_state(undo_stack.top(), obj);
}

}
