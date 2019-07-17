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
#include <cinolib/profiler.h>
#include <cinolib/cino_inline.h>
#include <set>
#include <iostream>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Profiler::push(const std::string & key)
{
    ProfilerEntry entry;
    entry.key   = key;
    entry.start = std::chrono::high_resolution_clock::now();
    tree_ptr    = tree.add_children(entry, tree_ptr);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Profiler::pop(const std::string extra_string)
{
    using namespace std::chrono;
    tree.node(tree_ptr).item.stop = high_resolution_clock::now();
    double t = delta_s(tree_ptr);

    tot_time += t;
    log_times[tree.node(tree_ptr).item.key] += t;
    log_calls[tree.node(tree_ptr).item.key] += 1;

    std::string s;
    for(uint i=0; i<tree.node(tree_ptr).depth-1; ++i) s += "----";
    s += tree.node(tree_ptr).item.key + " [" + std::to_string(t) + "s]";
    tree.node(tree_ptr).item.s = s;
    std::cout << s  << extra_string << std::endl;

    tree_ptr = tree.node(tree_ptr).father;

    return t;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Profiler::call_stack() const
{
    std::cout << "::::::::::::::: PROFILER CALL TREE (" << tot_time << "s) :::::::::::::::" << std::endl;

    std::vector<ProfilerEntry> items;
    tree.depth_first_traverse(items);

    for(const ProfilerEntry & obj : items) std::cout << obj.s << std::endl;

    std::cout << "::::::::::::::::::::::::::::::::::::::::::::::::::\n" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Profiler::report() const
{
    std::set<std::pair<double,std::string>,std::greater<std::pair<double,std::string>>> ordered_items; // most time consuming first
    for(auto obj : log_times) ordered_items.insert(std::make_pair(obj.second,obj.first));

    std::cout << "::::::::::::::: PROFILER STATISTICS (" << tot_time << "s) :::::::::::::::" << std::endl;

    for(auto obj : ordered_items)
    {
        std::cout << obj.first << "s\t" << obj.second << " (called " << log_calls.at(obj.second) << " times)" << std::endl;
    }

    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::\n" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Profiler::delta_s(const uint id) const
{
    using namespace std::chrono;
    duration<double> delta = duration_cast<duration<double>>(tree.node(id).item.stop - tree.node(id).item.start);
    return delta.count();
}

}
