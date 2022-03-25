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
#include <cinolib/drawable_triangle_soup.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableTriangleSoup::DrawableTriangleSoup(const std::vector<double> & coords,
                                           const std::vector<uint>   & tris,
                                           const Color               & poly_color,
                                           const Color               & edge_color)
{
    drawlist.draw_mode  = DRAW_TRIS | DRAW_TRI_FLAT | DRAW_TRI_FACECOLOR | DRAW_SEGS;

    for(uint i=0; i<coords.size(); i+=3)
    {
        bbox.push(vec3d(coords[i  ],
                        coords[i+1],
                        coords[i+2]));
    }

    // pre-allocate memory
    uint size = tris.size();
    drawlist.tris.resize        (size  );
    drawlist.tri_coords.resize  (size*3);
    drawlist.tri_v_norms.resize (size*3);
    drawlist.tri_v_colors.resize(size*4);
    drawlist.segs.resize        (size*2);
    drawlist.seg_coords.resize  (size*3);
    drawlist.seg_colors.resize  (size*4);

    for(uint i=0,i2=0,i3=0,i4=0; i<tris.size(); i+=3,i2=i*2,i3=i*3,i4=i*4)
    {
        drawlist.tris[i  ] = i;
        drawlist.tris[i+1] = i+1;
        drawlist.tris[i+2] = i+2;

        // coords
        uint  v0_ptr  = 3*tris[i  ];
        uint  v1_ptr  = 3*tris[i+1];
        uint  v2_ptr  = 3*tris[i+2];
        vec3d v0(coords[v0_ptr], coords[v0_ptr+1], coords[v0_ptr+2]);
        vec3d v1(coords[v1_ptr], coords[v1_ptr+1], coords[v1_ptr+2]);
        vec3d v2(coords[v2_ptr], coords[v2_ptr+1], coords[v2_ptr+2]);
        drawlist.tri_coords[i3  ] = static_cast<float>(v0.x());
        drawlist.tri_coords[i3+1] = static_cast<float>(v0.y());
        drawlist.tri_coords[i3+2] = static_cast<float>(v0.z());
        drawlist.tri_coords[i3+3] = static_cast<float>(v1.x());
        drawlist.tri_coords[i3+4] = static_cast<float>(v1.y());
        drawlist.tri_coords[i3+5] = static_cast<float>(v1.z());
        drawlist.tri_coords[i3+6] = static_cast<float>(v2.x());
        drawlist.tri_coords[i3+7] = static_cast<float>(v2.y());
        drawlist.tri_coords[i3+8] = static_cast<float>(v2.z());

        // normals (only flat shading for a soup)
        vec3d n = (v1-v0).cross(v2-v0); n.normalize();
        drawlist.tri_v_norms[i3  ] = n.x();
        drawlist.tri_v_norms[i3+1] = n.y();
        drawlist.tri_v_norms[i3+2] = n.z();
        drawlist.tri_v_norms[i3+3] = n.x();
        drawlist.tri_v_norms[i3+4] = n.y();
        drawlist.tri_v_norms[i3+5] = n.z();
        drawlist.tri_v_norms[i3+6] = n.x();
        drawlist.tri_v_norms[i3+7] = n.y();
        drawlist.tri_v_norms[i3+8] = n.z();

        // face colors
        drawlist.tri_v_colors[i4   ] = poly_color.r;
        drawlist.tri_v_colors[i4+ 1] = poly_color.g;
        drawlist.tri_v_colors[i4+ 2] = poly_color.b;
        drawlist.tri_v_colors[i4+ 3] = poly_color.a;
        drawlist.tri_v_colors[i4+ 4] = poly_color.r;
        drawlist.tri_v_colors[i4+ 5] = poly_color.g;
        drawlist.tri_v_colors[i4+ 6] = poly_color.b;
        drawlist.tri_v_colors[i4+ 7] = poly_color.a;
        drawlist.tri_v_colors[i4+ 8] = poly_color.r;
        drawlist.tri_v_colors[i4+ 9] = poly_color.g;
        drawlist.tri_v_colors[i4+10] = poly_color.b;
        drawlist.tri_v_colors[i4+11] = poly_color.a;

        // edges
        drawlist.segs[i2  ] = i;
        drawlist.segs[i2+1] = i+1;
        drawlist.segs[i2+2] = i+1;
        drawlist.segs[i2+3] = i+2;
        drawlist.segs[i2+4] = i+2;
        drawlist.segs[i2+5] = i;
        //
        drawlist.seg_coords[i3  ] = v0.x();
        drawlist.seg_coords[i3+1] = v0.y();
        drawlist.seg_coords[i3+2] = v0.z();
        drawlist.seg_coords[i3+3] = v1.x();
        drawlist.seg_coords[i3+4] = v1.y();
        drawlist.seg_coords[i3+5] = v1.z();
        drawlist.seg_coords[i3+6] = v2.x();
        drawlist.seg_coords[i3+7] = v2.y();
        drawlist.seg_coords[i3+8] = v2.z();

        // edge colors
        drawlist.seg_colors[i4   ] = edge_color.r;
        drawlist.seg_colors[i4+ 1] = edge_color.g;
        drawlist.seg_colors[i4+ 2] = edge_color.b;
        drawlist.seg_colors[i4+ 3] = edge_color.a;
        drawlist.seg_colors[i4+ 4] = edge_color.r;
        drawlist.seg_colors[i4+ 5] = edge_color.g;
        drawlist.seg_colors[i4+ 6] = edge_color.b;
        drawlist.seg_colors[i4+ 7] = edge_color.a;
        drawlist.seg_colors[i4+ 8] = edge_color.r;
        drawlist.seg_colors[i4+ 9] = edge_color.g;
        drawlist.seg_colors[i4+10] = edge_color.b;
        drawlist.seg_colors[i4+11] = edge_color.a;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableTriangleSoup::draw(const float) const
{
    render(drawlist);
}

}
