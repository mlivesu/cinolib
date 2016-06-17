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
#ifndef CINO_LOGGER_H
#define CINO_LOGGER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <streambuf>

namespace cinolib
{

// http://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream
//
struct Logger : std::ostream, std::streambuf
{
    Logger();

    int  overflow(int c);
    void dispatch_cout(char c);
    void dispatch_file(char c);
    void enable();
    void disable();
    void set_log_file(const char *filename);

    bool dispatch_to_cout;
    bool dispatch_to_file;
    std::ofstream f;
};

CINO_INLINE std::ostream & newl (std::ostream & os);
CINO_INLINE std::ostream & endl (std::ostream & os);
CINO_INLINE std::ostream & flush(std::ostream & os);

}

#ifndef  CINO_STATIC_LIB
#include "logger.cpp"
#endif

#endif // CINO_LOGGER_H
