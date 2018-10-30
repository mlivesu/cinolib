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
#include <cinolib/io/write_OFF.h>


#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_OFF(const char                * filename,
              const std::vector<double> & xyz,
              const std::vector<uint>  & tri,
              const std::vector<uint>  & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << std::endl;
        exit(-1);
    }

    int n_poly = tri.size()/3 + quad.size()/4;
    fprintf (fp, "OFF\n%zu %d 0\n", xyz.size()/3, n_poly);

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "3 %d %d %d\n", tri[i], tri[i+1], tri[i+2]);
    }

    for(size_t i=0; i<quad.size(); i+=4)
    {
        fprintf(fp, "4 %d %d %d %d\n", quad[i], quad[i+1], quad[i+2], quad[i+3]);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OFF(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & faces)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << std::endl;
        exit(-1);
    }

    uint n_faces = faces.size();
    fprintf (fp, "OFF\n%zu %d 0\n", xyz.size()/3, n_faces);

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(uint i=0; i<faces.size(); ++i)
    {
        fprintf(fp, "%d ", static_cast<int>(faces.at(i).size()));
        for(uint j=0; j<faces.at(i).size(); ++j)
        {
            fprintf(fp, "%d ", faces.at(i).at(j));
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}


}
