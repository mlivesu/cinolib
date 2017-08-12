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

#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/io/read_write.h>
#include <cinolib/quality.h>
#include <cinolib/bbox.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & polys)
{
    this->verts = verts;
    from_serialized_vids_to_general_polyhedra(polys);

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    from_serialized_vids_to_general_polyhedra(polys);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    this->verts = verts;
    from_serialized_vids_to_general_polyhedra(polys);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & faces,
                            const std::vector<std::vector<uint>> & polys,
                            const std::vector<std::vector<bool>> & polys_face_winding)
{
    this->verts = verts;
    this->faces = faces;
    this->polys = polys;
    this->polys_face_winding = polys_face_winding;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Tetmesh<M,V,E,F,P>::Tetmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::from_serialized_vids_to_general_polyhedra(const std::vector<uint> & tets)
{
    this->faces.clear();
    this->polys.clear();
    this->polys_face_winding.clear();

    uint n_tets = tets.size()/4;

    std::map<std::vector<uint>,uint> f_map;
    for(uint tid=0; tid<n_tets; ++tid)
    {
        std::vector<uint> p_faces;
        std::vector<bool> p_winding;

        for(uint i=0; i<4; ++i)
        {
            uint base = tid*4;
            std::vector<uint> f =
            {
                tets.at(base + TET_FACES[i][0]),
                tets.at(base + TET_FACES[i][1]),
                tets.at(base + TET_FACES[i][2]),
            };
            std::vector<uint> sorted_f = f;
            sort(sorted_f.begin(), sorted_f.end());
            auto query = f_map.find(sorted_f);

            if (query == f_map.end())
            {
                uint fresh_id = f_map.size();
                f_map[sorted_f] = fresh_id;
                this->faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        this->polys.push_back(p_faces);
        this->polys_face_winding.push_back(p_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::from_serialized_vids_to_general_polyhedra(const std::vector<std::vector<uint>> & tets)
{
    this->faces.clear();
    this->polys.clear();
    this->polys_face_winding.clear();

    std::map<std::vector<uint>,uint> f_map;
    for(uint tid=0; tid<tets.size(); ++tid)
    {
        assert(tets.at(tid).size() == 4);

        std::vector<uint> p_faces;
        std::vector<bool> p_winding;

        for(uint i=0; i<4; ++i)
        {
            std::vector<uint> f =
            {
                tets.at(tid).at(TET_FACES[i][0]),
                tets.at(tid).at(TET_FACES[i][1]),
                tets.at(tid).at(TET_FACES[i][2]),
            };
            std::vector<uint> sorted_f = f;
            sort(sorted_f.begin(), sorted_f.end());
            auto query = f_map.find(sorted_f);

            if (query == f_map.end())
            {
                uint fresh_id = f_map.size();
                f_map[sorted_f] = fresh_id;
                this->faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        this->polys.push_back(p_faces);
        this->polys_face_winding.push_back(p_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();

    std::vector<std::vector<uint>> tets;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        read_MESH(filename, this->verts, tets);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        read_VTU(filename, this->verts, tets);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        read_VTK(filename, this->verts, tets);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    from_serialized_vids_to_general_polyhedra(tets);

    logger << this->num_polys() << " tetrahedra read" << endl;
    logger << this->num_verts() << " vertices   read" << endl;

    this->mesh_data().filename = std::string(filename);
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
        write_MESH(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        write_MESH(filename, this->verts, this->p2v);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        write_MESH(filename, this->verts, this->p2v);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::init()
{
    AbstractPolyhedralMesh<M,V,E,F,P>::init();
    reorder_p2v(); // makes sure the p2v adjacency stores vertices in a way that uniquely defines per element connectivity
    update_tet_quality();
    this->copy_xyz_to_uvw(UVW_param);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::update_normals()
{
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        vec3d v0 = this->face_vert(fid,0);
        vec3d v1 = this->face_vert(fid,1);
        vec3d v2 = this->face_vert(fid,2);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        this->face_data(fid).normal = n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::reorder_p2v()
{
    for(uint pid=0; pid<this->num_polys(); ++pid) reorder_p2v(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Tetmesh<M,V,E,F,P>::reorder_p2v(const uint pid)
{
    std::vector<uint> new_p2v;
    uint f = this->poly_face_id(pid,0);
    for(uint i=0; i<this->verts_per_face(f); ++i) new_p2v.push_back(this->face_vert_id(f,i));
    if (this->poly_face_is_CCW(pid,f)) std::reverse(new_p2v.begin(),new_p2v.end());
    for(uint vid : this->adj_p2v(pid))
    {
        if (this->face_contains_vert(f,vid)) continue;
        new_p2v.push_back(vid);
    }
    assert(new_p2v.size()==4);
    this->p2v.at(pid) = new_p2v;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Tetmesh<M,V,E,F,P>::face_tessellation(const uint fid) const
{
    return this->faces.at(fid);
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
            double l_k       = poly_edge_length(pid, e_opp);
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
int Tetmesh<M,V,E,F,P>::poly_adjacent_through_face(const uint pid, const uint face_offset) const
{
    // TODO: move to global ids!!!!!!
    for(uint nbr : this->adj_p2p(pid))
    {
        if (this->poly_shared_face(pid,nbr) == (int)face_offset) return nbr;
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
vec3d Tetmesh<M,V,E,F,P>::poly_face_normal(const uint pid, const uint face_offset) const
{
    // TODO: move to global ids!!!!!!
    vec3d v0 = this->poly_vert(pid, TET_FACES[face_offset][0]);
    vec3d v1 = this->poly_vert(pid, TET_FACES[face_offset][1]);
    vec3d v2 = this->poly_vert(pid, TET_FACES[face_offset][2]);
    vec3d n = (v1-v0).cross(v2-v0);
    n.normalize();
    return n;
    return vec3d();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Tetmesh<M,V,E,F,P>::poly_face_area(const uint pid, const uint face_offset) const
{
    // TODO: move to global ids!!!!!!
    vec3d v0 = this->poly_vert(pid, TET_FACES[face_offset][0]);
    vec3d v1 = this->poly_vert(pid, TET_FACES[face_offset][1]);
    vec3d v2 = this->poly_vert(pid, TET_FACES[face_offset][2]);
    return (0.5 * (v1-v0).cross(v2-v0).length());
    return 0.0;
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
double Tetmesh<M,V,E,F,P>::poly_dihedral_angle(const uint pid, const uint face_offset1, const uint face_offset2) const
{
    // TODO: move to global ids!!!!!!
    vec3d   n0 = poly_face_normal(pid, face_offset1);
    vec3d   n1 = poly_face_normal(pid, face_offset2);
    double  alpha = acos(n0.dot(-n1));
    assert(!std::isnan(alpha));
    return alpha;
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_vert_opposite_to(const uint pid, const uint face_offset) const
{
    // TODO: move to global ids!!!!!!
    for(uint vert_offset=0; vert_offset<4; ++vert_offset)
    {
        uint vid = this->poly_vert_id(pid, vert_offset);

        if (this->poly_vert_id(pid, TET_FACES[face_offset][0]) != vid &&
            this->poly_vert_id(pid, TET_FACES[face_offset][1]) != vid &&
            this->poly_vert_id(pid, TET_FACES[face_offset][2]) != vid)
        {
            return vid;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_edge_opposite_to(const uint pid, const uint vid0, const uint vid1) const
{
    // TODO: move to global ids!!!!!!
    assert(this->poly_contains_vert(pid,vid0));
    assert(this->poly_contains_vert(pid,vid1));

    for(uint off=0; off<this->edges_per_poly(); ++off)
    {
        uint e0 = this->poly_vert_id(pid, TET_EDGES[off][0]);
        uint e1 = this->poly_vert_id(pid, TET_EDGES[off][1]);
        if ((e0!=vid0 && e1!=vid1)  || (e0!=vid1 && e1!=vid0)) return off;
    }
//    for(uint eid : this->adj_p2e(pid))
//    {
//        if (this->edge_contains_vert(eid,vid0) &&
//            this->edge_contains_vert(eid,vid1))
//        {
//            return eid;
//        }
//    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Tetmesh<M,V,E,F,P>::poly_face_opposite_to(const uint pid, const uint vid) const
{
    // TODO: move to global ids!!!!!!
    assert(this->poly_contains_vert(pid, vid));
    for(uint f=0; f<this->faces_per_poly(); ++f)
    {
        if (this->poly_vert_id(pid, TET_FACES[f][0]) != vid &&
            this->poly_vert_id(pid, TET_FACES[f][1]) != vid &&
            this->poly_vert_id(pid, TET_FACES[f][2]) != vid)
        {
            return f;
        }
    }
    assert(false);

//    assert(this->poly_contains_vert(pid, vid));
//    for(uint fid : this->adj_p2f(pid))
//    {
//        if (!this->face_contains_vert(fid,vid)) return fid;
//    }
//    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Tetmesh<M,V,E,F,P>::poly_edge_length(const uint pid, const uint edge_offset) const
{
    // TODO: move to global ids!!!!!!
    vec3d A = this->poly_vert(pid, TET_EDGES[edge_offset][0]);
    vec3d B = this->poly_vert(pid, TET_EDGES[edge_offset][1]);
    return (A-B).length();
    return 0.0;
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
