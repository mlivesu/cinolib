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
#include <cinolib/io/read_IV.h>
#include <cinolib/io/io_utilities.h>

namespace cinolib
{

CINO_INLINE
void read_IV(const char          * filename,
             std::vector<double> & xyz,
             std::vector<uint>   & tri,
             std::vector<int>    & patch)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_IV() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    assert(seek_keyword(fp, "vertex"));
    assert(seek_keyword(fp, "["));

    // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
    //
    double x,y,z;
    while (fscanf(fp, "%lf %lf %lf,", &x, &y, &z) == 3)
    {
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    assert(seek_keyword(fp, "coordIndex"));
    assert(seek_keyword(fp, "["));

    int v0, v1, v2;
    while (fscanf(fp,"%d, %d, %d, -1,", &v0, &v1, &v2) == 3)
    {
        tri.push_back(v0);
        tri.push_back(v1);
        tri.push_back(v2);
    }

    if (seek_keyword(fp, "materialIndex"))
    {
        assert(seek_keyword(fp, "["));

        int patch_id;
        while (fscanf(fp,"%d,", &patch_id) == 1)
        {
            patch.push_back(patch_id);
        }
    }

    fclose(fp);
}

}
