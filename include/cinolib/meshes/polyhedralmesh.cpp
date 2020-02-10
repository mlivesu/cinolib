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
#include <cinolib/meshes/polyhedralmesh.h>
#include <cinolib/io/read_write.h>
#include <cinolib/geometry/aabb.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/vector_serialization.h>

#include <algorithm>
#include <cmath>
#include <float.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const char * filename)
{
    load(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const std::vector<vec3d>             & verts,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & polys,
                                          const std::vector<std::vector<bool>> & polys_face_winding)
{
    this->init(verts, faces, polys, polys_face_winding);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();
    this->mesh_data().filename = std::string(filename);

    std::vector<vec3d>             tmp_verts;
    std::vector<std::vector<uint>> tmp_faces;
    std::vector<std::vector<uint>> tmp_polys;
    std::vector<std::vector<bool>> tmp_polys_face_winding;
    std::vector<int>               vert_labels;
    std::vector<int>               poly_labels;

    std::string str(filename);
    std::string filetype = "." + get_file_extension(str);

    if (filetype.compare(".hybrid") == 0 ||
        filetype.compare(".HYBRID") == 0)
    {
        read_HYBDRID(filename, tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
        this->init(tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
    }
    else if (filetype.compare(".hedra") == 0 ||
             filetype.compare(".HEDRA") == 0)
    {
        read_HEDRA(filename, tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
        this->init(tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
    }
    else if (filetype.compare(".mesh") == 0 ||
             filetype.compare(".MESH") == 0)
    {
        read_MESH(filename, tmp_verts, tmp_polys, vert_labels, poly_labels);
        this->init(tmp_verts, tmp_polys, vert_labels, poly_labels);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        read_VTU(filename, tmp_verts, tmp_polys);
        this->init(tmp_verts, tmp_polys, vert_labels, poly_labels);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        read_VTK(filename, tmp_verts, tmp_polys);
        this->init(tmp_verts, tmp_polys, vert_labels, poly_labels);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << std::endl;
    }    
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::save(const char * filename) const
{
    std::string str(filename);
    std::string filetype = str.substr(str.size()-6,6);

    if (filetype.compare(".hedra") == 0 ||
        filetype.compare(".HEDRA") == 0)
    {
        write_HEDRA(filename, this->verts, this->faces, this->polys, this->polys_face_winding);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << std::endl;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_f_normal(const uint fid)
{
    assert(this->verts_per_face(fid)>2);
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_face(fid); ++off) points.push_back(this->face_vert(fid,off));
    this->face_data(fid).normal = polygon_normal(points);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_hexahedron(const uint pid) const
{
    if(this->verts_per_poly(pid)!=8) return false;
    if(this->faces_per_poly(pid)!=6) return false;
    for(uint fid : this->adj_p2f(pid))
    {
        if(!this->face_is_quad(fid)) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_tetrahedron(const uint pid) const
{
    if(this->verts_per_poly(pid)!=4) return false;
    if(this->faces_per_poly(pid)!=4) return false;
    for(uint fid : this->adj_p2f(pid))
    {
        if(!this->face_is_tri(fid)) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_hexable_w_midpoint(const uint pid) const
{
    // the only topological condition to be satisfied for a genus
    // zero polyedron to be hexable, is that each of its vertices
    // has three incident edges:
    //
    // Hexahedral Meshing Using Midpoint Subdivision and Integer Programming
    // T.S. Li, R.M. McKeag, C.G. Armstrong
    // Computer Methods in Applied Mechanics and Engineering, 1995
    //
    for(uint vid : this->adj_p2v(pid))
    {        
        if(this->poly_vert_valence(pid,vid)!=3) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_prism(const uint pid) const
{
    // test all possible bases for a prism
    //
    for(uint fid : this->adj_p2f(pid))
    {
        if(this->poly_is_prism(pid,fid)) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_prism(const uint pid, const uint fid) const
{
    assert(this->poly_contains_face(pid,fid));

    // a prism base has one incident quad per edge and
    // an a twin n-gon at the opposite base

    int nf = this->faces_per_poly(pid);

    if((int)this->edges_per_face(fid)!=nf-2) return false;

    std::set<uint> f_visited;
    f_visited.insert(fid);
    for(uint eid : this->adj_f2e(fid))
    {
        uint nbr = this->poly_face_adj_through_edge(pid, fid, eid);
        if (!this->face_is_quad(nbr)) return false;
        f_visited.insert(nbr);
    }
    //assert((int)f_visited.size()==nf-1);

    for(uint nbr : this->adj_p2f(pid))
    {
        if(CONTAINS(f_visited,nbr)) continue;
        if(this->verts_per_face(nbr)==this->verts_per_face(fid)) return true;
    }
    return false;
}

}
