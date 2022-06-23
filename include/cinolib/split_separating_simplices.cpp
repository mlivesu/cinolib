/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#include <cinolib/split_separating_simplices.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
uint split_separating_simplices(Trimesh<M,V,E,P> & m)
{
    uint split_elements = 0;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!m.edge_is_boundary(eid))
        {
            if(m.vert_is_boundary(m.edge_vert_id(eid,0)) &&
               m.vert_is_boundary(m.edge_vert_id(eid,1)))
            {
                m.edge_split(eid);
                ++split_elements;
                // note: there is no need to re-evaluate previous indices because
                // all new elements will receive ids >= eid and will be legal by definition
            }
        }
    }
    return split_elements;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint split_separating_simplices(Tetmesh<M,V,E,F,P> & m)
{
    uint split_elements = 0;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!m.edge_is_on_srf(eid))
        {
            if(m.vert_is_on_srf(m.edge_vert_id(eid,0)) &&
               m.vert_is_on_srf(m.edge_vert_id(eid,1)))
            {
                m.edge_split(eid);
                ++split_elements;
                // note: there is no need to re-evaluate previous indices because
                // all new elements will receive ids >= eid and will be legal by definition
            }
        }
    }
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        if(!m.face_is_on_srf(fid))
        {
            if(m.vert_is_on_srf(m.face_vert_id(fid,0)) &&
               m.vert_is_on_srf(m.face_vert_id(fid,1)) &&
               m.vert_is_on_srf(m.face_vert_id(fid,2)))
            {
                m.face_split(fid);
                ++split_elements;
                // note: there is no need to re-evaluate previous indices because
                // all new elements will receive ids >= fid and will be legal by definition
            }
        }
    }
    return split_elements;
}

}
