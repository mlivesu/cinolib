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
#include <cinolib/io/read_STL.h>
#include <cinolib/io/io_utilities.h>
#include <map>

namespace cinolib
{

CINO_INLINE
void read_STL(const char         * filename,
              std::vector<vec3d> & verts,
              std::vector<uint>  & tris)
{
    std::vector<vec3d> normals;
    read_STL(filename, verts, normals, tris);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_STL(const char         * filename,
              std::vector<vec3d> & verts,
              std::vector<vec3d> & normals,
              std::vector<uint>  & tris)
{
    // https://en.wikipedia.org/wiki/STL_(file_format)

    verts.clear();
    normals.clear();
    tris.clear();

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");
    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_STL() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    std::map<vec3d,uint> vmap;

    if(seek_keyword(fp, "solid")) // ASCII file
    {
        while(seek_keyword(fp, "facet"))
        {
            vec3d n;
            if(!seek_keyword(fp, "normal")) assert(false && "could not find keyword NORMAL");
            if(!eat_double(fp, n.x())) assert(false && "could not parse x coord");
            if(!eat_double(fp, n.y())) assert(false && "could not parse y coord");
            if(!eat_double(fp, n.z())) assert(false && "could not parse z coord");
            normals.push_back(n);

            if(!seek_keyword(fp, "outer")) assert(false && "could not find keyword OUTER");
            if(!seek_keyword(fp, "loop"))  assert(false && "could not find keyword LOOP");
            for(int i=0; i<3; ++i)
            {
                vec3d v;
                if(!seek_keyword(fp, "vertex")) assert(false && "could not find keyword VERTEX");
                if(!eat_double(fp, v.x())) assert(false && "could not parse x coord");
                if(!eat_double(fp, v.y())) assert(false && "could not parse y coord");
                if(!eat_double(fp, v.z())) assert(false && "could not parse z coord");

                auto it = vmap.find(v);
                if(it == vmap.end())
                {
                    uint fresh_id = vmap.size();
                    vmap[v] = fresh_id;
                    verts.push_back(v);
                    tris.push_back(fresh_id);
                }
                else tris.push_back(it->second);
            }
            if(!seek_keyword(fp, "endloop"))  assert(false && "could not find keyword ENDLOOP");
            if(!seek_keyword(fp, "endfacet")) assert(false && "could not find keyword ENDFACET");
        }
        fclose(fp);
    }

    if(tris.empty()) // BINARY file
    {
        // close file in ASCII mode and reopen it in binary mode
        fclose(fp);
        FILE *fp = fopen(filename, "rb");
        assert(fp!=NULL);

        // read header
        char header[80];
        if(fread(header, 1, 80, fp)!=80) assert(false && "error reading STL binary header");

        // read triangles
        unsigned int nt;
        if(fread(&nt, sizeof(unsigned int), 1, fp)!=1) assert(false && "error reading number of triangles");
        for(unsigned int i=0; i<nt; ++i)
        {
            // read normal
            float nf[3];
            if(fread(&nf, sizeof(float), 3, fp)!=3) assert(false && "error reading normal");
            vec3d n(nf[0], nf[1], nf[2]);
            normals.push_back(n);

            // read verts
            for(int j=0; j<3; ++j)
            {
                float vf[3];
                if(fread(&vf, sizeof(float), 3, fp)!=3) assert(false && "error reading vertex");

                vec3d v(vf[0], vf[1], vf[2]);
                auto it = vmap.find(v);
                if(it == vmap.end())
                {
                    uint fresh_id = vmap.size();
                    vmap[v] = fresh_id;
                    verts.push_back(v);
                    tris.push_back(fresh_id);
                }
                else tris.push_back(it->second);
            }

            // read (and discard) attribute
            unsigned short attribute;
            if(fread(&attribute, sizeof(unsigned short), 1, fp)!=1) assert(false && "error reading attribute");
        }
        fclose(fp);
    }
}

}
