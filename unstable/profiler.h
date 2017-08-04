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
#ifndef CINO_PROFILER_H
#define CINO_PROFILER_H

#include <cinolib/cinolib.h>
#include <cinolib/unstable/tree.h>
#include <chrono>
#include <map>

namespace cinolib
{

typedef struct
{
    // http://www.cplusplus.com/reference/chrono/high_resolution_clock/now/
    std::chrono::high_resolution_clock::time_point start, stop;
    std::string f_prototype;
    std::string s;
}
ProfilerEntry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class Profiler
{
    public:

        Profiler() { tree_ptr = 0; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push(const std::string & f_prototype);
        void pop();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void report()     const;
        void call_stack() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double delta_s(const uint id) const;

    protected:

        std::map<std::string,double> log_times; // how much time was spent into each function
        std::map<std::string,uint>   log_calls; // how many times a function was called
        Tree<ProfilerEntry>          tree;
        uint                         tree_ptr;
};

}

#ifndef  CINO_STATIC_LIB
#include "profiler.cpp"
#endif

#endif // CINO_PROFILER_H
