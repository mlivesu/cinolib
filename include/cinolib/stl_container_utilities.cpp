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
#include <cinolib/stl_container_utilities.h>
#include <algorithm>
#include <numeric>
#include <random>
#include <iterator>

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

template<typename C>
CINO_INLINE
void PRINT(const C & container, const std::string & name)
{
    std::cout << name << ": ";
    for(auto item : container) std::cout << item << " ";
    std::cout << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename C_iterator>
CINO_INLINE
int INDEX_OF(const C & container, const C_iterator & it)
{
    return std::distance(container.begin(), it);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename Iterator>
CINO_INLINE
int INDEX_OF(const Iterator & base_addr, const Iterator & it)
{
    return std::distance(base_addr, it);
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
    if(biggest_first) std::reverse(vec.begin(), vec.end());
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REMOVE_DUPLICATES_FROM_VEC(std::vector<T> & vec)
{
    // https://stackoverflow.com/questions/1041620/whats-the-most-efficient-way-to-erase-duplicates-and-sort-a-vector
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
T AVERAGE_VEC(const std::vector<T> & vec)
{
    return std::accumulate(vec.begin(), vec.end(), 0.0)/static_cast<double>(vec.size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_INTERSECTION(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets)
{
    // std::set_intersection requires the sets to be pre-ordered from smaller to bigger,
    // so if the input sets are not (i.e. they are vectors) use true to ask ordering or
    // order them yourself

    if(pre_sort_sets)
    {
        SORT_VEC(set1, false);
        SORT_VEC(set2, false);
    }
    inters.clear();
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(inters));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_DIFFERENCE(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets)
{
    // std::set_difference requires the sets to be pre-ordered from smaller to bigger,
    // so if the input sets are not (i.e. they are vectors) use true to ask ordering or
    // order them yourself

    if(pre_sort_sets)
    {
        SORT_VEC(set1, false);
        SORT_VEC(set2, false);
    }
    inters.clear();
    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(inters));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C, typename E>
CINO_INLINE
void SET_SYMMETRIC_DIFFERENCE(C & set1, C & set2, std::vector<E> & inters, const bool pre_sort_sets)
{
    // std::set_symmetric_difference requires the sets to be pre-ordered from smaller to bigger,
    // so if the input sets are not (i.e. they are vectors) use true to ask ordering or
    // order them yourself

    if(pre_sort_sets)
    {
        SORT_VEC(set1, false);
        SORT_VEC(set2, false);
    }
    inters.clear();
    std::set_symmetric_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(inters));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void CIRCULAR_SHIFT_VEC(std::vector<T> & vec, const T & new_first_elem)
{
    auto ptr = std::find(vec.begin(), vec.end(), new_first_elem);
    assert(ptr!=vec.end());
    std::rotate(vec.begin(), ptr, vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void REVERSE_VEC(std::vector<T> & vec)
{
    std::reverse(vec.begin(), vec.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void SHUFFLE_VEC(std::vector<T> & vec, const unsigned seed)
{
    // randomize points (http://www.cplusplus.com/reference/algorithm/shuffle/)
    std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void VEC_INSERT_AFTER(std::vector<T> & vec, const T & ref_item, const T & new_item)
{
    auto pos = std::find(vec.begin(), vec.end(), ref_item);
    assert(pos!=vec.end());
    vec.insert(pos, new_item);
}

}
