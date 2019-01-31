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
void export_hexahedra(const Polyhedralmesh<M,V,E,F,P>       m_in,
                            Hexmesh<M,V,E,F,P>            & m_out,
                            std::unordered_map<uint,uint> & v_map) // (m_in to m_out)
{
    m_out.clear();
    v_map.clear();

    // add vertices incident to at least one hexa
    for(uint vid=0; vid<m_in.num_verts(); ++vid)
    {
        bool touches_hexa = false;
        for(uint pid : m_in.adj_v2p(vid))
        {
            if(m_in.poly_is_hexahedron(pid))
            {
                touches_hexa = true;
                break;
            }
        }
        if(touches_hexa)
        {
            uint fresh_id = v_map.size();
            v_map[vid] = fresh_id;
            m_out.vert_add(m_in.vert(vid));
        }
    }

    // add faces incident to at least one hexa
    std::unordered_map<uint,uint> f_map;
    for(uint fid=0; fid<m_in.num_faces(); ++fid)
    {
        bool touches_hexa = false;
        for(uint pid : m_in.adj_f2p(fid))
        {
            if(m_in.poly_is_hexahedron(pid))
            {
                touches_hexa = true;
                break;
            }
        }
        if(touches_hexa)
        {
            uint fresh_id = f_map.size();
            f_map[fid] = fresh_id;
            std::vector<uint> f_verts;
            for(uint vid : m_in.adj_f2v(fid)) f_verts.push_back(v_map.at(vid));
            m_out.face_add(f_verts);
        }
    }

    // make hexa
    for(uint pid=0; pid<m_in.num_polys(); ++pid)
    {
        if(m_in.poly_is_hexahedron(pid))
        {
            std::vector<uint> f;
            std::vector<bool> w;
            for(uint fid : m_in.adj_p2f(pid))
            {
                f.push_back(f_map.at(fid));
                w.push_back(m_in.poly_face_is_CCW(pid,fid));
            }
            m_out.poly_add(f,w);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_hexahedra(const Polyhedralmesh<M,V,E,F,P>   m_in,
                            Hexmesh<M,V,E,F,P>        & m_out)
{
    std::unordered_map<uint,uint> v_map;
    export_hexahedra(m_in, m_out, v_map);
}

}

