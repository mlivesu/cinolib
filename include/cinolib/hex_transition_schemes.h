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

#include <vector>
#include <sys/types.h>

/* Implementation of the convertion schemes to transform an adaptively refined grid
 * with hanging nodes into a conforming all hexahedral mesh. These schemes are based
 * on ideas contained in:
 *
 *     Optimal Dual Schemes for Adaptive Grid Based Hexmeshing
 *     Marco Livesu, Luca Pitzalis, Gianmarco Cherchi
 *     ACM Transactions on Graphics, 2021
 *
 * Schemes are defined in the primal (grid) mesh, and are meant to regularize the
 * valence of hanging nodes, ensuring that once dualized each vertex yields a hexahedron.
 *
 * The basic set of 8 schemes supports strongly balanced grids, that is, grids where
 * the difference in the amount of refinement between any cell sharing a vertex, edge
 * or face is no ore than one. Specifically, the schemes are:
 *
 *      - Flat         => building block for the basic flat transition
 *      - Flat_Convex  => allows to blend flat and convex transitions
 *      - Convex_1     => a cube with one   convex turn
 *      - Convex_2     => a cube with two   convex turns
 *      - Convex_3     => a cube with three convex turns
 *      - Edge         => building block for concave turning
 *      - Vert_center  => center  block for transitioning at concave corners
 *      - Vert_side    => lateral block for transitioning at concave corners
 *
 * A set of 5 additional schemes allows to process weakly balanced grids, where there may
 * be grid edges where cubes having three different levels of refinement are incident. The
 * additional schemes are:
 *
 *      - Edge_WB           => hybrid convex/concave turning
 *      - Vert_center_WB_1  => hybrid convex/concave turning for 1 out of 3 edges incident at a concave corner
 *      - Vert_center_WB_2  => hybrid convex/concave turning for 2 out of 3 edges incident at a concave corner
 *      - Vert_center_WB_3  => hybrid convex/concave turning for 3 out of 3 edges incident at a concave corner
 *      - Vert_side_WB      => hybrid convex/concave turning for the brick aside a concave corner
 *
 * For each scheme we hard coded vertices, faces and cells of the (primal) polyhedral mesh.
 * Each mesh is a 1x1x1 cube, and the vertex with id 0 is always the "application point",
 * that is, the vertex that must be used for positioning the scheme in the grid. As an example,
 * for the Flat scheme the vertex zero is the point at the middle of the 4x4 and 2x2 grids
 * that need to be connected.
 *
 * USAGE: Schemes are meant to be used as follows:
 *
 *  1 - locate type, size, and application point of the scheme in the adaptive grid
 *
 *  2 - position the scheme in your grid
 *
 *  3 - remove the standard grid element from your grid, substituting it with the
 *      scheme, also merging merging coincident vertices to topologically weld the
 *      two meshes
 *
 *  4 - dualize the so generated polyhedral mesh to obtain a full hexahedral mesh
 *
 * Steps (2,3) can be done with cinolib::hex_transition_install. Step 4 is a standard
 * dualization, and can be done with cinolib::dual_mesh
*/

