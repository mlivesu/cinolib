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
#include <cinolib/io/write_OBJ.h>
#include <cinolib/color.h>
#include <cinolib/stl_container_utilities.h>
#include <cinolib/string_utilities.h>

#include <iostream>
#include <algorithm>
#include <map>

namespace cinolib
{


CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<uint>   & tri,
               const std::vector<uint>   & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    for(size_t i=0; i<quad.size(); i+=4)
    {
        fprintf(fp, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(auto p : poly)
    {
        fprintf(fp, "f ");
        for(uint vid : p) fprintf(fp, "%d ", vid+1);
        fprintf(fp, "\n");
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<uint>   & tri,
               const std::vector<uint>   & quad,
               const std::vector<Color>  & colors)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string mtl_filename(filename);
    mtl_filename.resize(mtl_filename.size()-4);
    mtl_filename.append(".mtu");

    FILE *f_mtl = fopen(mtl_filename.c_str(), "w");
    FILE *f_obj = fopen(filename, "w");

    if(!f_obj || !f_mtl)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    assert(colors.size() == tri.size()/3 + quad.size()/4);

    std::map<Color,uint> color_map;

    for(const Color & c : colors)
    {
        if (DOES_NOT_CONTAIN(color_map, c))
        {
            uint fresh_id = color_map.size();
            color_map[c]  = fresh_id;
            fprintf(f_mtl, "newmtl color_%d\nKd %f %f %f\n", fresh_id, c.r, c.g, c.b);
        }
    }

    fprintf(f_obj, "mtllib %s\n", get_file_name(mtl_filename).c_str());

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_obj, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(uint i=0; i<tri.size(); i+=3)
    {
        fprintf(f_obj, "usemtl color_%d\n", color_map[colors.at(i/3)]);
        fprintf(f_obj, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    for(uint i=0; i<quad.size(); i+=4)
    {
        fprintf(f_obj, "usemtl color_%d\n", color_map[colors.at(i/4)]);
        fprintf(f_obj, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
    }

    fclose(f_obj);
    fclose(f_mtl);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<uint>   & tri,
               const std::vector<uint>   & quad,
               const Color               & color)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string mtl_filename(filename);
    mtl_filename.resize(mtl_filename.size()-4);
    mtl_filename.append(".mtu");

    FILE *f_mtl = fopen(mtl_filename.c_str(), "w");
    FILE *f_obj = fopen(filename, "w");

    if(!f_obj || !f_mtl)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    fprintf(f_mtl, "newmtl color\nKd %f %f %f\n", color.r, color.g, color.b);
    fprintf(f_obj, "mtllib %s\n", get_file_name(mtl_filename).c_str());

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_obj, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(uint i=0; i<tri.size(); i+=3)
    {
        fprintf(f_obj, "usemtl color\n");
        fprintf(f_obj, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    for(uint i=0; i<quad.size(); i+=4)
    {
        fprintf(f_obj, "usemtl color\n");
        fprintf(f_obj, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
    }

    fclose(f_obj);
    fclose(f_mtl);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & poly,
               const std::vector<Color>             & colors)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string mtl_filename(filename);
    mtl_filename.resize(mtl_filename.size()-4);
    mtl_filename.append(".mtu");

    FILE *f_mtl = fopen(mtl_filename.c_str(), "w");
    FILE *f_obj = fopen(filename, "w");

    if(!f_obj || !f_mtl)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    assert(colors.size() == poly.size());

    std::map<Color,uint> color_map;

    for(const Color & c : colors)
    {
        if (DOES_NOT_CONTAIN(color_map, c))
        {
            uint fresh_id = color_map.size();
            color_map[c]  = fresh_id;
            fprintf(f_mtl, "newmtl color_%d\nKd %f %f %f\n", fresh_id, c.r, c.g, c.b);
        }
    }

    fprintf(f_obj, "mtllib %s\n", get_file_name(mtl_filename).c_str());

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_obj, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(uint fid=0; fid<poly.size(); ++fid)
    {
        fprintf(f_obj, "usemtl color_%d\n", color_map.at(colors.at(fid)));
        fprintf(f_obj, "f ");
        for(uint vid : poly.at(fid)) fprintf(f_obj, "%d ", vid+1);
        fprintf(f_obj, "\n");
    }

    fclose(f_obj);
    fclose(f_mtl);
}



//CINO_INLINE
//void write_OBJ(const char                * filename,
//               const std::vector<double> & xyz,
//               const std::vector<uint>   & tri,
//               const std::vector<uint>   & quad,
//               const std::vector<int>    & labels)
//{
//    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

//    std::string mtl_filename(filename);
//    mtl_filename.resize(mtl_filename.size()-4);
//    mtl_filename.append(".mtu");

//    FILE *f_mtl = fopen(mtl_filename.c_str(), "w");
//    FILE *f_obj = fopen(filename, "w");

//    if(!f_obj || !f_mtl)
//    {
//        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
//        exit(-1);
//    }

//    int min    = *std::min_element(labels.begin(), labels.end());
//    int max    = *std::max_element(labels.begin(), labels.end());
//    int delta  = max - min;

//    for(int l=min; l<=max; ++l)
//    {
//        Color c = Color::scatter(delta, l);
//        fprintf(f_mtl, "newmtl label_%d\nKd %f %f %f\n", l, c.r, c.g, c.b);
//    }

//    fprintf(f_obj, "mtllib %s\n", get_file_name(mtl_filename).c_str().c_str());

//    for(uint i=0; i<xyz.size(); i+=3)
//    {
//        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
//        //
//        fprintf(f_obj, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
//    }

//    for(uint i=0; i<tri.size(); i+=3)
//    {
//        fprintf(f_obj, "usemtl label_%d\n", labels.at(i/3));
//        fprintf(f_obj, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
//    }

//    for(uint i=0; i<quad.size(); i+=4)
//    {
//        fprintf(f_obj, "usemtl label_%d\n", labels.at(i/4));
//        fprintf(f_obj, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
//    }

//    fclose(f_obj);
//    fclose(f_mtl);
//}

}
