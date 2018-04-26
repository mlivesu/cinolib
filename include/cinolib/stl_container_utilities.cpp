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
#include <cinolib/stl_container_utilities.h>
#include <algorithm>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef NDEBUG
template<typename C>
CINO_INLINE
void CHECK_BOUNDS(const C & container, int index)
{
    assert(index >= 0);
    assert(index < (int)container.size());
}
#else
template<typename C>
CINO_INLINE void CHECK_BOUNDS(const C &, int) {}
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS(const C & container, const E & element)
{
    return (container.find(element) != container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN(const C & container, const E & element)
{
    return !CONTAINS(container,element);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS_VEC(const C & container, const E & element)
{
    return (std::find(container.begin(), container.end(), element) != container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN_VEC(const C & container, const E & element)
{
    return !CONTAINS_VEC(container,element);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REMOVE_FROM_VEC(std::vector<T> & vec, const T & elem)
{
    // https://en.wikipedia.org/wiki/Erase–remove_idiom
    //
    // IMPORTANT: erase–remove can only be used with containers
    // holding elements with full value semantics without incurring
    // resource leaks.
    //
    vec.erase(std::remove(vec.begin(), vec.end(), elem), vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void SORT_VEC(std::vector<T> & vec, const bool biggest_first)
{
    std::sort(vec.begin(), vec.end()); // ascending order (smallest first)
    if (biggest_first) std::reverse(vec.begin(), vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
std::vector<T> SORT_VEC(const std::vector<T> & vec, const bool biggest_first)
{
    std::vector<T> tmp = vec;
    SORT_VEC(tmp, biggest_first);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
bool INTERSECTION_IS_EMPTY_VEC(const std::vector<T> & vec1, const std::vector<T> & vec2)
{
    // https://stackoverflow.com/questions/27131628/check-whether-two-elements-have-a-common-element-in-c
    return (std::find_first_of(vec1.begin(), vec1.end(),
                               vec2.begin(), vec2.end()) == vec1.end());
}

}
