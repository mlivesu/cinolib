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
#include <cinolib/string_utilities.h>

namespace cinolib
{

// "/folder1/folder2/file.ext" => "ext"
//
CINO_INLINE
std::string get_file_extension(const std::string & s)
{
    size_t pos = s.find_last_of(".");
    if(pos>=s.size()) return "";
    return s.substr(pos+1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// "/folder1/folder2/file.ext" => "/folder1/folder2/"
//
CINO_INLINE
std::string get_file_path(const std::string & s)
{
    size_t pos = s.find_last_of("/");
    if(pos>=s.size()) return "./";
    return s.substr(0,pos+1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


// "/folder1/folder2/file.ext" => "file.ext"
//
CINO_INLINE
std::string get_file_name(const std::string & s, const bool with_extension)
{
    size_t pos = s.find_last_of("/");
    std::string ss = (pos>=s.size()) ? s : s.substr(pos+1);
    if(with_extension) return ss;
    pos = ss.find_last_of(".");
    return ss.substr(0,pos);
}

}
