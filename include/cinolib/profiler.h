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
#ifndef CINO_PROFILER_H
#define CINO_PROFILER_H

#include <cinolib/tree.h>
#include <chrono>
#include <map>
#include <string>

namespace cinolib
{

typedef struct
{
    // http://www.cplusplus.com/reference/chrono/high_resolution_clock/now/
    std::chrono::high_resolution_clock::time_point start, stop;
    std::string key;
    std::string s;
}
ProfilerEntry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class Profiler
{
    public:

        explicit Profiler() { tree_ptr = 0; tot_time = 0; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void   push(const std::string & key);
        double pop(const std::string extra_string = "");

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void report()     const;
        void call_stack() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        double delta_s(const uint id) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double                       tot_time;
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


/* EXAMPLE
        Profiler profiler;
        profiler.push("cinolib::func1");
            profiler.push("cinolib::func2.1");
                profiler.push("cinolib::func3.1");
                    profiler.push("cinolib::func4.1"); profiler.pop();
                    profiler.push("cinolib::func4.1"); profiler.pop();
                    profiler.push("cinolib::func4.1"); profiler.pop();
                    profiler.push("cinolib::func4.1"); profiler.pop();
                    profiler.push("cinolib::func4.1"); profiler.pop();
                    profiler.push("cinolib::func4.1"); profiler.pop();
                profiler.pop();
                profiler.push("cinolib::func3.2"); profiler.pop();
            profiler.pop();
            profiler.push("cinolib::func2.2"); profiler.pop();
            profiler.push("cinolib::func2.3");
                profiler.push("cinolib::func3.3"); profiler.pop();
                profiler.push("cinolib::func3.4"); profiler.pop();
                profiler.push("cinolib::func3.5"); profiler.pop();
            profiler.pop();
            profiler.push("cinolib::func2.4");
                profiler.push("cinolib::func3.6"); profiler.pop();
            profiler.pop();
            profiler.push("cinolib::func2.5"); profiler.pop();
        profiler.pop();

        profiler.call_stack();
        profiler.report();
*/
