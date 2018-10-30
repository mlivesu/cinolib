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
#include <cinolib/export_hexahedra.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_hexahedra(Polyhedralmesh<M,V,E,F,P>   m_in,
                      Hexmesh<M,V,E,F,P>        & m_out)
{
    // remove all non hexa elements from m_in
    std::vector<uint> non_hexa;
    for(uint pid=0; pid<m_in.num_polys(); ++pid)
    {
        if(!m_in.poly_is_hexahedron(pid)) non_hexa.push_back(pid);
    }
    m_in.polys_remove(non_hexa);

    // copy verts/faces/polys into m_out
    m_out.clear();
    for(uint vid=0; vid<m_in.num_verts(); ++vid) m_out.vert_add(m_in.vert(vid));
    for(uint fid=0; fid<m_in.num_faces(); ++fid) m_out.face_add(m_in.face_verts_id(fid));
    for(uint pid=0; pid<m_in.num_polys(); ++pid)
    {
        std::vector<uint> f;
        std::vector<bool> w;
        for(uint fid : m_in.adj_p2f(pid))
        {
            f.push_back(fid);
            w.push_back(m_in.poly_face_is_CCW(pid,fid));
        }
        m_out.poly_add(f,w);
    }
    m_out.poly_fix_orientation();
}

}

