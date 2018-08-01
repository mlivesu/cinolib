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
#include <cinolib/drawable_vector_field.h>

namespace cinolib
{

CINO_INLINE
DrawableVectorField::DrawableVectorField()
{
    arrow_color = Color::RED();
    arrow_size  = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableVectorField::DrawableVectorField(const uint size) : VectorField(size)
{
    pos.resize(size);
    arrow_color = Color::RED();
    arrow_size  = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableVectorField::DrawableVectorField(const std::vector<vec3d> & data,
                                         const std::vector<vec3d> & pos)
    : VectorField(data)
    , pos(pos)
{
    arrow_color = Color::RED();
    arrow_size  = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
DrawableVectorField::DrawableVectorField(const AbstractMesh<M,V,E,P> &m, const bool field_on_poly)
{
    if(field_on_poly)
    {
        setZero(3*m.num_polys());
        pos.resize(m.num_polys());
        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            pos.at(pid) = m.poly_centroid(pid);
        }
    }
    else
    {
        setZero(3*m.num_verts());
        pos.resize(m.num_verts());
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            pos.at(vid) = m.vert(vid);
        }
    }
    arrow_color = Color::RED();
    arrow_size  = 0.5 * m.edge_avg_length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableVectorField::draw(const float) const
{
    for(uint i=0; i<pos.size(); ++i)
    {
        vec3d tip  = pos.at(i) + arrow_size * vec_at(i);
        arrow<vec3d>(pos.at(i), tip, arrow_size*0.1, arrow_color.rgba);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableVectorField::set_arrow_color(const Color &c)
{
    arrow_color = c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableVectorField::set_arrow_size(float s)
{
    arrow_size = s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Color DrawableVectorField::get_arrow_color() const
{
    return arrow_color;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float DrawableVectorField::get_arrow_size() const
{
    return arrow_size;
}

}