namespace cinolib
{

namespace Flat
{
    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          1 ,   0 ,   0 ,
          0 ,   1 ,   0 ,
          1 ,   1 ,   0 ,
          0 ,   0 ,   1 ,
          1 ,   0 ,   1 ,
          0 ,   1 ,   1 ,
          1 ,   1 ,   1 ,
          0 , 0.3 ,   0 ,
        0.5 ,   0 , 0.5 ,
          0 ,   0 , 0.5 ,
          1 ,   0 , 0.5 ,
          0 , 0.5 ,   0 ,
        0.5 , 0.3 ,   0 ,
          1 , 0.5 ,   0 ,
        0.5 ,   0 ,   1 ,
        0.5 ,   0 ,   0 ,
          0 , 0.5 ,   1 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        { 10 ,  4 , 15 ,  9      },
        {  9 , 15 ,  5 , 11      },
        {  0 , 10 ,  9 , 16      },
        { 16 ,  9 , 11 ,  1      },
        {  6 ,  7 ,  3 ,  2      },
        { 10 ,  0 ,  8           },
        {  9 , 16 , 13           },
        { 11 ,  1 , 14           },
        {  0 , 16 , 13 ,  8      },
        { 16 ,  1 , 14 , 13      },
        { 10 ,  9 , 13 ,  8      },
        {  9 , 11 , 14 , 13      },
        {  4 , 15 , 17           },
        {  8 , 13 , 12           },
        { 17 , 12 ,  2 ,  6      },
        { 12 , 13 , 14 ,  3 ,  2 },
        {  8 , 10 ,  4 , 17 , 12 },
        { 13 ,  9 , 15 , 17 , 12 },
        {  5 , 15 , 17 ,  6 ,  7 },
        { 11 ,  5 ,  7 ,  3 , 14 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  5 ,  6 ,  8 , 10 ,  2              },
        {  6 ,  7 ,  9 , 11 ,  3              },
        { 10 , 13 , 12 , 16 , 17 ,  0         },
        { 11 , 17 , 15 , 19 , 18 , 14 , 4 , 1 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , false                       },
        { false , true  , false , true  , false                       },
        { false , false , true  , true  , false , false               },
        { false , true  , false , false , false , false , true , false},
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Flat_Convex
{
    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          1 ,   0 ,   0 ,
          0 ,   1 ,   0 ,
          1 ,   1 ,   0 ,
          0 ,   0 ,   1 ,
          1 ,   0 ,   1 ,
          0 ,   1 ,   1 ,
          1 ,   1 ,   1 ,
          0 , 0.3 ,   0 ,
        0.5 ,   0 , 0.5 ,
          0 ,   0 , 0.5 ,
          1 ,   0 , 0.5 ,
          0 , 0.5 ,   0 ,
        0.5 , 0.3 ,   0 ,
          1 , 0.5 ,   0 ,
        0.5 ,   0 ,   1 ,
        0.5 ,   0 ,   0 ,
          0 , 0.5 ,   1 ,
        0.5 ,   1 ,   1 ,
          1 ,   1 , 0.5 ,
          1 , 0.5 ,   1 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        { 10 ,  4 , 15 ,  9           },
        {  9 , 15 ,  5 , 11           },
        {  0 , 10 ,  9 , 16           },
        { 16 ,  9 , 11 ,  1           },
        { 19 ,  3 , 14 , 11 ,  5 , 20 },
        { 10 ,  0 ,  8                },
        {  9 , 16 , 13                },
        { 11 ,  1 , 14                },
        {  0 , 16 , 13 ,  8           },
        { 16 ,  1 , 14 , 13           },
        { 10 ,  9 , 13 ,  8           },
        {  9 , 11 , 14 , 13           },
        {  4 , 15 , 17                },
        {  8 , 13 , 12                },
        { 17 , 12 ,  2 ,  6           },
        { 12 , 13 , 14 ,  3 ,  2      },
        {  8 , 10 ,  4 , 17 , 12      },
        { 13 ,  9 , 15 , 17 , 12      },
        { 20 ,  5 , 15 , 17 ,  6 , 18 },
        { 19 ,  3 ,  2 ,  6 , 18      },
        { 18 , 19 , 20                },
        {  7 , 19 , 20                },
        { 18 ,  7 , 20                },
        { 18 , 19 ,  7                },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  5 ,  6 ,  8 , 10 ,  2                    },
        {  6 ,  7 ,  9 , 11 ,  3                    },
        { 10 , 13 , 12 , 16 , 17 ,  0               },
        { 11 , 17 , 15 ,  4 , 18 , 14 , 19 , 1 , 20 },
        { 20 , 21 , 22 , 23                         },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , false                                },
        { false , true  , false , true  , false                                },
        { false , false , true  , true  , false , false                        },
        { false , true  , false , false , false , false , true  , false , false},
        { true  , false , false , false                                        },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Convex_1
{

    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          1 ,   1 ,   1 ,
          1 ,   1 ,   0 ,
          1 ,   0 ,   1 ,
          0 ,   1 ,   1 ,
          0 ,   0 ,   1 ,
          1 ,   0 ,   0 ,
          0 ,   1 ,   0 ,
          0 , 0.5 ,   0 ,
        0.5 ,   0 ,   0 ,
          0 ,   0 , 0.5 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        { 6 , 3 ,  1 , 2      },
        { 3 , 5 ,  4 , 1      },
        { 7 , 2 ,  1 , 4      },
        { 8 , 0 ,  9          },
        { 8 , 0 , 10          },
        { 8 , 9 , 10          },
        { 0 , 9 , 10 ,        },
        { 5 , 3 ,  6 , 9 , 10 },
        { 6 , 2 ,  7 , 8 ,  9 },
        { 5 , 4 ,  7 , 8 , 10 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        { 6 , 3 , 4 , 5             },
        { 5 , 7 , 8 , 9 , 0 , 1 , 2 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { true  , false , true  , false                         },
        { true  , false , false , true  , false , false , false },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Convex_2
{
    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          1 ,   1 ,   1 ,
          1 ,   1 ,   0 ,
          1 ,   0 ,   1 ,
          0 ,   1 ,   1 ,
          0 ,   0 ,   1 ,
          1 ,   0 ,   0 ,
          0 ,   1 ,   0 ,
          0 , 0.5 ,   0 ,
        0.5 ,   0 ,   0 ,
          0 ,   0 , 0.5 ,
          0 , 0.5 ,   1 ,
        0.5 ,   1 ,   1 ,
          0 ,   1 , 0.5 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  6 ,  3 ,  1 ,  2           },
        { 13 ,  7 ,  8 , 10 ,  5 , 11 },
        { 12 ,  1 ,  3 ,  5 , 11      },
        {  8 ,  0 ,  9                },
        {  8 ,  0 , 10                },
        {  8 ,  9 , 10                },
        {  0 ,  9 , 10                },
        {  5 ,  3 ,  6 ,  9 , 10      },
        {  6 ,  2 ,  7 ,  8 ,  9      },
        { 13 ,  7 ,  2 ,  1 , 12      },
        { 11 , 12 , 13                },
        {  4 , 12 , 13                },
        { 11 ,  4 , 13                },
        { 11 , 12 , 4                 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  6 ,  3 ,  4 ,  5                  },
        {  5 ,  7 ,  8 ,  1 , 0 , 2 , 9 , 10 },
        { 10 , 11 , 12 , 13                  },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { true  , false , true  , false                                },
        { true  , false , false , true  , false , false , false , true },
        { false , true  , true  , true                                 },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Convex_3
{
    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          1 ,   1 ,   1 ,
          1 ,   1 ,   0 ,
          1 ,   0 ,   1 ,
          0 ,   1 ,   1 ,
          0 ,   0 ,   1 ,
          1 ,   0 ,   0 ,
          0 ,   1 ,   0 ,
          0 , 0.5 ,   0 ,
        0.5 ,   0 ,   0 ,
          0 ,   0 , 0.5 ,
          0 , 0.5 ,   1 ,
        0.5 ,   1 ,   1 ,
          0 ,   1 , 0.5 ,
          1 ,   1 , 0.5 ,
          1 , 0.5 ,   0 ,
        0.5 ,   1 ,   0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        { 16 ,  7 ,  8 ,  9 ,  6 , 15 },
        { 13 ,  7 ,  8 , 10 ,  5 , 11 },
        { 12 ,  1 ,  3 ,  5 , 11      },
        {  8 ,  0 ,  9                },
        {  8 ,  0 , 10                },
        {  8 ,  9 , 10                },
        {  0 ,  9 , 10                },
        {  5 ,  3 ,  6 ,  9 , 10      },
        { 15 ,  6 ,  3 ,  1 , 14      },
        { 14 ,  1 , 12 , 13 ,  7 , 16 },
        { 11 , 12 , 13                },
        {  4 , 12 , 13                },
        { 11 ,  4 , 13                },
        { 11 , 12 ,  4                },
        { 14 , 15 , 16                },
        {  2 , 15 , 16                },
        { 14 ,  2 , 16                },
        { 14 , 15 ,  2                },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  6 ,  3 ,  4 ,  5                       },
        {  5 ,  7 ,  0 ,  1 , 8 , 2 , 9 , 10 , 14 },
        { 10 , 11 , 12 , 13                       },
        { 14 , 15 , 16 , 17                       },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { true  , false , true  , false                                        },
        { true  , false , false , true  , false , false , false , true  , true },
        { false , true  , true  , true                                         },
        { false , true  , true  , true                                         },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Edge
{
    static const std::vector<double> verts =
    {
         0 ,    0 ,   0 ,
         1 ,    0 ,   1 ,
         1 ,    1 , 0.5 ,
         1 ,    1 ,   0 ,
         1 ,    1 ,   1 ,
         1 ,    0 , 0.3 ,
         1 ,  0.5 ,   0 ,
         1 ,    0 , 0.5 ,
         1 ,    0 ,   0 ,
         0 ,  0.5 ,   1 ,
         0 ,    0 ,   1 ,
         0 ,    1 , 0.5 ,
         0 ,  0.5 , 0.5 ,
         0 ,    0 , 0.5 ,
         0 ,    1 ,   0 ,
         0 ,  0.5 ,   0 ,
         0 ,    1 ,   1 ,
        0.5 ,   1 ,   0 ,
        0.5 , 0.5 ,   0 ,
        0.5 ,   0 ,   0 ,
        0.3 ,   0 ,   1 ,
        0.3 ,   0 , 0.5 ,
        0.5 ,   0 , 0.3 ,
        0.5 ,   0 ,   1 ,
        0.5 ,   1 ,   1 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  5 ,  6 ,  3 ,  2 ,  7     },
        {  6 ,  8 ,  5               },
        {  2 ,  7 ,  1 ,  4          },
        {  9 , 12 , 11 , 16          },
        { 10 , 13 , 12 ,  9          },
        { 12 , 15 , 14 , 11          },
        { 13 ,  0 , 15 , 12          },
        { 15 , 18 , 17 , 14          },
        {  0 , 19 , 18 , 15          },
        { 18 ,  6 ,  3 , 17          },
        { 19 ,  8 ,  6 , 18          },
        {  9 , 10 , 20               },
        { 12 , 13 , 21               },
        { 18 , 19 , 22               },
        { 16 , 11 , 24               },
        {  3 , 17 ,  2               },
        { 10 , 13 , 21 , 20          },
        { 19 ,  8 ,  5 , 22          },
        {  9 , 12 , 21 , 20          },
        { 18 ,  6 ,  5 , 22          },
        { 20 , 21 , 23               },
        {  5 , 22 ,  7               },
        { 22 , 21 , 13 ,  0 , 19     },
        { 22 , 21 , 12 , 15 , 18     },
        { 22 , 21 , 23 ,  1 ,  7     },
        { 24 , 11 , 12 , 21 , 23     },
        {  2 , 17 , 18 , 22 ,  7     },
        { 16 , 24 , 23 , 20 ,  9     },
        { 23 , 24 ,  4 ,  1          },
        { 24 , 11 , 14 , 17 ,  2 , 4 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  4 , 11 , 12 , 16 , 18                     },
        {  6 ,  8 , 12 , 13 , 22 , 23                },
        { 10 , 13 ,  1 , 17 , 19                     },
        {  3 , 27 , 18 , 20 , 14 , 25                },
        {  9 ,  0 , 19 , 21 , 15 , 26                },
        {  5 ,  7 , 25 , 26 , 28 ,  2 , 29 , 24 , 23 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , false                                },
        { false , false , true  , false , true  , false                        },
        { false , true  , false , true  , false                                },
        { false , false , true  , true  , false , false                        },
        { false , true  , true  , false , true  , true                         },
        { false , false , true  , false , false , false , false , false , true },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_center
{
    static const std::vector<double> verts =
    {
          0 ,   0 ,   0 ,
          0 ,   1 , 0.5 ,
          0 ,   1 ,   1 ,
          0 , 0.5 ,   0 ,
          0 , 0.5 , 0.5 ,
          0 , 0.5 ,   1 ,
          0 ,   1 ,   0 ,
          0 ,   0 , 0.5 ,
          0 ,   0 ,   1 ,
        0.5 ,   0 ,   0 ,
        0.5 ,   0 , 0.5 ,
        0.5 ,   0 ,   1 ,
          1 ,   0 ,   0 ,
          1 ,   0 , 0.5 ,
          1 ,   0 ,   1 ,
        0.5 , 0.5 ,   0 ,
          1 , 0.5 ,   0 ,
        0.5 ,   1 ,   0 ,
          1 ,   1 ,   0 ,
          1 ,   1 ,   1 ,
        0.3 ,   1 ,   1 ,
        0.5 ,   1 ,   1 ,
          1 , 0.3 ,   1 ,
          1 , 0.5 ,   1 ,
          1 ,   1 , 0.3 ,
          1 ,   1 , 0.5 ,
        0.3 , 0.5 ,   1 ,
          1 , 0.3 , 0.5 ,
        0.5 ,   1 , 0.3 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  6 ,  1 ,  4 ,  3           },
        {  1 ,  2 ,  5 ,  4           },
        {  3 ,  4 ,  7 ,  0           },
        {  4 ,  5 ,  8 ,  7           },
        {  0 ,  7 , 10 ,  9           },
        {  7 ,  8 , 11 , 10           },
        {  9 , 10 , 13 , 12           },
        { 10 , 11 , 14 , 13           },
        { 16 , 18 , 17 , 15           },
        { 16 , 15 ,  9 , 12           },
        { 15 ,  3 ,  0 ,  9           },
        { 15 , 17 ,  6 ,  3           },
        {  5 , 26 , 20 ,  2           },
        { 14 , 22 , 27 , 13           },
        { 18 , 24 , 28 , 17           },
        {  5 , 26 ,  4                },
        {  2 , 20 ,  1                },
        { 10 , 27 , 13                },
        { 14 , 22 , 11                },
        { 15 , 28 , 17                },
        { 16 , 24 , 18                },
        {  1 , 20 , 26 ,  4           },
        { 11 , 22 , 27 , 10           },
        { 16 , 24 , 28 , 15           },
        { 20 , 21 , 26                },
        { 22 , 23 , 27                },
        { 24 , 25 , 28                },
        { 21 , 20 ,  1 ,  6 , 17 , 28 },
        { 23 , 22 , 11 ,  8 ,  5 , 26 },
        { 25 , 24 , 16 , 12 , 13 , 27 },
        { 21 , 19 , 25 , 28           },
        { 21 , 19 , 23 , 26           },
        { 23 , 19 , 25 , 27           },
        { 26 ,  4 ,  7 , 10 , 27 , 23 },
        { 27 , 10 ,  9 , 15 , 28 , 25 },
        { 28 , 15 ,  3 ,  4 , 26 , 21 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        { 1 , 12 , 15 , 16 , 21                     },
        { 7 , 13 , 17 , 18 , 22                     },
        { 8 , 14 , 19 , 20 , 23                     },
        { 0 , 11 , 24 , 35 , 21 , 19 , 27           },
        { 3 ,  5 , 25 , 33 , 22 , 15 , 28           },
        { 9 ,  6 , 26 , 34 , 23 , 17 , 29           },
        { 2 , 10 ,  4 , 33 , 34 , 35 , 30 , 31 , 32 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , true                                },
        { false , false , true  , true  , true                                },
        { false , false , true  , false , true                                },
        { false , false , false , false , false , false , false               },
        { false , false , false , false , false , true  , false               },
        { false , false , false , false , false , false , false               },
        { false , false , false , true  , true  , true  , true  , false, false},
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_side
{
    static const std::vector<double> verts =
    {
          0 ,   0 , 0.5 ,
          0 ,   0 ,   1 ,
          0 ,   0 ,   0 ,
        0.5 ,   0 ,   0 ,
          1 ,   0 ,   0 ,
          1 ,   0 ,   1 ,
        0.3 ,   0 ,   1 ,
        0.5 ,   0 ,   1 ,
          1 ,   0 , 0.3 ,
          1 ,   0 , 0.5 ,
        0.5 ,   0 , 0.3 ,
        0.3 , 0.5 ,   1 ,
          1 ,   1 , 0.5 ,
          0 , 0.5 ,   1 ,
          0 ,   1 , 0.5 ,
          1 , 0.5 ,   0 ,
          0 ,   1 ,   0 ,
        0.5 , 0.5 ,   0 ,
          0 , 0.5 ,   0 ,
          1 ,   1 ,   0 ,
          1 ,   1 ,   1 ,
        0.5 ,   1 ,   1 ,
          0 , 0.5 , 0.5 ,
          0 ,   1 ,   1 ,
        0.5 ,   1 ,   0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  4 ,  8 , 10 ,  3           },
        {  1 ,  6 ,  0                },
        {  8 ,  9 , 10                },
        {  7 ,  6 ,  0 ,  2 ,  3 , 10 },
        {  7 ,  5 ,  9 , 10           },
        {  6 , 11 , 22 ,  0           },
        { 11 , 13 , 22                },
        {  8 , 10 , 17 , 15           },
        { 18 , 22 , 14 , 16           },
        { 12 , 19 , 15 ,  8 ,  9      },
        { 22 , 13 , 23 , 14           },
        {  6 , 11 , 13 ,  1           },
        { 12 , 24 , 17 , 10 ,  9      },
        {  9 ,  5 , 20 , 12           },
        {  4 ,  3 , 17 , 15           },
        { 12 , 19 , 24                },
        {  7 ,  6 , 11                },
        { 21 , 14 , 23                },
        {  5 , 20 , 21 , 11 ,  7      },
        {  0 ,  1 , 13 , 22           },
        { 14 , 22 , 11 , 21           },
        { 13 , 23 , 21 , 11           },
        {  7 , 11 , 22 , 18 , 17 , 10 },
        { 24 , 12 , 20 , 21 , 14 , 16 },
        {  2 ,  0 , 22 , 18           },
        { 15 , 17 , 24 , 19           },
        {  8 ,  4 , 15                },
        { 17 , 18 , 16 , 24           },
        { 10 ,  3 , 17                },
        {  2 ,  3 , 17 , 18           },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        { 11 ,  6 ,  1 , 19 ,  5                    },
        { 10 , 21 ,  6 , 17 , 20                    },
        {  8 , 27 , 20 , 22 , 12 , 4 , 13 , 18 , 23 },
        { 29 , 24 ,  5 , 16 , 22 , 3 , 28           },
        { 25 ,  7 ,  9 , 15 , 12 , 2                },
        { 14 , 26 , 28 ,  0 ,  7                    },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { true  , false , false , true  , false                                },
        { true  , false , true  , true  , false                                },
        { true  , true  , true  , true  , false , false , false , true  , false},
        { false , true  , true  , false , false , true  , true                 },
        { true  , true  , false , true  , true  , true                         },
        { true  , true  , false , true  , false                                },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Edge_WB
{
    static const std::vector<double> verts =
    {
           0 ,    0 ,    0 ,
           1 ,    0 ,    1 ,
           1 ,    1 ,  0.5 ,
           1 ,    1 ,    0 ,
           1 ,    1 ,    1 ,
           1 ,    0 ,  0.3 ,
           1 ,  0.5 ,    0 ,
           1 ,    0 ,  0.5 ,
           1 ,    0 ,    0 ,
           0 ,  0.5 ,    1 ,
           0 ,    0 ,    1 ,
           0 ,    1 ,  0.5 ,
           0 ,  0.5 ,  0.5 ,
           0 ,    0 ,  0.5 ,
           0 ,    1 ,    0 ,
           0 ,  0.5 ,    0 ,
           0 ,    1 ,    1 ,
         0.5 ,    1 ,    0 ,
         0.5 ,  0.5 ,    0 ,
         0.5 ,    0 ,    0 ,
         0.3 ,    0 ,    1 ,
         0.3 ,    0 ,  0.5 ,
         0.5 ,    0 ,  0.3 ,
         0.5 ,    0 ,    1 ,
         0.5 ,    1 ,    1 ,
           0 ,  0.5 , 0.25 ,
           0 , 0.75 ,    0 ,
           0 , 0.25 ,    0 ,
        0.25 ,  0.5 ,    0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  5 ,  6 ,  3 ,  2 ,  7      },
        {  6 ,  8 ,  5                },
        {  2 ,  7 ,  1 ,  4           },
        {  9 , 12 , 11 , 16           },
        { 10 , 13 , 12 ,  9           },
        { 26 , 14 , 11 , 12 , 25      },
        { 25 , 26 , 28                },
        { 28 , 18 , 17 , 14 , 26      },
        { 15 , 26 , 28                },
        { 18 ,  6 ,  3 , 17           },
        { 19 ,  8 ,  6 , 18           },
        {  9 , 10 , 20                },
        { 12 , 13 , 21                },
        { 18 , 19 , 22                },
        { 16 , 11 , 24                },
        {  3 , 17 ,  2                },
        { 10 , 13 , 21 , 20           },
        { 19 ,  8 ,  5 , 22           },
        {  9 , 12 , 21 , 20           },
        { 18 ,  6 ,  5 , 22           },
        { 20 , 21 , 23                },
        {  5 , 22 ,  7                },
        { 22 , 21 , 13 ,  0 , 19      },
        { 25 , 26 , 15                },
        { 22 , 21 , 23 ,  1 ,  7      },
        { 24 , 11 , 12 , 21 , 23      },
        {  2 , 17 , 18 , 22 ,  7      },
        { 16 , 24 , 23 , 20 ,  9      },
        { 23 , 24 ,  4 ,  1           },
        { 24 , 11 , 14 , 17 ,  2 ,  4 },
        { 25 , 27 , 28                },
        { 15 , 27 , 28                },
        { 25 , 15 , 28                },
        { 25 , 27 , 15                },
        { 25 , 12 , 13 ,  0 , 27      },
        { 27 ,  0 , 19 , 18 , 28      },
        { 28 , 18 , 22 , 21 , 12 , 25 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        { 4 , 11 , 12 , 16 , 18 },
        { 6 , 8 , 32 , 23 },
        { 10 , 13 , 1 , 17 , 19 },
        { 3 , 27 , 18 , 20 , 14 , 25 },
        { 9 , 0 , 19 , 21 , 15 , 26 },
        { 5 , 7 , 25 , 26 , 28 , 2 , 29 , 24 , 36 , 6 },
        { 30 , 31 , 32 , 33 },
        { 34 , 35 , 12 , 13 , 22 , 36 , 30 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , false },
        { false , true  , true  , true  },
        { false , true  , false , true  , false },
        { false , false , true  , true  , false , false },
        { false , true  , true  , false , true  , true  },
        { false , false , true  , false , false , false , false , false , true  , true  },
        { true  , false , false , false },
        { false , false , true  , false , true  , false , false },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_center_WB_1
{
    static const std::vector<double> verts =
    {
           0 ,    0 ,    0 ,
           0 ,    1 ,  0.5 ,
           0 ,    1 ,    1 ,
           0 ,  0.5 ,    0 ,
           0 ,  0.5 ,  0.5 ,
           0 ,  0.5 ,    1 ,
           0 ,    1 ,    0 ,
           0 ,    0 ,  0.5 ,
           0 ,    0 ,    1 ,
         0.5 ,    0 ,    0 ,
         0.5 ,    0 ,  0.5 ,
         0.5 ,    0 ,    1 ,
           1 ,    0 ,    0 ,
           1 ,    0 ,  0.5 ,
           1 ,    0 ,    1 ,
         0.5 ,  0.5 ,    0 ,
           1 ,  0.5 ,    0 ,
         0.5 ,    1 ,    0 ,
           1 ,    1 ,    0 ,
           1 ,    1 ,    1 ,
         0.3 ,    1 ,    1 ,
         0.5 ,    1 ,    1 ,
           1 ,  0.3 ,    1 ,
           1 ,  0.5 ,    1 ,
           1 ,    1 ,  0.3 ,
           1 ,    1 ,  0.5 ,
         0.3 ,  0.5 ,    1 ,
           1 ,  0.3 ,  0.5 ,
         0.5 ,    1 ,  0.3 ,
           0 , 0.25 ,  0.5 ,
           0 ,    0 , 0.25 ,
           0 ,    0 , 0.75 ,
        0.25 ,    0 ,  0.5 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  6 ,  1 ,  4 ,  3                },
        {  1 ,  2 ,  5 ,  4                },
        { 30 ,  0 ,  3 ,  4 , 29           },
        { 29 , 30 , 32                     },
        { 32 , 10 ,  9 ,  0 , 30           },
        {  7 , 30 , 32                     },
        {  9 , 10 , 13 , 12                },
        { 10 , 11 , 14 , 13                },
        { 16 , 18 , 17 , 15                },
        { 16 , 15 ,  9 , 12                },
        { 15 ,  3 ,  0 ,  9                },
        { 15 , 17 ,  6 ,  3                },
        {  5 , 26 , 20 ,  2                },
        { 14 , 22 , 27 , 13                },
        { 18 , 24 , 28 , 17                },
        {  5 , 26 ,  4                     },
        {  2 , 20 ,  1                     },
        { 10 , 27 , 13                     },
        { 14 , 22 , 11                     },
        { 15 , 28 , 17                     },
        { 16 , 24 , 18                     },
        {  1 , 20 , 26 ,  4                },
        { 11 , 22 , 27 , 10                },
        { 16 , 24 , 28 , 15                },
        { 20 , 21 , 26                     },
        { 22 , 23 , 27                     },
        { 24 , 25 , 28                     },
        { 21 , 20 ,  1 ,  6 , 17 , 28      },
        { 23 , 22 , 11 ,  8 ,  5 , 26      },
        { 25 , 24 , 16 , 12 , 13 , 27      },
        { 21 , 19 , 25 , 28                },
        { 21 , 19 , 23 , 26                },
        { 23 , 19 , 25 , 27                },
        { 29 , 30 ,  7                     },
        { 27 , 10 ,  9 , 15 , 28 , 25      },
        { 28 , 15 ,  3 ,  4 , 26 , 21      },
        { 29 , 31 , 32                     },
        {  7 , 31 , 32                     },
        { 29 ,  7 , 32                     },
        { 29 , 31 ,  7                     },
        { 29 ,  4 ,  5 ,  8 , 31           },
        { 31 ,  8 , 11 , 10 , 32           },
        { 32 , 10 , 27 , 23 , 26 ,  4 , 29 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  1 , 12 , 15 , 16 , 21                         },
        {  7 , 13 , 17 , 18 , 22                         },
        {  8 , 14 , 19 , 20 , 23                         },
        {  0 , 11 , 24 , 35 , 21 , 19 , 27               },
        {  3 ,  5 , 38 , 33                              },
        {  9 ,  6 , 26 , 34 , 23 , 17 , 29               },
        {  2 , 10 ,  4 , 42 , 34 , 35 , 30 , 31 , 32 , 3 },
        { 36 , 37 , 38 , 39                              },
        { 40 , 41 , 25 , 42 , 22 , 15 , 28 , 36          },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , true                                          },
        { false , false , true  , true  , true                                          },
        { false , false , true  , false , true                                          },
        { false , false , false , false , false , false , false                         },
        { false , true  , true  , true                                                  },
        { false , false , false , false , false , false , false                         },
        { false , false , false , true  , true  , true  , true  , false , false , true  },
        { true  , false , false , false                                                 },
        { false , false , false , false , false , true  , false , false                 },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_center_WB_2
{
    static const std::vector<double> verts =
    {
           0 ,    0 ,    0 ,
           0 ,    1 ,  0.5 ,
           0 ,    1 ,    1 ,
           0 ,  0.5 ,    0 ,
           0 ,  0.5 ,  0.5 ,
           0 ,  0.5 ,    1 ,
           0 ,    1 ,    0 ,
           0 ,    0 ,  0.5 ,
           0 ,    0 ,    1 ,
         0.5 ,    0 ,    0 ,
         0.5 ,    0 ,  0.5 ,
         0.5 ,    0 ,    1 ,
           1 ,    0 ,    0 ,
           1 ,    0 ,  0.5 ,
           1 ,    0 ,    1 ,
         0.5 ,  0.5 ,    0 ,
           1 ,  0.5 ,    0 ,
         0.5 ,    1 ,    0 ,
           1 ,    1 ,    0 ,
           1 ,    1 ,    1 ,
         0.3 ,    1 ,    1 ,
         0.5 ,    1 ,    1 ,
           1 ,  0.3 ,    1 ,
           1 ,  0.5 ,    1 ,
           1 ,    1 ,  0.3 ,
           1 ,    1 ,  0.5 ,
         0.3 ,  0.5 ,    1 ,
           1 ,  0.3 ,  0.5 ,
         0.5 ,    1 ,  0.3 ,
           0 , 0.25 ,  0.5 ,
           0 ,    0 , 0.25 ,
           0 ,    0 , 0.75 ,
        0.25 ,    0 ,  0.5 ,
         0.5 ,    0 , 0.25 ,
        0.25 ,    0 ,    0 ,
        0.75 ,    0 ,    0 ,
         0.5 , 0.25 ,    0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  6 ,  1 ,  4 ,  3                },
        {  1 ,  2 ,  5 ,  4                },
        { 30 ,  0 ,  3 ,  4 , 29           },
        { 29 , 30 , 32                     },
        { 36 , 15 ,  3 ,  0 , 34           },
        {  7 , 30 , 32                     },
        { 33 , 34 , 36                     },
        { 10 , 11 , 14 , 13                },
        { 16 , 18 , 17 , 15                },
        {  9 , 34 , 36                     },
        { 34 ,  0 , 30 , 32 , 10 , 33      },
        { 15 , 17 ,  6 ,  3                },
        {  5 , 26 , 20 ,  2                },
        { 14 , 22 , 27 , 13                },
        { 18 , 24 , 28 , 17                },
        {  5 , 26 ,  4                     },
        {  2 , 20 ,  1                     },
        { 10 , 27 , 13                     },
        { 14 , 22 , 11                     },
        { 15 , 28 , 17                     },
        { 16 , 24 , 18                     },
        {  1 , 20 , 26 ,  4                },
        { 11 , 22 , 27 , 10                },
        { 16 , 24 , 28 , 15                },
        { 20 , 21 , 26                     },
        { 22 , 23 , 27                     },
        { 24 , 25 , 28                     },
        { 21 , 20 ,  1 ,  6 , 17 , 28      },
        { 23 , 22 , 11 ,  8 ,  5 , 26      },
        { 25 , 24 , 16 , 12 , 13 , 27      },
        { 21 , 19 , 25 , 28                },
        { 21 , 19 , 23 , 26                },
        { 23 , 19 , 25 , 27                },
        { 29 , 30 ,  7                     },
        { 33 , 34 ,  9                     },
        { 28 , 15 ,  3 ,  4 , 26 , 21      },
        { 29 , 31 , 32                     },
        {  7 , 31 , 32                     },
        { 29 ,  7 , 32                     },
        { 29 , 31 ,  7                     },
        { 29 ,  4 ,  5 ,  8 , 31           },
        { 31 ,  8 , 11 , 10 , 32           },
        { 32 , 10 , 27 , 23 , 26 ,  4 , 29 },
        { 33 , 35 , 36                     },
        {  9 , 35 , 36                     },
        { 33 ,  9 , 36                     },
        { 33 , 35 ,  9                     },
        { 35 , 12 , 16 , 15 , 36           },
        { 33 , 10 , 13 , 12 , 35           },
        { 36 , 15 , 28 , 25 , 27 , 10 , 33 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  1 , 12 , 15 , 16 , 21                             },
        {  7 , 13 , 17 , 18 , 22                             },
        {  8 , 14 , 19 , 20 , 23                             },
        {  0 , 11 , 24 , 35 , 21 , 19 , 27                   },
        {  3 ,  5 , 38 , 33                                  },
        {  6 ,  9 , 45 , 34                                  },
        {  2 ,  4 , 10 , 42 , 49 , 35 , 30 , 31 , 32 , 3 , 6 },
        { 36 , 37 , 38 , 39                                  },
        { 40 , 41 , 25 , 42 , 22 , 15 , 28 , 36              },
        { 43 , 44 , 45 , 46                                  },
        { 47 , 48 , 26 , 49 , 23 , 17 , 29 , 43              },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , true                                                  },
        { false , false , true  , true  , true                                                  },
        { false , false , true  , false , true                                                  },
        { false , false , false , false , false , false , false                                 },
        { false , true  , true  , true                                                          },
        { false , true  , true  , true                                                          },
        { false , false , false , true  , true  , true  , true  , false , false , true  , true  },
        { true  , false , false , false                                                         },
        { false , false , false , false , false , true  , false , false                         },
        { true  , false , false , false                                                         },
        { false , false , false , false , false , false , false , false                         },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_center_WB_3
{
    static const std::vector<double> verts =
    {
           0 ,    0 ,    0 ,
           0 ,    1 ,  0.5 ,
           0 ,    1 ,    1 ,
           0 ,  0.5 ,    0 ,
           0 ,  0.5 ,  0.5 ,
           0 ,  0.5 ,    1 ,
           0 ,    1 ,    0 ,
           0 ,    0 ,  0.5 ,
           0 ,    0 ,    1 ,
         0.5 ,    0 ,    0 ,
         0.5 ,    0 ,  0.5 ,
         0.5 ,    0 ,    1 ,
           1 ,    0 ,    0 ,
           1 ,    0 ,  0.5 ,
           1 ,    0 ,    1 ,
         0.5 ,  0.5 ,    0 ,
           1 ,  0.5 ,    0 ,
         0.5 ,    1 ,    0 ,
           1 ,    1 ,    0 ,
           1 ,    1 ,    1 ,
         0.3 ,    1 ,    1 ,
         0.5 ,    1 ,    1 ,
           1 ,  0.3 ,    1 ,
           1 ,  0.5 ,    1 ,
           1 ,    1 ,  0.3 ,
           1 ,    1 ,  0.5 ,
         0.3 ,  0.5 ,    1 ,
           1 ,  0.3 ,  0.5 ,
         0.5 ,    1 ,  0.3 ,
           0 , 0.25 ,  0.5 ,
           0 ,    0 , 0.25 ,
           0 ,    0 , 0.75 ,
        0.25 ,    0 ,  0.5 ,
         0.5 ,    0 , 0.25 ,
        0.25 ,    0 ,    0 ,
        0.75 ,    0 ,    0 ,
         0.5 , 0.25 ,    0 ,
           0 ,  0.5 , 0.25 ,
           0 , 0.75 ,    0 ,
           0 , 0.25 ,    0 ,
        0.25 ,  0.5 ,    0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        { 37 , 39 , 40                     },
        {  1 ,  2 ,  5 ,  4                },
        { 37 ,  4 , 29 , 30 ,  0 , 39      },
        { 29 , 30 , 32                     },
        { 39 ,  0 , 34 , 36 , 15 , 40      },
        {  7 , 30 , 32                     },
        { 33 , 34 , 36                     },
        { 10 , 11 , 14 , 13                },
        { 16 , 18 , 17 , 15                },
        {  9 , 34 , 36                     },
        { 34 ,  0 , 30 , 32 , 10 , 33      },
        {  3 , 39 , 40                     },
        {  5 , 26 , 20 ,  2                },
        { 14 , 22 , 27 , 13                },
        { 18 , 24 , 28 , 17                },
        {  5 , 26 ,  4                     },
        {  2 , 20 ,  1                     },
        { 10 , 27 , 13                     },
        { 14 , 22 , 11                     },
        { 15 , 28 , 17                     },
        { 16 , 24 , 18                     },
        {  1 , 20 , 26 ,  4                },
        { 11 , 22 , 27 , 10                },
        { 16 , 24 , 28 , 15                },
        { 20 , 21 , 26                     },
        { 22 , 23 , 27                     },
        { 24 , 25 , 28                     },
        { 21 , 20 ,  1 ,  6 , 17 , 28      },
        { 23 , 22 , 11 ,  8 ,  5 , 26      },
        { 25 , 24 , 16 , 12 , 13 , 27      },
        { 21 , 19 , 25 , 28                },
        { 21 , 19 , 23 , 26                },
        { 23 , 19 , 25 , 27                },
        { 29 , 30 ,  7                     },
        { 33 , 34 ,  9                     },
        { 37 , 39 ,  3                     },
        { 29 , 31 , 32                     },
        {  7 , 31 , 32                     },
        { 29 ,  7 , 32                     },
        { 29 , 31 ,  7                     },
        { 29 ,  4 ,  5 ,  8 , 31           },
        { 31 ,  8 , 11 , 10 , 32           },
        { 32 , 10 , 27 , 23 , 26 ,  4 , 29 },
        { 33 , 35 , 36                     },
        {  9 , 35 , 36                     },
        { 33 ,  9 , 36                     },
        { 33 , 35 ,  9                     },
        { 35 , 12 , 16 , 15 , 36           },
        { 33 , 10 , 13 , 12 , 35           },
        { 36 , 15 , 28 , 25 , 27 , 10 , 33 },
        { 37 , 38 , 40                     },
        {  3 , 38 , 40                     },
        { 37 ,  3 , 40                     },
        { 37 , 38 ,  3                     },
        { 38 ,  6 ,  1 ,  4 , 37           },
        { 40 , 15 , 17 ,  6 , 38           },
        { 37 ,  4 , 26 , 21 , 28 , 15 , 40 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        {  1 , 12 , 15 , 16 , 21                                 },
        {  7 , 13 , 17 , 18 , 22                                 },
        {  8 , 14 , 19 , 20 , 23                                 },
        {  0 , 11 , 52 , 35                                      },
        {  3 ,  5 , 38 , 33                                      },
        {  6 ,  9 , 45 , 34                                      },
        {  2 ,  4 , 10 , 42 , 49 , 56 , 30 , 31 , 32 , 3 , 6 , 0 },
        { 36 , 37 , 38 , 39                                      },
        { 40 , 41 , 25 , 42 , 22 , 15 , 28 , 36                  },
        { 43 , 44 , 45 , 46                                      },
        { 47 , 48 , 26 , 49 , 23 , 17 , 29 , 43                  },
        { 50 , 51 , 52 , 53                                      },
        { 54 , 55 , 24 , 56 , 21 , 19 , 27 , 50                  },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { false , true  , false , true  , true                                                          },
        { false , false , true  , true  , true                                                          },
        { false , false , true  , false , true                                                          },
        { true  , false , false , false                                                                 },
        { false , true  , true  , true                                                                  },
        { false , true  , true  , true                                                                  },
        { false , false , false , true  , true  , true  , true  , false , false , true  , true  , false },
        { true  , false , false , false                                                                 },
        { false , false , false , false , false , true  , false , false                                 },
        { true  , false , false , false                                                                 },
        { false , false , false , false , false , false , false , false                                 },
        { false , true  , true  , true                                                                  },
        { false , false , false , false , false , false , false , true                                  },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace Vert_side_WB
{
    static const std::vector<double> verts =
    {
           0 ,    0 ,    0 ,
           0 ,    0 ,  0.5 ,
           0 ,    0 ,    1 ,
           0 ,  0.5 ,    0 ,
           0 ,    1 ,    0 ,
           0 ,    1 ,    1 ,
           0 ,  0.3 ,    1 ,
           0 ,  0.5 ,    1 ,
           0 ,    1 ,  0.3 ,
           0 ,    1 ,  0.5 ,
           0 ,  0.3 ,  0.5 ,
         0.5 ,    1 ,  0.3 ,
           1 ,  0.5 ,    1 ,
         0.5 ,    1 ,    0 ,
           1 ,  0.5 ,    0 ,
         0.5 ,    0 ,    1 ,
           1 ,    0 ,    0 ,
         0.5 ,    0 ,  0.5 ,
         0.5 ,    0 ,    0 ,
           1 ,    0 ,    1 ,
           1 ,    1 ,    1 ,
           1 ,    1 ,  0.5 ,
         0.5 ,  0.5 ,    0 ,
           1 ,    1 ,    0 ,
           1 ,    0 ,  0.5 ,
         0.5 , 0.25 ,    0 ,
        0.75 ,    0 ,    0 ,
         0.5 ,    0 , 0.25 ,
        0.25 ,    0 ,    0 ,
    };

    static const std::vector<std::vector<uint>> faces =
    {
        {  2 ,  6 , 10 ,  1                },
        {  3 ,  8 ,  4                     },
        {  6 ,  7 , 10                     },
        {  9 ,  8 ,  3 ,  0 ,  1 , 10      },
        {  7 ,  5 ,  9 , 10                },
        {  8 , 11 , 22 ,  3                },
        { 11 , 13 , 22                     },
        {  6 , 10 , 17 , 15                },
        { 25 , 27 , 28                     },
        { 12 , 19 , 15 ,  6 ,  7           },
        { 22 , 13 , 23 , 14                },
        {  8 , 11 , 13 ,  4                },
        { 12 , 24 , 17 , 10 ,  7           },
        {  7 ,  5 , 20 , 12                },
        {  2 ,  1 , 17 , 15                },
        { 12 , 19 , 24                     },
        {  9 ,  8 , 11                     },
        { 21 , 14 , 23                     },
        {  5 , 20 , 21 , 11 ,  9           },
        {  3 ,  4 , 13 , 22                },
        { 14 , 22 , 11 , 21                },
        { 13 , 23 , 21 , 11                },
        { 18 , 27 , 28                     },
        { 24 , 12 , 20 , 21 , 14 , 16      },
        { 28 ,  0 ,  1 , 17 , 27           },
        { 15 , 17 , 24 , 19                },
        {  6 ,  2 , 15                     },
        { 25 , 18 , 28                     },
        { 10 ,  1 , 17                     },
        { 28 ,  0 ,  3 , 22 , 25           },
        { 25 , 26 , 27                     },
        { 18 , 26 , 27                     },
        { 25 , 18 , 27                     },
        { 25 , 26 , 18                     },
        { 25 , 22 , 14 , 16 , 26           },
        { 26 , 16 , 24 , 17 , 27           },
        { 27 , 17 , 10 ,  9 , 11 , 22 , 25 },
    };

    static const std::vector<std::vector<uint>> polys =
    {
        { 11 ,  6 ,  1 , 19 ,  5                         },
        { 10 , 21 ,  6 , 17 , 20                         },
        {  8 , 22 , 27 , 32                              },
        { 24 , 29 ,  5 , 16 , 36 , 3 , 28 , 8            },
        { 25 ,  7 ,  9 , 15 , 12 , 2                     },
        { 14 , 26 , 28 ,  0 ,  7                         },
        { 30 , 31 , 32 , 33                              },
        { 34 , 35 , 20 , 36 , 12 , 4 , 13 , 18 , 23 , 30 },
    };

    static const std::vector<std::vector<bool>> winding =
    {
        { true  , false , true  , true  , false                                         },
        { true  , false , true  , true  , false                                         },
        { false , true  , true  , false                                                 },
        { false , true  , true  , false , false , true  , true  , true                  },
        { true  , true  , false , true  , true  , true                                  },
        { true  , true  , false , true  , false                                         },
        { false , true  , true  , true                                                  },
        { true  , true  , true  , true  , false , true  , false , true  , false , true  },
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
#endif // CINO_HEX_TRANSITION_SCHEMES_H
