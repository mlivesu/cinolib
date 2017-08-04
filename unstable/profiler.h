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
#include <chrono>
#include <stack>
#include <map>

namespace cinolib
{

// http://www.cplusplus.com/reference/chrono/high_resolution_clock/now/

typedef struct
{
    std::chrono::high_resolution_clock::time_point start, stop;
    std::string func_prototype;
}
ProfilerEntry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class Profiler
{
    public:

        Profiler() {}

        void push(const std::string & func_prototype);
        void pop();

        std::string report()     const;
        std::string call_stack() const;

    private:

        std::vector<std::string>     str_calls;
        std::map<std::string,double> report_times; // how much time was spent into each function
        std::map<std::string,uint>   report_calls; // how many times a function was called
        std::stack<ProfilerEntry>    stack; // THIS MAUST BE A N-ARY TREE!!!!!!!
};

//    Profiler p;
//    p.push("cinolib::func1");
//        p.push("cinolib::func2.1");
//            p.push("cinolib::func3.1");
//                p.push("cinolib::func4.1"); p.pop();
//            p.pop();
//            p.push("cinolib::func3.2"); p.pop();
//        p.pop();
//        p.push("cinolib::func2.2"); p.pop();
//        p.push("cinolib::func2.3");
//            p.push("cinolib::func3.3"); p.pop();
//            p.push("cinolib::func3.4"); p.pop();
//            p.push("cinolib::func3.5"); p.pop();
//        p.pop();
//        p.push("cinolib::func2.4");
//            p.push("cinolib::func3.6"); p.pop();
//        p.pop();
//        p.push("cinolib::func2.5"); p.pop();
//    p.pop();

//    std::cout << p.call_stack() << std::endl;
//    std::cout << p.report() << std::endl;


}

#ifndef  CINO_STATIC_LIB
#include "profiler.cpp"
#endif

#endif // CINO_PROFILER_H
