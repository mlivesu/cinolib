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

#include <cinolib/cino_inline.h>

namespace cinolib
{

CINO_INLINE
Logger::Logger() : std::ostream(this)
{
    enable();
}

CINO_INLINE
int Logger::overflow(int c)
{
    if (dispatch_to_cout) dispatch_cout(c);
    if (dispatch_to_file) dispatch_file(c);
    return 0;
}

CINO_INLINE
void Logger::dispatch_cout(char c)
{
    std::cout.put(c);
}

CINO_INLINE
void Logger::dispatch_file(char c)
{
    f.put(c);
}

CINO_INLINE
void Logger::enable()
{
    dispatch_to_cout = true;
    dispatch_to_file = f.is_open();
}

CINO_INLINE
void Logger::disable()
{
    dispatch_to_cout = false;
    dispatch_to_file = false;
}

CINO_INLINE
void Logger::set_log_file(const char * filename)
{
    f.open(filename);
    if (f.is_open()) dispatch_to_file = true;
}

// https://github.com/isocpp/CppCoreGuidelines/issues/357
//
CINO_INLINE
std::ostream & newl(std::ostream & os)
{
    return os << "\n";
}

CINO_INLINE
std::ostream & flush(std::ostream & os)
{
    os.flush();
    return os;
}

CINO_INLINE
std::ostream & endl(std::ostream & os)
{
    newl(os);
    flush(os);
    return os;
}

}
