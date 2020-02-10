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
#ifndef CINO_STL_CONTAINER_UTILITIES
#define CINO_STL_CONTAINER_UTILITIES

#include <vector>
#include <chrono>
#include <cinolib/cino_inline.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C>
CINO_INLINE
void CHECK_BOUNDS(const C & container, int index);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS(const C & container, const E & element);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN(const C & container, const E & element);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS_VEC(const C & container, const E & element);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN_VEC(const C & container, const E & element);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C>
CINO_INLINE
void PRINT(const C & container, const std::string & name = "");

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename C_iterator>
CINO_INLINE
int INDEX_OF(const C & container, const C_iterator & it);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Iterator>
CINO_INLINE
int INDEX_OF(const Iterator & base_addr, const Iterator & it);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REMOVE_FROM_VEC(std::vector<T> & vec, const T & elem);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void SORT_VEC(std::vector<T> & vec, const bool biggest_first = true);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
std::vector<T> SORT_VEC(const std::vector<T> & vec, const bool biggest_first = true);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
bool INTERSECTION_IS_EMPTY_VEC(const std::vector<T> & vec1, const std::vector<T> & vec2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REMOVE_DUPLICATES_FROM_VEC(std::vector<T> & vec);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
T AVERAGE_VEC(const std::vector<T> & vec);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_INTERSECTION(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_DIFFERENCE(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_SYMMETRIC_DIFFERENCE(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void CIRCULAR_SHIFT_VEC(std::vector<T> & vec, const T & new_first_elem);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REVERSE_VEC(std::vector<T> & vec);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void SHUFFLE_VEC(std::vector<T> & vec, const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count());

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void VEC_INSERT_AFTER(std::vector<T> & vec, const T & ref_item, const T & new_item);

}

#ifndef  CINO_STATIC_LIB
#include "stl_container_utilities.cpp"
#endif

#endif // CINO_STL_CONTAINER_UTILITIES
