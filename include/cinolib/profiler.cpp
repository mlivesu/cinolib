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
void Profiler::pop()
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
    std::cout << s << std::endl;

    tree_ptr = tree.node(tree_ptr).father;
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
