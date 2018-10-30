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
