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

template<class Mesh>
CINO_INLINE
DrawableVectorField<Mesh>::DrawableVectorField()
{
    m_ptr         = nullptr;
    arrow_color   = Color::RED();
    arrow_size    = 0.5;
    field_on_poly = true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
DrawableVectorField<Mesh>::DrawableVectorField(const Mesh &m, const bool field_on_poly)
: VectorField()
, field_on_poly(field_on_poly)
{
    uint size = (field_on_poly) ? 3*m.num_polys() : 3*m.num_verts();
    this->setZero(size);
    m_ptr = &m;
    set_arrow_color(Color::RED());
    set_arrow_size(0.5);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void DrawableVectorField<Mesh>::draw(const float) const
{
    if(m_ptr == nullptr) return;

    if(field_on_poly) // per polygon/polyhedron field
    {
        for(uint pid=0; pid<m_ptr->num_polys(); ++pid)
        {
            vec3d base = m_ptr->poly_centroid(pid);
            vec3d tip  = base + arrow_length * vec_at(pid);
            arrow<vec3d>(base, tip, arrow_thicknes, arrow_color.rgba);
        }
    }
    else // per vertex field
    {
        for(uint vid=0; vid<m_ptr->num_verts(); ++vid)
        {
            vec3d base = m_ptr->vert(vid);
            vec3d tip  = base + arrow_length * vec_at(vid);
            arrow<vec3d>(base, tip, arrow_thicknes, arrow_color.rgba);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void DrawableVectorField<Mesh>::set_arrow_color(const Color &c)
{
    arrow_color = c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void DrawableVectorField<Mesh>::set_arrow_size(float s)
{
    if (m_ptr)
    {
        arrow_size     = s;
        arrow_length   = m_ptr->edge_avg_length() * s;
        arrow_thicknes = arrow_length * 0.1;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Color DrawableVectorField<Mesh>::get_arrow_color() const
{
    return arrow_color;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
float DrawableVectorField<Mesh>::get_arrow_size() const
{
    return arrow_size;
}

}
