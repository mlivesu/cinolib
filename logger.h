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
