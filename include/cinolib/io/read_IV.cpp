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
#include <cinolib/io/read_IV.h>
#include <string.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
bool seek_keyword(FILE *fp, const char *kw)
{
    static char s[256];
    s[0]='\0';
    do fscanf(fp,"%255s",s); while (strcmp(s,kw) && !feof(fp));
    if (feof(fp)) return 0;
    return 1;
}

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
