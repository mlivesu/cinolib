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
