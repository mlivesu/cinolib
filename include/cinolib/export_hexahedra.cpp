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

