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
        bbox.push(vec3d(coords[ i ],
                        coords[i+1],
                        coords[i+2]));
    }

    // pre-allocate memory
    uint size = tris.size();
    drawlist.tris.reserve        (size);
    drawlist.tri_coords.reserve  (size*3);
    drawlist.tri_v_norms.reserve (size*3);
    drawlist.tri_v_colors.reserve(size*4);
    drawlist.segs.reserve        (size*2);
    drawlist.seg_coords.reserve  (size*3);
    drawlist.seg_colors.reserve  (size*4);

    for(uint i=0; i<tris.size(); i+=3)
    {
        drawlist.tris.emplace_back(i  );
        drawlist.tris.emplace_back(i+1);
        drawlist.tris.emplace_back(i+2);

        // coords
        uint  v0_ptr  = 3*tris.at( i );
        uint  v1_ptr  = 3*tris.at(i+1);
        uint  v2_ptr  = 3*tris.at(i+2);
        vec3d v0(coords.at(v0_ptr), coords.at(v0_ptr+1), coords.at(v0_ptr+2));
        vec3d v1(coords.at(v1_ptr), coords.at(v1_ptr+1), coords.at(v1_ptr+2));
        vec3d v2(coords.at(v2_ptr), coords.at(v2_ptr+1), coords.at(v2_ptr+2));
        drawlist.tri_coords.emplace_back(static_cast<float>(v0.x()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v0.y()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v0.z()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v1.x()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v1.y()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v1.z()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v2.x()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v2.y()));
        drawlist.tri_coords.emplace_back(static_cast<float>(v2.z()));

        // normals (only flat shading for a soup)
        vec3d n = (v1-v0).cross(v2-v0); n.normalize();
        drawlist.tri_v_norms.emplace_back(n.x());
        drawlist.tri_v_norms.emplace_back(n.y());
        drawlist.tri_v_norms.emplace_back(n.z());
        drawlist.tri_v_norms.emplace_back(n.x());
        drawlist.tri_v_norms.emplace_back(n.y());
        drawlist.tri_v_norms.emplace_back(n.z());
        drawlist.tri_v_norms.emplace_back(n.x());
        drawlist.tri_v_norms.emplace_back(n.y());
        drawlist.tri_v_norms.emplace_back(n.z());

        // face colors
        drawlist.tri_v_colors.emplace_back(poly_color.r);
        drawlist.tri_v_colors.emplace_back(poly_color.g);
        drawlist.tri_v_colors.emplace_back(poly_color.b);
        drawlist.tri_v_colors.emplace_back(poly_color.a);
        drawlist.tri_v_colors.emplace_back(poly_color.r);
        drawlist.tri_v_colors.emplace_back(poly_color.g);
        drawlist.tri_v_colors.emplace_back(poly_color.b);
        drawlist.tri_v_colors.emplace_back(poly_color.a);
        drawlist.tri_v_colors.emplace_back(poly_color.r);
        drawlist.tri_v_colors.emplace_back(poly_color.g);
        drawlist.tri_v_colors.emplace_back(poly_color.b);
        drawlist.tri_v_colors.emplace_back(poly_color.a);

        // edges
        drawlist.segs.emplace_back(i  );
        drawlist.segs.emplace_back(i+1);
        drawlist.segs.emplace_back(i+1);
        drawlist.segs.emplace_back(i+2);
        drawlist.segs.emplace_back(i+2);
        drawlist.segs.emplace_back(i  );
        //
        drawlist.seg_coords.emplace_back(v0.x());
        drawlist.seg_coords.emplace_back(v0.y());
        drawlist.seg_coords.emplace_back(v0.z());
        drawlist.seg_coords.emplace_back(v1.x());
        drawlist.seg_coords.emplace_back(v1.y());
        drawlist.seg_coords.emplace_back(v1.z());
        drawlist.seg_coords.emplace_back(v2.x());
        drawlist.seg_coords.emplace_back(v2.y());
        drawlist.seg_coords.emplace_back(v2.z());

        // edge colors
        drawlist.seg_colors.emplace_back(edge_color.r);
        drawlist.seg_colors.emplace_back(edge_color.g);
        drawlist.seg_colors.emplace_back(edge_color.b);
        drawlist.seg_colors.emplace_back(edge_color.a);
        drawlist.seg_colors.emplace_back(edge_color.r);
        drawlist.seg_colors.emplace_back(edge_color.g);
        drawlist.seg_colors.emplace_back(edge_color.b);
        drawlist.seg_colors.emplace_back(edge_color.a);
        drawlist.seg_colors.emplace_back(edge_color.r);
        drawlist.seg_colors.emplace_back(edge_color.g);
        drawlist.seg_colors.emplace_back(edge_color.b);
        drawlist.seg_colors.emplace_back(edge_color.a);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableTriangleSoup::draw(const float) const
{
    render(drawlist);
}

}
