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
