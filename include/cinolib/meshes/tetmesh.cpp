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
#include <cinolib/meshes/tetmesh.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/io/read_write.h>
#include <cinolib/quality.h>
#include <cinolib/cot.h>
#include <cinolib/symbols.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & polys)
{
    init_tetmesh(verts, polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    init_tetmesh(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    init_tetmesh(verts, polys);
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
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
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
        exit(-1);
    }

    init_tetmesh(tmp_verts, tmp_polys, vert_labels, poly_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::save(const char * filename) const
{
    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
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
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << std::endl;
        exit(-1);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::init_tetmesh(const std::vector<vec3d>             & verts,
                                      const std::vector<std::vector<uint>> & polys)
{
    for(auto v : verts) this->vert_add(v);
    for(auto p : polys) this->poly_add(p);

    this->copy_xyz_to_uvw(UVW_param);

    std::cout << "new mesh\t"      <<
                 this->num_verts() << "V / " <<
                 this->num_edges() << "E / " <<
                 this->num_faces() << "F / " <<
                 this->num_polys() << "P   " << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::init_tetmesh(const std::vector<vec3d>             & verts,
                                      const std::vector<std::vector<uint>> & polys,
                                      const std::vector<int>               & vert_labels,
                                      const std::vector<int>               & poly_labels)
{
    init_tetmesh(verts, polys);

    if(vert_labels.size()==this->num_verts())
    {
        std::cout << "set vert labels" << std::endl;
        for(uint vid=0; vid<this->num_verts(); ++vid)
        {
            this->vert_data(vid).label = vert_labels.at(vid);
        }
    }

    if(poly_labels.size()==this->num_polys())
    {
        std::cout << "set poly labels" << std::endl;
        for(uint pid=0; pid<this->num_polys(); ++pid)
        {
            this->poly_data(pid).label = poly_labels.at(pid);
        }
        this->poly_color_wrt_label();
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

    vec3d u = v1 - v0;    u.normalize();
    vec3d v = v2 - v0;    v.normalize();
    vec3d n = u.cross(v); n.normalize();

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
    uint new_vid = this->vert_add(p);
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
    this->edge_remove(eid);
    return new_vid;
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
uint Tetmesh<M,V,E,F,P>::face_split(const uint fid, const std::vector<double> & bary)
{
    assert(bary.size()==3);

    vec3d p = this->face_vert(fid,0) * bary.at(0) +
              this->face_vert(fid,1) * bary.at(1) +
              this->face_vert(fid,2) * bary.at(2);

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
bool Tetmesh<M,V,E,F,P>::poly_bary_coords(const uint pid, const vec3d & p, std::vector<double> & wgts) const
{
    return tet_barycentric_coords(this->poly_vert(pid,0),
                                  this->poly_vert(pid,1),
                                  this->poly_vert(pid,2),
                                  this->poly_vert(pid,3),
                                  p, wgts);
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

    // detect faces
    std::vector<uint> f0 = { vlist.at(TET_FACES[0][0]), vlist.at(TET_FACES[0][1]), vlist.at(TET_FACES[0][2]) };
    std::vector<uint> f1 = { vlist.at(TET_FACES[1][0]), vlist.at(TET_FACES[1][1]), vlist.at(TET_FACES[1][2]) };
    std::vector<uint> f2 = { vlist.at(TET_FACES[2][0]), vlist.at(TET_FACES[2][1]), vlist.at(TET_FACES[2][2]) };
    std::vector<uint> f3 = { vlist.at(TET_FACES[3][0]), vlist.at(TET_FACES[3][1]), vlist.at(TET_FACES[3][2]) };

    // detect face ids
    int fid0 = this->face_id(f0);
    int fid1 = this->face_id(f1);
    int fid2 = this->face_id(f2);
    int fid3 = this->face_id(f3);

    // add missing faces (with vertices CCW)
    if(fid0 == -1) fid0 = this->face_add(f0);
    if(fid1 == -1) fid1 = this->face_add(f1);
    if(fid2 == -1) fid2 = this->face_add(f2);
    if(fid3 == -1) fid3 = this->face_add(f3);

    std::vector<bool> w(4,false); // assume CW for each face w.r.t. new poly
    if(this->face_winding_agrees_with(fid0, f0.at(0), f0.at(1))) w[0] = true; // set CCW
    if(this->face_winding_agrees_with(fid1, f1.at(0), f1.at(1))) w[1] = true; // set CCW
    if(this->face_winding_agrees_with(fid2, f2.at(0), f2.at(1))) w[2] = true; // set CCW
    if(this->face_winding_agrees_with(fid3, f3.at(0), f3.at(1))) w[3] = true; // set CCW

    uint pid = poly_add({static_cast<uint>(fid0),
                         static_cast<uint>(fid1),
                         static_cast<uint>(fid2),
                         static_cast<uint>(fid3)},w);

    reorder_p2v(pid); // make sure p2v stores tet vertices in the standard way
    update_tet_quality(pid);
    return pid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_split(const uint pid, const std::vector<double> & bary)
{
    assert(bary.size()==4);

    vec3d p = this->poly_vert(pid,0) * bary.at(0) +
              this->poly_vert(pid,1) * bary.at(1) +
              this->poly_vert(pid,2) * bary.at(2) +
              this->poly_vert(pid,3) * bary.at(3);

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
