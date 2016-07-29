/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/logger.h>
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
