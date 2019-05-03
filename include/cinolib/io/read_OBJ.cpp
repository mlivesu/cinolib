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
*                                                                               *
*     Fabrizio Corda (cordafab@gmail.com)                                       *
*     http://www.fabriziocorda.com                                              *
*                                                                               *
*     University of Cagliari                                                    *
*     Via Ospedale, 72                                                          *
*     09124 Cagliari,                                                           *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/io/read_OBJ.h>
#include <cinolib/to_openGL_unified_verts.h>
#include <cinolib/string_utilities.h>
#include <sstream>
#include <string.h>
#include <iostream>
#include <assert.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://stackoverflow.com/questions/9310327/sscanf-optional-column
//
CINO_INLINE
void read_point_id(char * s, int & v, int & vt, int & vn)
{
    v = vt = vn = -1;
         if(sscanf(s, "%d/%d/%d", &v, &vt, &vn) == 3) { --v; --vt; --vn; }
    else if(sscanf(s, "%d/%d",    &v, &vt     ) == 2) { --v; --vt;       }
    else if(sscanf(s, "%d//%d",   &v, &vn     ) == 2) { --v; --vn;       }
    else if(sscanf(s, "%d",       &v          ) == 1) { --v;             }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & poly)
{
    std::vector<vec3d> tex, nor;
    std::vector<std::vector<uint>> poly_tex, poly_nor;
    std::vector<Color> poly_col;
    read_OBJ(filename, verts, tex, nor, poly, poly_tex, poly_nor, poly_col);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & xyz,
              std::vector<vec3d>             & uvw,
              std::vector<std::vector<uint>> & poly)
{
    std::vector<vec3d> pos, tex, nor;
    std::vector<std::vector<uint>> poly_pos, poly_tex, poly_nor;
    std::vector<Color> poly_col;
    read_OBJ(filename, pos, tex, nor, poly_pos, poly_tex, poly_nor, poly_col);

    if (poly_pos.size() == poly_tex.size())
    {
        to_openGL_unified_verts(pos, tex, poly_pos, poly_tex, xyz, uvw, poly);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,           // vertex xyz positions
              std::vector<vec3d>             & tex,           // vertex uv(w) texture coordinates
              std::vector<vec3d>             & nor,           // vertex normals
              std::vector<std::vector<uint>> & poly_pos,      // polygons with references to pos
              std::vector<std::vector<uint>> & poly_tex,      // polygons with references to tex
              std::vector<std::vector<uint>> & poly_nor,      // polygons with references to nor
              std::vector<Color>             & poly_col,      // per polygon colors
              std::string                    & diffuse_path,  // path of the image encoding the diffuse  texture component
              std::string                    & specular_path, // path of the image encoding the specular texture component
              std::string                    & normal_path)   // path of the image encoding the normal   texture component
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    pos.clear();
    tex.clear();
    nor.clear();
    poly_pos.clear();
    poly_tex.clear();
    poly_nor.clear();
    poly_col.clear();
    diffuse_path.clear();
    specular_path.clear();
    normal_path.clear();

    FILE *f = fopen(filename, "r");

    if(!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    std::map<std::string,Color> color_map;
    Color curr_color = Color::WHITE();     // set WHITE as default color
    bool has_per_face_color = false;       // true if a mtllib is found. If "has_per_face_color" stays
                                           // false the "poly_color" vector will be emptied before returning.
    char line[1024];
    while(fgets(line, 1024, f))
    {
        switch(line[0])
        {
            case 'v':
            {
                // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                //
                double a, b, c;
                     if(sscanf(line, "v  %lf %lf %lf", &a, &b, &c) == 3) pos.push_back(vec3d(a,b,c));
                else if(sscanf(line, "vt %lf %lf %lf", &a, &b, &c) == 3) tex.push_back(vec3d(a,b,c));
                else if(sscanf(line, "vt %lf %lf %lf", &a, &b, &c) == 2) tex.push_back(vec3d(a,b,0));
                else if(sscanf(line, "vn %lf %lf %lf", &a, &b, &c) == 3) nor.push_back(vec3d(a,b,c));
                break;
            }

            case 'f':
            {
                std::string s(line);
                s = s.substr(1,s.size()-1); // discard the 'f' letter
                std::istringstream ss(s);
                std::vector<uint> p_pos, p_tex, p_nor;
                for(std::string sub_str; ss >> sub_str;)
                {
                    int v_pos, v_tex, v_nor;
                    read_point_id(strdup(sub_str.c_str()), v_pos, v_tex, v_nor);
                    if (v_pos >= 0) p_pos.push_back(v_pos);
                    if (v_tex >= 0) p_tex.push_back(v_tex);
                    if (v_nor >= 0) p_nor.push_back(v_nor);
                }
                if (!p_tex.empty()) poly_tex.push_back(p_tex);
                if (!p_nor.empty()) poly_nor.push_back(p_nor);
                if (!p_pos.empty())
                {
                    poly_pos.push_back(p_pos);
                    poly_col.push_back(curr_color);
                }
                break;
            }

            case 'u':
            {
                char mat_c[1024];
                if (sscanf(line, "usemtl %s", mat_c) == 1)
                {
                    auto query = color_map.find(std::string(mat_c));
                    if (query != color_map.end())
                    {
                        curr_color = query->second;
                    }
                    else std::cerr << "WARNING: could not find material: " << mat_c << std::endl;
                }
                break;
            }

            case 'm':
            {
                char mtu_c[1024];
                if (sscanf(line, "mtllib %[^\n]s", mtu_c) == 1)
                {
                    std::string s0(filename);
                    std::string s1(mtu_c);
                    std::string s2 = get_file_path(s0) + get_file_name(s1);
                    read_MTU(s2.c_str(), color_map, diffuse_path, specular_path, normal_path);
                    has_per_face_color = true;
                }
                break;
            }
        }
    }
    fclose(f);
    if (!has_per_face_color) poly_col.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,         // vertex xyz positions
              std::vector<vec3d>             & tex,         // vertex uv(w) texture coordinates
              std::vector<vec3d>             & nor,         // vertex normals
              std::vector<std::vector<uint>> & poly_pos,    // polygons with references to pos
              std::vector<std::vector<uint>> & poly_tex,    // polygons with references to tex
              std::vector<std::vector<uint>> & poly_nor,    // polygons with references to nor
              std::vector<Color>             & poly_col)    // per polygon colors
{
    std::string  diffuse_path;
    std::string  specular_path;
    std::string  normal_path;
    read_OBJ(filename, pos, tex, nor, poly_pos, poly_tex, poly_nor, poly_col, diffuse_path, specular_path, normal_path);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MTU(const char                  * filename,
              std::map<std::string,Color> & color_map,
              std::string                 & diffuse_path,  // path of the image encoding the diffuse  texture component
              std::string                 & specular_path, // path of the image encoding the specular texture component
              std::string                 & normal_path)  // path of the image encoding the normal   texture component
{
    diffuse_path.clear();
    specular_path.clear();
    normal_path.clear();
    color_map.clear();

    FILE *f = fopen(filename, "r");

    if(!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_MTU() : couldn't open color file " << filename << std::endl;
        exit(-1);
    }

    char curr_material[1024];
    char line[1024];
    while(fgets(line, 1024, f))
    {
        switch(line[0])
        {
            case 'n': sscanf(line, "newmtl %s", curr_material); break;

            case 'K':
            {
                float r,g,b;
                if (sscanf(line, "Kd %f %f %f", &r, &g, &b) == 3)
                {
                    color_map[std::string(curr_material)] = Color(r,g,b);
                }
                break;
            }

            default:
            {
                std::istringstream iss(line);
                std::string token;
                iss >> token;
                if(token == "map_Kd")
                {
                    std::string s(filename);
                    iss >> token;
                    diffuse_path = get_file_path(s) + token;
                }
            }
        }
    }
    fclose(f);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MTU(const char                  * filename,
              std::map<std::string,Color> & color_map)
{
    std::string  diffuse_path;
    std::string  specular_path;
    std::string  normal_path;
    read_MTU(filename, color_map, diffuse_path, specular_path, normal_path);
}

}
