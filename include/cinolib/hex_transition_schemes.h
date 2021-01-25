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
#ifndef CINO_HEX_TRANSITION_SCHEMES_H
#define CINO_HEX_TRANSITION_SCHEMES_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

namespace HexTransitions
{

namespace Flat
{

// 4x4 to 2x2 frustum
static const std::vector<double> verts =
{
    0.5  , 0.5  , 0.5  , // v0 -> center of the 4x4 grid. Useful to position the scheme in the target mesh
    0.5  , 0.5  , 0    , // v1
    1    , 0.5  , 0.5  , // v2
    1    , 0.5  , 0    , // v...
    0.5  , 1    , 0    ,
    0.5  , 1    , 0.5  ,
    1    , 1    , 0.5  ,
    1    , 1    , 0    ,
    0    , 0.5  , 0    ,
    0    , 0.5  , 0.5  ,
    0    , 1    , 0    ,
    0    , 1    , 0.5  ,
    0.5  , 0.5  , 1    ,
    1    , 0.5  , 1    ,
    0.5  , 1    , 1    ,
    1    , 1    , 1    ,
    0    , 0.5  , 1    ,
    0    , 1    , 1    ,
    0.5  , 0.65 , 0.5  ,
    0.75 , 0.5  , 0.75 ,
    0.25 , 0.5  , 0.75 ,
    0.5  , 0.5  , 0.75 ,
    0    , 0.5  , 0.75 ,
    1    , 0.5  , 0.75 ,
    0.5  , 0.75 , 0.5  ,
    0.75 , 0.5  , 0    ,
    0.75 , 0.5  , 0.25 ,
    1    , 0.5  , 0.25 ,
    0.75 , 0.65 , 0.5  ,
    1    , 0.75 , 0.5  ,
    0.25 , 0.5  , 0    ,
    0.25 , 0.5  , 0.25 ,
    0.5  , 0.5  , 0.25 ,
    0    , 0.5  , 0.25 ,
    0.5  , 0.75 , 0    ,
    0.75 , 0.5  , 1    ,
    0.75 , 0.5  , 0.5  ,
    0.25 , 0.5  , 1    ,
    0.5  , 0.75 , 1    ,
    0    , 0.75 , 0.5  ,
    0.25 , 0.5  , 0.5  ,
    0.25 , 0.65 , 0.5
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x4 to 2x2 frustum
static const std::vector<std::vector<uint>> faces =
{
    { 16 , 37 , 20 , 22      },
    { 20 , 37 , 12 , 21      },
    { 21 , 12 , 35 , 19      },
    { 19 , 35 , 13 , 23      },
    { 22 , 20 , 40 ,  9      },
    { 40 , 20 , 21 ,  0      },
    {  0 , 21 , 19 , 36      },
    { 36 , 19 , 23 ,  2      },
    { 33 ,  9 , 40 , 31      },
    { 31 , 40 ,  0 , 32      },
    { 32 ,  0 , 36 , 26      },
    { 26 , 36 ,  2 , 27      },
    {  8 , 33 , 31 , 30      },
    { 31 , 32 ,  1 , 30      },
    {  1 , 32 , 26 , 25      },
    { 25 , 26 , 27 ,  3      },
    { 14 , 15 ,  6 ,  5      },
    {  4 ,  7 ,  6 ,  5      },
    {  4 , 10 , 11 ,  5      },
    { 14 , 17 , 11 ,  5      },
    { 22 ,  9 , 39           },
    {  9 , 33 , 39           },
    { 20 , 40 , 41           },
    { 40 , 31 , 41           },
    { 21 ,  0 , 18           },
    { 32 ,  0 , 18           },
    { 19 , 36 , 28           },
    { 26 , 36 , 28           },
    { 23 ,  2 , 29           },
    { 27 ,  2 , 29           },
    {  9 , 40 , 41 , 39      },
    { 40 ,  0 , 18 , 41      },
    {  0 , 36 , 28 , 18      },
    { 36 ,  2 , 29 , 28      },
    { 22 , 20 , 41 , 39      },
    { 33 , 31 , 41 , 39      },
    { 20 , 21 , 18 , 41      },
    { 31 , 32 , 18 , 41      },
    { 21 , 19 , 28 , 18      },
    { 32 , 26 , 28 , 18      },
    { 19 , 23 , 29 , 28      },
    { 26 , 27 , 29 , 28      },
    {  1 , 25 , 34           },
    { 12 , 35 , 38           },
    {  1 , 30 , 34           },
    { 12 , 37 , 38           },
    { 18 , 28 , 24           },
    { 18 , 41 , 24           },
    { 34 , 24 ,  5 ,  4      },
    { 38 , 24 ,  5 , 14      },
    { 24 , 28 , 29 ,  6 ,  5 },
    { 24 , 41 , 39 , 11 ,  5 },
    { 18 , 32 ,  1 , 34 , 24 },
    { 28 , 26 , 25 , 34 , 24 },
    { 18 , 21 , 12 , 38 , 24 },
    { 28 , 19 , 35 , 38 , 24 },
    { 41 , 31 , 30 , 34 , 24 },
    { 41 , 20 , 37 , 38 , 24 },
    { 16 , 37 , 38 , 14 , 17 },
    { 13 , 35 , 38 , 14 , 15 },
    {  8 , 30 , 34 ,  4 , 10 },
    {  3 , 25 , 34 ,  4 ,  7 },
    { 22 , 16 , 17 , 11 , 39 },
    { 23 , 13 , 15 ,  6 , 29 },
    { 33 ,  8 , 10 , 11 , 39 },
    { 27 ,  3 ,  7 ,  6 , 29 },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x4 to 2x2 frustum
static const std::vector<std::vector<uint>> polys =
{
    { 20 , 22 , 30 , 34 ,  4                },
    { 21 , 23 , 30 , 35 ,  8                },
    { 22 , 24 , 31 , 36 ,  5                },
    { 23 , 25 , 31 , 37 ,  9                },
    { 24 , 26 , 32 , 38 ,  6                },
    { 25 , 27 , 32 , 39 , 10                },
    { 26 , 28 , 33 , 40 ,  7                },
    { 27 , 29 , 33 , 41 , 11                },
    { 39 , 42 , 52 , 53 , 46 , 14           },
    { 38 , 46 , 43 , 54 , 55 ,  2           },
    { 37 , 52 , 44 , 56 , 47 , 13           },
    { 36 , 47 , 54 , 45 , 57 ,  1           },
    { 41 , 53 , 65 , 61 , 48 , 50 , 17 , 15 },
    { 40 , 55 , 50 , 63 , 59 , 49 , 16 ,  3 },
    { 35 , 56 , 48 , 64 , 60 , 51 , 18 , 12 },
    { 34 , 57 , 49 , 51 , 62 , 58 , 19 ,  0 },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x4 to 2x2 frustum -> TODO
static const std::vector<std::vector<bool>> winding =
{
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true, true, true },
    { true, true, true, true, true, true, true, true },
    { true, true, true, true, true, true, true, true },
    { true, true, true, true, true, true, true, true },
};

} // Flat

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Half_Flat
{

// 4x2 to 2x1 frustum
static const std::vector<double> verts =
{
    0.5  , 0.5  , 0.5  , // v0 -> center of the 4x4 grid of which this scheme is one half. Useful to position the scheme in the target mesh
    0.5  , 0.5  , 0    ,
    0    , 0.5  , 0.25 ,
    0.25 , 0.5  , 1    ,
    0.5  , 1    , 0    ,
    0.5  , 1    , 0.5  ,
    0.5  , 0.75 , 1    ,
    0    , 0.75 , 0.5  ,
    0    , 0.5  , 0    ,
    0    , 0.5  , 0.5  ,
    0    , 1    , 0    ,
    0    , 1    , 0.5  ,
    0.5  , 0.5  , 1    ,
    0.25 , 0.5  , 0.5  ,
    0.5  , 1    , 1    ,
    0.25 , 0.65 , 0.5  ,
    0    , 0.5  , 1    ,
    0    , 1    , 1    ,
    0.5  , 0.65 , 0.5  ,
    0.25 , 0.5  , 0    ,
    0.25 , 0.5  , 0.75 ,
    0.5  , 0.5  , 0.75 ,
    0    , 0.5  , 0.75 ,
    0.25 , 0.5  , 0.25 ,
    0.5  , 0.75 , 0.5  ,
    0.5  , 0.75 , 0    ,
    0.5  , 0.5  , 0.25
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x2 to 2x1 frustum
static const std::vector<std::vector<uint>> faces =
{
    { 16 ,  3 , 20 , 22      },
    { 20 ,  3 , 12 , 21      },
    { 18 , 15 , 24           },
    { 22 , 16 , 17 , 11 , 7  },
    { 22 , 20 , 13 ,  9      },
    { 13 , 20 , 21 ,  0      },
    { 20 , 21 , 18 , 15      },
    { 18 , 21 , 12 ,  6 , 24 },
    {  2 ,  9 , 13 , 23      },
    { 23 , 13 ,  0 , 26      },
    { 12 ,  3 ,  6           },
    {  8 , 19 , 25 ,  4 , 10 },
    {  8 ,  2 , 23 , 19      },
    { 23 , 26 ,  1 , 19      },
    { 23 , 26 , 18 , 15      },
    { 15 , 20 ,  3 ,  6 , 24 },
    { 16 ,  3 ,  6 , 14 , 17 },
    {  2 ,  8 , 10 , 11 ,  7 },
    {  4 , 10 , 11 ,  5      },
    { 14 , 17 , 11 ,  5      },
    { 22 ,  9 ,  7           },
    {  9 ,  2 ,  7           },
    { 20 , 13 , 15           },
    { 13 , 23 , 15           },
    { 21 ,  0 , 18           },
    { 26 ,  0 , 18           },
    {  1 , 19 , 25           },
    { 15 , 23 , 19 , 25 , 24 },
    { 25 , 24 ,  5 ,  4      },
    { 24 , 15 ,  7 , 11 ,  5 },
    {  9 , 13 , 15 ,  7      },
    { 13 ,  0 , 18 , 15      },
    {  6 , 24 ,  5 , 14      },
    { 18 , 26 ,  1 , 25 , 24 },
    { 22 , 20 , 15 ,  7      },
    {  2 , 23 , 15 ,  7      },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x2 to 2x1 frustum
static const std::vector<std::vector<uint>> polys =
{
    { 20 , 22 , 30 , 34 ,  4                 },
    { 21 , 23 , 30 , 35 ,  8                 },
    { 22 , 24 , 31 ,  6 ,  5                 },
    { 23 , 25 , 31 , 14 ,  9                 },
    {  6 ,  2 ,  7 , 10 , 15 ,  1            },
    { 14 , 33 , 26 , 27 ,  2 , 13            },
    { 34 , 15 , 32 , 29 ,  3 , 16 , 19 ,  0  },
    { 35 , 27 , 28 , 17 , 11 , 29 , 18 , 12  },

};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4x4 to 2x2 frustum -> TODO
static const std::vector<std::vector<bool>> winding =
{
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true                   },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true             },
    { true, true, true, true, true, true, true, true },
    { true, true, true, true, true, true, true, true },
};

} // Half_Flat

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace ConvexEdge
{

// 4 to 2 convex edge
static const std::vector<double> verts =
{
    0.5  , 0.5  , 1    ,
    0    , 0    , 0.5  ,
    1    , 0.5  , 1    ,
    0    , 0    , 1    ,
    0.5  , 0    , 0.5  ,
    0    , 0.5  , 0.5  ,
    0.5  , 0.5  , 0.5  ,
    0.5  , 0    , 1    ,
    0    , 0.5  , 1    ,
    1    , 0    , 0.5  ,
    1    , 0.5  , 0.5  ,
    1    , 0    , 1    ,
    0.75 , 0.5  , 1    ,
    0.25 , 0.5  , 1    ,
    0.5  , 0.25 , 1    ,
    0.5  , 0.5  , 0.75
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4 to 2 convex edge
static const std::vector<std::vector<uint>> faces =
{
    { 11 ,  9 ,  4 ,  7       },
    {  7 ,  4 ,  1 ,  3       },
    {  9 , 10 ,  6 ,  4       },
    {  4 ,  6 ,  5 ,  1       },
    {  8 ,  3 ,  1 ,  5       },
    {  2 , 11 ,  9 , 10       },
    { 12 ,  0 , 14            },
    { 12 ,  0 , 15            },
    { 13 ,  0 , 14            },
    { 13 ,  0 , 15            },
    { 12 , 14 , 15            },
    { 13 , 14 , 15            },
    {  0 , 14 , 15            },
    {  6 ,  4 ,  7 , 14 , 15  },
    {  7 ,  3 ,  8 , 13 , 14  },
    {  6 ,  5 ,  8 , 13 , 15  },
    {  7 , 11 ,  2 , 12 , 14  },
    {  6 , 10 ,  2 , 12 , 15  },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4 to 2 convex edge
static const std::vector<std::vector<uint>> polys =
{
    {  6 ,  7 , 10 , 12              },
    { 12 ,  8 ,  9 , 11              },
    { 11 , 13 , 14 , 15 , 1 , 3 , 4  },
    { 10 , 13 , 16 , 17 , 0 , 2 , 5  },
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 4 to 2 convex edge -> TODO
static const std::vector<std::vector<bool>> winding =
{
    { true, true, true, true                   },
    { true, true, true, true                   },
    { true, true, true, true, true, true, true },
    { true, true, true, true, true, true, true },
};
} // namespace ConvexEdge


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Half_ConvexEdge
{

// 2 to 1 convex edge
static const std::vector<double> verts =
{
    0.5  , 0.5  , 1    ,
    0    , 0    , 0.5  ,
    0.5  , 0.5  , 0.75 ,
    0    , 0    , 1    ,
    0.5  , 0    , 0.5  ,
    0    , 0.5  , 0.5  ,
    0.5  , 0.5  , 0.5  ,
    0.5  , 0    , 1    ,
    0    , 0.5  , 1    ,
    0.25 , 0.5  , 1    ,
    0.5  , 0.25 , 1
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 2 to 1 convex edge
static const std::vector<std::vector<uint>> faces =
{
    { 6 ,  4 ,  7 , 10 ,  2 },
    { 7 ,  4 ,  1 ,  3      },
    { 7 ,  3 ,  8 ,  9 , 10 },
    { 4 ,  6 ,  5 ,  1      },
    { 8 ,  3 ,  1 ,  5      },
    { 6 ,  5 ,  8 ,  9 ,  2 },
    { 0 , 10 ,  2           },
    { 9 , 10 ,  2           },
    { 9 ,  0 , 10           },
    { 9 ,  0 ,  2           },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 2 to 1 convex edge
static const std::vector<std::vector<uint>> polys =
{
    { 7 , 0 , 2 , 5 , 1 , 3 , 4 },
    { 6 , 8 , 9 , 7             },
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// 2 to 1 convex edge
static const std::vector<std::vector<bool>> winding =
{
    { true, true, true, true, true, true, true },
    { true, true, true, true                   },
};

} // namespace Half_ConvexEdge

} // namespace HexTransitions
} // namespace cinolib

#ifndef  CINO_STATIC_LIB
#include "hex_transition_schemes.cpp"
#endif

#endif // CINO_HEX_TRANSITION_SCHEMES_H
