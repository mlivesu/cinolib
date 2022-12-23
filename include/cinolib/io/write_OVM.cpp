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
#include <cinolib/io/write_OVM.h>
#include <fstream>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void write_OVM(const char                              * filename,
               const AbstractPolyhedralMesh<M,V,E,F,P> & m)
{
    std::ofstream f(filename);
    f << "OVM ASCII\n";

    f << "Vertices\n" << m.num_verts() << "\n";
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        f.precision(std::numeric_limits<double>::digits10+1);
        f << m.vert(vid).x() << " "
          << m.vert(vid).y() << " "
          << m.vert(vid).z() << "\n";
    }

    f << "Edges\n" << m.num_edges() << "\n";
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        f << m.edge_vert_id(eid,0) << " "
          << m.edge_vert_id(eid,1) << "\n";
    }

    f << "Faces\n" << m.num_faces() << "\n";
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        f << m.verts_per_face(fid);
        for(uint i=0; i<m.verts_per_face(fid); ++i)
        {
            uint v_beg = m.face_vert_id(fid,i);
            uint v_end = m.face_vert_id(fid,(i+1)%m.verts_per_face(fid));
            int  eid   = m.edge_id(v_beg,v_end);
            assert(eid>=0);
            int off = (m.edge_vert_id(eid,0)==v_beg)?0:1; // account for half-edge orientation
            f << " " << eid*2+off;
        }
        f << "\n";
    }

    f << "Polyhedra\n" << m.num_polys() << "\n";
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        f << m.faces_per_poly(pid);
        for(uint fid : m.adj_p2f(pid))
        {
            int off = m.poly_face_winding(pid,fid)?0:1; // account for half-face orientation
            f << " " << fid*2+off;
        }
        f << "\n";
    }

    f.close();
}

}
