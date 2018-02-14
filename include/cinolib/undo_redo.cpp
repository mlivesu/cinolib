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
    if (undo_stack.size()<2)
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
    if (redo_stack.empty()) return false;
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
