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
#include <cinolib/unstable/profiler.h>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Profiler::push(const std::string & func_prototype)
{
    ProfilerEntry e;
    e.func_prototype = func_prototype;
    e.start = std::chrono::high_resolution_clock::now();
    stack.push(e);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Profiler::pop()
{
    using namespace std::chrono;
    high_resolution_clock::time_point stop = high_resolution_clock::now();
    duration<double> delta = duration_cast<duration<double>>(stop - stack.top().start);
    report_times[stack.top().func_prototype] += delta.count();
    report_calls[stack.top().func_prototype] += 1;
    std::string s;
    for(uint i=0; i<stack.size()-1; ++i) s.append("----");
    s.append(stack.top().func_prototype);
    str_calls.push_back(s);
    stack.pop();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::string Profiler::call_stack() const
{
    std::cout << "::::::::::::::: PROFILER CALL TREE :::::::::::::::" << std::endl;
    for(uint i=str_calls.size(); i-->0; ) std::cout << str_calls.at(i) << std::endl;
    std::cout << "::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    return std::string();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::string Profiler::report() const
{
    std::set<std::pair<double,std::string>,std::greater<std::pair<double,std::string>>> ordered_list;
    for(auto obj : report_times) ordered_list.insert(std::make_pair(obj.second,obj.first));

    std::string s("::::::::::::::: PROFILER STATISTICS :::::::::::::::\n");
    for(auto obj : ordered_list)
    {
        s.append(std::to_string(obj.first));
        s.append("\t");
        s.append(obj.second);
        s.append(" (called ");
        s.append(std::to_string(report_calls.at(obj.second)));
        s.append(" times)\n");
    }
    s.append(":::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    return s;
}

}
