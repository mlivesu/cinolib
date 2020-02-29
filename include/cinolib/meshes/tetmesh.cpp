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
#include <cinolib/meshes/tetmesh.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/io/read_write.h>
#include <cinolib/quality.h>
#include <cinolib/cot.h>
#include <cinolib/symbols.h>
#include <cinolib/io/io_utilities.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & polys)
{
    this->init(verts, polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    this->init(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const char * filename)
{
    load(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();
    this->mesh_data().filename = std::string(filename);

    std::vector<vec3d>             tmp_verts;
    std::vector<std::vector<uint>> tmp_polys;
    std::vector<int>               vert_labels;
    std::vector<int>               poly_labels;

    std::string str(filename);
    std::string filetype = "." + get_file_extension(str);

    if (filetype.compare(".mesh") == 0 ||
        filetype.compare(".MESH") == 0)
    {
        read_MESH(filename, tmp_verts, tmp_polys, vert_labels, poly_labels);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        read_VTU(filename, tmp_verts, tmp_polys);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        read_VTK(filename, tmp_verts, tmp_polys);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << std::endl;
    }

    this->init(tmp_verts, tmp_polys, vert_labels, poly_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::save(const char * filename) const
{
    std::string str(filename);
    std::string filetype = "." + get_file_extension(str);

    if (filetype.compare(".mesh") == 0 ||
        filetype.compare(".MESH") == 0)
    {
        if(this->polys_are_labeled())
        {
            write_MESH(filename, this->verts, this->p2v, std::vector<int>(this->num_verts(),0), this->vector_poly_labels());
        }
        else write_MESH(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".tet") == 0 ||
             filetype.compare(".TET") == 0)
    {
        write_TET(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        write_VTU(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        write_VTK(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".hedra") == 0 ||
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
void Tetmesh<M,V,E,F,P>::update_f_normal(const uint fid)
{
    vec3d v0 = this->face_vert(fid,0);
    vec3d v1 = this->face_vert(fid,1);
    vec3d v2 = this->face_vert(fid,2);

    vec3d u = v1 - v0;
    vec3d v = v2 - v0;
    vec3d n = u.cross(v);
    n.normalize();

    this->face_data(fid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::reorder_p2v(const uint pid)
{
    uint fid = this->poly_face_id(pid,0);
    std::vector<uint> vlist(4);
    vlist[0] = this->face_vert_id(fid,TET_FACES[0][0]);
    vlist[1] = this->face_vert_id(fid,TET_FACES[0][1]);
    vlist[2] = this->face_vert_id(fid,TET_FACES[0][2]);
    vlist[3] = this->poly_vert_opposite_to(pid,fid);
    if (this->poly_face_is_CW(pid,fid)) std::swap(vlist[1],vlist[2]);
    this->p2v.at(pid) = vlist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Tetmesh<M,V,E,F,P>::face_area(const uint fid) const
{
    return triangle_area(this->face_vert(fid,0),
                         this->face_vert(fid,1),
                         this->face_vert(fid,2));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::edge_split(const uint eid, const double lambda)
{
    return edge_split(eid, this->edge_sample_at(eid,lambda));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::edge_split(const uint eid, const vec3d & p)
{
    // create sub edges and propagate attributes
    uint vid0    = this->edge_vert_id(eid, 0);
    uint vid1    = this->edge_vert_id(eid, 1);
    uint new_vid = this->vert_add(p);
    uint eid0    = this->edge_add(vid0, new_vid);
    uint eid1    = this->edge_add(vid1, new_vid);
    this->edge_data(eid0) = this->edge_data(eid);
    this->edge_data(eid1) = this->edge_data(eid);

    // create sub faces and propagate attributes
    for(uint fid : this->adj_e2f(eid))
    {
        if(!this->face_verts_are_CCW(fid,vid1,vid0)) std::swap(vid1,vid0);
        uint vid2 = this->face_vert_opposite_to(fid,eid);
        uint fid0 = this->face_add({new_vid, vid1, vid2});
        uint fid1 = this->face_add({new_vid, vid2, vid0});
        this->face_data(fid0) = this->face_data(fid);
        this->face_data(fid1) = this->face_data(fid);
        this->update_f_normal(fid0);
        this->update_f_normal(fid1);
    }

    // create sub polys and propagate attributes
    for(uint pid : this->adj_e2p(eid))
    {
        for(uint fid : this->poly_faces_opposite_to(pid,eid))
        {
            std::vector<uint> tet(4);
            tet[0] = this->face_vert_id(fid,TET_FACES[0][0]);
            tet[1] = this->face_vert_id(fid,TET_FACES[0][1]);
            tet[2] = this->face_vert_id(fid,TET_FACES[0][2]);
            tet[3] = new_vid;
            if (this->poly_face_is_CW(pid,fid)) std::swap(tet[1],tet[2]);
            uint new_pid = this->poly_add(tet);
            this->poly_data(new_pid) = this->poly_data(pid);
        }
    }

    // remove old edge and all elements attached to it
    this->edge_remove(eid);
    return new_vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Tetmesh<M,V,E,F,P>::edge_is_collapsible(const uint eid, const vec3d & p) const
{
    if(!edge_is_topologically_collapsible(eid)) return false;
    if(!edge_is_geometrically_collapsible(eid, p)) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Tetmesh<M,V,E,F,P>::edge_is_collapsible(const uint eid, const double lambda) const
{
    return edge_is_collapsible(eid, this->edge_sample_at(eid, lambda));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Tetmesh<M,V,E,F,P>::edge_is_topologically_collapsible(const uint eid) const
{
    // implements the "Link Condition" for 3-complexes described in:
    // Topology preserving edge contraction
    // Tamal K. Dey, Herbert Edelsbrunner, Sumanta Guha and Dmitry V. Nekhayev
    // 1999
    //
    // NOTE: to avoid topological changes at the border, boundary faces are assumed
    // to form a tet with an infinite vertex, which thus enters in the various simplex links

    uint v0 = this->edge_vert_id(eid,0);
    uint v1 = this->edge_vert_id(eid,1);

    auto v0_f_link = this->vert_faces_link(v0);
    auto v1_f_link = this->vert_faces_link(v1);

    // add faces at infinity (if any)
    // ("fake" unique ids for faces at infinity are obained by summing the number of faces with the id of the edge from which the each face emanates)
    if(this->vert_is_on_srf(v0)) for(uint fid : this->vert_adj_srf_faces(v0)) v0_f_link.push_back(this->num_faces()+this->face_edge_opposite_to(fid,v0));
    if(this->vert_is_on_srf(v1)) for(uint fid : this->vert_adj_srf_faces(v1)) v1_f_link.push_back(this->num_faces()+this->face_edge_opposite_to(fid,v1));

    std::vector<uint> inters;
    SET_INTERSECTION(v0_f_link, v1_f_link, inters, true);
    if(!inters.empty()) return false;

    auto v0_e_link = this->vert_edges_link(v0);
    auto v1_e_link = this->vert_edges_link(v1);
    auto e_e_link  = this->edge_edges_link(eid);

    // add edges at infinity (if any)
    // ("fake" unique ids for edges at infinity are obained by summing the number of edges with the id of the vertex from which the each edge emanates)
    if(this->vert_is_on_srf(v0))  for(uint vid : this->vert_adj_srf_verts(v0))  v0_e_link.push_back(this->num_edges()+vid);
    if(this->vert_is_on_srf(v1))  for(uint vid : this->vert_adj_srf_verts(v1))  v1_e_link.push_back(this->num_edges()+vid);
    if(this->edge_is_on_srf(eid)) for(uint fid : this->edge_adj_srf_faces(eid)) e_e_link.push_back (this->num_edges()+this->face_vert_opposite_to(fid,eid));
    SORT_VEC(e_e_link, false);

    SET_INTERSECTION(v0_e_link, v1_e_link, inters, true);
    if(inters!=e_e_link) return false;

    auto v0_v_link = this->vert_verts_link(v0);
    auto v1_v_link = this->vert_verts_link(v1);
    auto e_v_link  = this->edge_verts_link(eid);
    SORT_VEC(e_v_link, false);

    // add verts at infinity (if any)
    uint inf_vert = this->num_verts();
    if(this->vert_is_on_srf(v0))  v0_v_link.push_back(inf_vert);
    if(this->vert_is_on_srf(v1))  v1_v_link.push_back(inf_vert);
    if(this->edge_is_on_srf(eid)) e_v_link.push_back(inf_vert); // (last position in the vector is ok, as inf_vert is the biggest id!)

    SET_INTERSECTION(v0_v_link, v1_v_link, inters, true);
    if(inters!=e_v_link) return false;

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Tetmesh<M,V,E,F,P>::edge_is_geometrically_collapsible(const uint eid, const vec3d & p) const
{
    // no tet should flip or collapse
    uint vid0 = this->edge_vert_id(eid,0);
    uint vid1 = this->edge_vert_id(eid,1);

    std::unordered_set<uint> polys_to_test;
    for(uint pid : this->adj_v2p(vid0)) if(!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);
    for(uint pid : this->adj_v2p(vid1)) if(!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);

    for(uint pid : polys_to_test)
    {
        vec3d v[4];
        for(int i=0; i<4; ++i)
        {
            bool is_v0 = (this->poly_vert_id(pid,i) == vid0);
            bool is_v1 = (this->poly_vert_id(pid,i) == vid1);
            v[i] = (is_v0 || is_v1) ? p : this->poly_vert(pid,i);
        }
        // avoid collapses (but also tiny tets)
        if(tet_volume(v[0], v[1], v[2], v[3]) < 1e-10) return false;
    }

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int Tetmesh<M,V,E,F,P>::edge_collapse(const uint eid, const double lambda, const double topologic_check, const double geometric_check)
{
    vec3d p = this->edge_sample_at(eid, lambda);
    return edge_collapse(eid, p, topologic_check, geometric_check);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int Tetmesh<M,V,E,F,P>::edge_collapse(const uint eid, const vec3d & p, const double topologic_check, const double geometric_check)
{
    if(topologic_check && !edge_is_topologically_collapsible(eid))    return -1;
    if(geometric_check && !edge_is_geometrically_collapsible(eid, p)) return -1;

#ifndef NDEBUG
    int euler_before = this->Euler_characteristic();
#endif

    uint vert_to_keep   = this->edge_vert_id(eid,0);
    uint vert_to_remove = this->edge_vert_id(eid,1);
    if(vert_to_remove < vert_to_keep) std::swap(vert_to_keep, vert_to_remove); // remove vert with highest ID
    this->vert(vert_to_keep) = p; // reposition vertex

    for(uint pid : this->adj_v2p(vert_to_remove))
    {
        if (this->poly_contains_edge(pid, eid)) continue; // no need to update. will collapse

        uint off   = this->poly_vert_offset(pid, vert_to_remove);
        auto vlist = this->poly_verts_id(pid);
        vlist.at(off) = vert_to_keep;
        uint new_pid = this->poly_add(vlist);
        this->poly_data(new_pid) = this->poly_data(pid);

        for(uint fid: this->adj_p2f(pid))
        {
            if(this->face_is_on_srf(fid)) this->update_f_normal(fid);
        }
    }
    if(this->vert_is_on_srf(vert_to_keep)) this->update_v_normal(vert_to_keep);

    this->vert_remove(vert_to_remove);

    if(topologic_check)
    {
#ifndef NDEBUG
        assert(euler_before == this->Euler_characteristic());
#endif
    }

    return vert_to_keep;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::face_edge_opposite_to(const uint fid, const uint vid) const
{
    assert(this->face_contains_vert(fid,vid));
    for(uint eid : this->adj_f2e(fid))
    {
        if(!this->edge_contains_vert(eid,vid)) return eid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::face_vert_opposite_to(const uint fid, const uint eid) const
{
    assert(this->face_contains_edge(fid,eid));
    for(uint vid : this->adj_f2v(fid))
    {
        if(!this->edge_contains_vert(eid,vid)) return vid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::face_split(const uint fid, const std::vector<double> & bc)
{
    assert(bc.size()==3);

    vec3d p = this->face_vert(fid,0) * bc.at(0) +
              this->face_vert(fid,1) * bc.at(1) +
              this->face_vert(fid,2) * bc.at(2);

    return face_split(fid, p);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::face_split(const uint fid, const vec3d & p)
{
    uint new_vid = this->vert_add(p);

    for(uint pid : this->adj_f2p(fid))
    {
        uint opp_vid = this->poly_vert_opposite_to(pid, fid);
        for(uint off=0; off<3; ++off)
        {
            std::vector<uint> tet(4);
            tet[0] = this->face_vert_id(fid,off);
            tet[1] = new_vid;
            tet[2] = this->face_vert_id(fid,(off+1)%3);
            tet[3] = opp_vid;
            if (this->poly_face_is_CW(pid,fid)) std::swap(tet[1],tet[2]);
            uint new_pid = this->poly_add(tet);
            this->poly_data(new_pid) = this->poly_data(pid);
        }
    }

    this->face_remove(fid);
    return new_vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::vert_weights(const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const
{
    switch (type)
    {
        case UNIFORM   : this->vert_weights_uniform(vid, wgts); return;
        case COTANGENT : vert_weights_cotangent(vid, wgts); return;
        default        : assert(false && "Vert weights not supported at this level of the hierarchy!");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* As a reference for the tetmesh version of cotangent weights, see:
 * Gradient field based inhomogeneous volumetric mesh deformation for maxillofacial surgery simulation
 * Sheng-hui Liao, Ruo-feng Tong, Jin-xiang Dong, Fu-dong Zhu
 * Computer & Graphics, 2009
*/
template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::vert_weights_cotangent(const uint vid, std::vector<std::pair<uint,double>> & wgts) const
{
    wgts.clear();
    for(uint eid : this->adj_v2e(vid))
    {
        uint   nbr = this->vert_opposite_to(eid, vid);
        double wgt = 0.0;
        for(uint pid : this->adj_e2p(eid))
        {
            uint   e_opp     = poly_edge_opposite_to(pid, vid, nbr);
            uint   f_opp_vid = poly_face_opposite_to(pid, vid);
            uint   f_opp_nbr = poly_face_opposite_to(pid, nbr);
            double l_k       = this->edge_length(e_opp);
            double teta_k    = poly_dihedral_angle(pid, f_opp_vid, f_opp_nbr);

            wgt += cot(teta_k) * l_k;
        }
        wgt /= 6.0;
        wgts.push_back(std::make_pair(nbr,wgt));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int Tetmesh<M,V,E,F,P>::poly_shared_vert(const uint pid, const std::vector<uint> & incident_edges) const
{
    // TODO: move to global ids!!!!!!
    assert(incident_edges.size() > 1);
    assert(incident_edges.size() < 4);
    uint v_count[4] = { 0, 0, 0, 0 };
    for(uint e : incident_edges)
    {
        ++v_count[TET_EDGES[e][0]];
        ++v_count[TET_EDGES[e][1]];
    }
    for(uint i=0; i<4; ++i)
    {
        if (v_count[i] == incident_edges.size()) return this->poly_vert_id(pid,i);
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::poly_bary_coords(const uint pid, const vec3d & p, double bc[]) const
{
    tet_barycentric_coords(this->poly_vert(pid,0),
                           this->poly_vert(pid,1),
                           this->poly_vert(pid,2),
                           this->poly_vert(pid,3),
                           p, bc);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Tetmesh<M,V,E,F,P>::poly_volume(const uint pid) const
{
    return tet_unsigned_volume(this->poly_vert(pid,0),
                               this->poly_vert(pid,1),
                               this->poly_vert(pid,2),
                               this->poly_vert(pid,3));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_add(const std::vector<uint> & vlist) // vertex list
{
    assert(vlist.size()==4);
    uint pid = AbstractPolyhedralMesh<M,V,E,F,P>::poly_add(vlist);
    reorder_p2v(pid); // make sure p2v stores tet vertices in the standard way
    update_tet_quality(pid);
    return pid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_split(const uint pid, const std::vector<double> & bc)
{
    assert(bc.size()==4);

    vec3d p = this->poly_vert(pid,0) * bc.at(0) +
              this->poly_vert(pid,1) * bc.at(1) +
              this->poly_vert(pid,2) * bc.at(2) +
              this->poly_vert(pid,3) * bc.at(3);

    return poly_split(pid, p);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::polys_split(const std::vector<uint> & pids)
{
    // in order to avoid id conflicts split all the
    // polys starting from the one with highest id
    //
    std::vector<uint> tmp = SORT_VEC(pids, true);
    for(uint pid : tmp) poly_split(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_split(const uint pid, const vec3d & p)
{
    uint new_vid = this->vert_add(p);

    for(uint i=0; i<4; ++i)
    {
        std::vector<uint> tet(4, new_vid);
        tet[TET_FACES[i][0]] = this->poly_vert_id(pid,TET_FACES[i][0]);
        tet[TET_FACES[i][1]] = this->poly_vert_id(pid,TET_FACES[i][1]);
        tet[TET_FACES[i][2]] = this->poly_vert_id(pid,TET_FACES[i][2]);
        uint new_pid = this->poly_add(tet);
        this->poly_data(new_pid) = this->poly_data(pid);
    }
    this->poly_remove(pid);

    return new_vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Tetmesh<M,V,E,F,P>::poly_dihedral_angle(const uint pid, const uint fid0, const uint fid1) const
{
    vec3d n0 =  this->poly_face_normal(pid,fid0);
    vec3d n1 = -this->poly_face_normal(pid,fid1);
    return n0.angle_rad(n1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_vert_opposite_to(const uint pid, const uint fid) const
{
    assert(this->poly_contains_face(pid, fid));

    for(uint vid : this->adj_p2v(pid))
    {
        if (!this->face_contains_vert(fid, vid)) return vid;
    }
    assert(false);
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_edge_opposite_to(const uint pid, const uint eid) const
{
    assert(this->poly_contains_edge(pid,eid));
    return poly_edge_opposite_to(pid, this->edge_vert_id(eid,0), this->edge_vert_id(eid,1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_edge_opposite_to(const uint pid, const uint vid0, const uint vid1) const
{
    assert(this->poly_contains_vert(pid,vid0));
    assert(this->poly_contains_vert(pid,vid1));

    for(uint eid : this->adj_p2e(pid))
    {
        if (!this->edge_contains_vert(eid,vid0) &&
            !this->edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    assert(false);
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_face_opposite_to(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid, vid));

    for(uint fid : this->adj_p2f(pid))
    {
        if (!this->face_contains_vert(fid,vid)) return fid;
    }
    assert(false);
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Tetmesh<M,V,E,F,P>::poly_faces_opposite_to(const uint pid, const uint eid) const
{
    assert(this->poly_contains_edge(pid, eid));
    return this->poly_e2f(pid, this->poly_edge_opposite_to(pid,eid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::update_tet_quality(const uint pid)
{
    this->poly_data(pid).quality = tet_scaled_jacobian(this->poly_vert(pid,0),
                                                       this->poly_vert(pid,1),
                                                       this->poly_vert(pid,2),
                                                       this->poly_vert(pid,3));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::update_tet_quality()
{
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        update_tet_quality(pid);
    }
}

}
