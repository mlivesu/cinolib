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
#ifndef CINO_STANDARD_ELEMENTS_TABLES_H
#define CINO_STANDARD_ELEMENTS_TABLES_H

#include <sys/types.h>

/*    Reference               Reference
 *    Hexahedron:             Tetrahedron:
 *
 *       v7------v6              v3
 *      / |     / |             /| \
 *    v4------v5  |           /  |   \
 *    |   |    |  |         v0---|----v2
 *    |  v3----|--v2         \   |   /
 *    | /      | /             \ | /
 *    v0------v1                 v1
*/

namespace cinolib
{

static const uint TRI_EDGES[3][2] =
{
    { 0, 1 }, // e0
    { 1, 2 }, // e1
    { 2, 0 }, // e2
};

static const uint QUAD_EDGES[4][2] =
{
    { 0, 1 }, // e0
    { 1, 2 }, // e1
    { 2, 3 }, // e2
    { 3, 0 }, // e3
};

static const uint TET_FACES[4][3] = // for outgoing normals
{
    { 0, 2, 1 } , // f0
    { 0, 1, 3 } , // f1
    { 0, 3, 2 } , // f2
    { 1, 2, 3 }   // f3
};

static const uint TET_EDGES[6][2] =
{
    { 0, 2 }, // e0
    { 2, 1 }, // e1
    { 1, 0 }, // e2
    { 1, 3 }, // e3
    { 3, 0 }, // e4
    { 3, 2 }  // e5
};

static const uint TET_INCIDENT_EDEGES[4][3] =
{
    { 0, 2, 4 }, // edges incident to v0
    { 1, 2, 3 }, // edges incident to v1
    { 0, 1, 5 }, // edges incident to v2
    { 3, 4, 5 }, // edges incident to v3
};

static const uint HEXA_FACES[6][4] = // for outgoing normals
{
    { 0 , 3 , 2 , 1 } , // f0
    { 1 , 2 , 6 , 5 } , // f1
    { 4 , 5 , 6 , 7 } , // f2
    { 3 , 0 , 4 , 7 } , // f3
    { 0 , 1 , 5 , 4 } , // f4
    { 2 , 3 , 7 , 6 }   // f5
};

static const uint HEXA_EDGES[12][2] =
{
    { 0, 1 }, // e0
    { 1, 2 }, // e1
    { 2, 3 }, // e2
    { 3, 0 }, // e3
    { 4, 5 }, // e4
    { 5, 6 }, // e5
    { 6, 7 }, // e6
    { 7, 4 }, // e7
    { 0, 4 }, // e8
    { 1, 5 }, // e9
    { 2, 6 }, // e10
    { 3, 7 }  // e11
};

static const uint HEXA_INCIDENT_EDGES[8][3] =
{
    { 0, 3,  8 }, // edges incident to v0
    { 0, 1,  9 }, // edges incident to v1
    { 1, 2, 10 }, // edges incident to v2
    { 2, 3, 11 }, // edges incident to v3
    { 4, 7,  8 }, // edges incident to v4
    { 4, 5,  9 }, // edges incident to v5
    { 5, 6, 10 }, // edges incident to v6
    { 6, 7, 11 }, // edges incident to v7
};

}

#endif // CINO_STANDARD_ELEMENTS_TABLES_H
