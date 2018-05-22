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
#include <cinolib/meshes/hexmesh.h>
#include <cinolib/cino_inline.h>
#include <cinolib/quality.h>
#include <cinolib/io/read_write.h>
#include <cinolib/inf.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/subdivision_schemas.h>
#include <cinolib/vector_serialization.h>

#include <queue>
#include <float.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & polys)
{
    init_hexmesh(verts, polys_from_serialized_vids(polys,8));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    init_hexmesh(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,8));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    init_hexmesh(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const char * filename)
{
    load(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::load(const char * filename)
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

    init_hexmesh(tmp_verts, tmp_polys, vert_labels, poly_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::save(const char * filename) const
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
void Hexmesh<M,V,E,F,P>::init_hexmesh(const std::vector<vec3d>             & verts,
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

     print_quality();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::init_hexmesh(const std::vector<vec3d>             & verts,
                                      const std::vector<std::vector<uint>> & polys,
                                      const std::vector<int>               & vert_labels,
                                      const std::vector<int>               & poly_labels)
{
    init_hexmesh(verts, polys);

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
void Hexmesh<M,V,E,F,P>::update_f_normal(const uint fid)
{
    // STEAL BETTER NORMAL ESTIMATION FROM QUADMESH!
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
void Hexmesh<M,V,E,F,P>::reorder_p2v(const uint pid)
{
    uint fid_bot = this->poly_face_id(pid,0);
    uint fid_top = this->poly_face_opposite_to(pid, fid_bot);
    std::vector<uint> vlist(8);
    vlist[0] = this->face_vert_id(fid_bot,HEXA_FACES[0][0]);
    vlist[1] = this->face_vert_id(fid_bot,HEXA_FACES[0][1]);
    vlist[2] = this->face_vert_id(fid_bot,HEXA_FACES[0][2]);
    vlist[3] = this->face_vert_id(fid_bot,HEXA_FACES[0][3]);
    if (this->poly_face_is_CW(pid,fid_bot)) std::swap(vlist[1],vlist[3]);
    for(uint vid : this->face_verts_id(fid_top))
    {
        if(this->verts_are_adjacent(vid,vlist[0])) vlist[4] = vid; else
        if(this->verts_are_adjacent(vid,vlist[1])) vlist[5] = vid; else
        if(this->verts_are_adjacent(vid,vlist[2])) vlist[6] = vid; else
        if(this->verts_are_adjacent(vid,vlist[3])) vlist[7] = vid; else
        assert(false);
    }
    this->p2v.at(pid) = vlist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::print_quality(const bool list_folded_elements)
{
    if (list_folded_elements) std::cout << "Folded Hexa: ";

    double asj = 0.0;
    double msj = inf_double;
    uint   inv = 0;

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        double q = this->poly_data(pid).quality;

        asj += q;
        msj = std::min(msj, q);

        if (q <= 0.0)
        {
            ++inv;
            if (list_folded_elements) std::cout << pid << " - ";
        }
    }
    asj /= static_cast<double>(this->num_polys());

    if (list_folded_elements) std::cout << std::endl << std::endl;

    std::cout << std::endl;
    std::cout << "MIN SJ : " << msj << std::endl;
    std::cout << "AVG SJ : " << asj << std::endl;
    std::cout << "INV EL : " << inv << " (out of " << this->num_polys() << ")" << std::endl;
    std::cout << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Hexmesh<M,V,E,F,P>::vert_is_singular(const uint vid) const
{
    uint count = 0;
    for(uint eid : this->adj_v2e(vid))
    {
        if(this->edge_is_singular(eid)) ++count;
    }
    if (count>0 && count!=2) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Hexmesh<M,V,E,F,P>::vert_is_regular(const uint vid) const
{
    return !this->vert_is_singular(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Hexmesh<M,V,E,F,P>::edge_is_singular(const uint eid) const
{
    if (this->edge_is_on_srf(eid))
    {
        if(this->edge_valence(eid)!=2) return true;
        return false;
    }
    if (this->edge_valence(eid)!=4) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Hexmesh<M,V,E,F,P>::edge_is_regular(const uint eid) const
{
    return !this->edge_is_singular(eid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Hexmesh<M,V,E,F,P>::face_sheet(const uint fid) const
{
    std::vector<uint> sheet;

    std::queue<uint>  q;
    q.push(fid);

    std::unordered_set<uint> visited;
    visited.insert(fid);

    while(!q.empty())
    {
        uint fid = q.front();
        q.pop();

        sheet.push_back(fid);

        for(uint eid : this->adj_f2e(fid))
        {
            if (this->edge_is_singular(eid)) continue;

            for(uint nbr : this->adj_e2f(eid))
            {
                if(!this->faces_share_poly(fid,nbr) && DOES_NOT_CONTAIN(visited,nbr))
                {
                    visited.insert(nbr);
                    q.push(nbr);
                }
            }
        }
    }
    return sheet;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d Hexmesh<M,V,E,F,P>::verts_average(const std::vector<uint> & vids) const
{
    vec3d res(0,0,0);
    for(uint vid: vids) res += this->vert(vid);
    res /= static_cast<double>(vids.size());
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::update_hex_quality(const uint pid)
{
    this->poly_data(pid).quality = hex_scaled_jacobian(this->poly_vert(pid,0),
                                                       this->poly_vert(pid,1),
                                                       this->poly_vert(pid,2),
                                                       this->poly_vert(pid,3),
                                                       this->poly_vert(pid,4),
                                                       this->poly_vert(pid,5),
                                                       this->poly_vert(pid,6),
                                                       this->poly_vert(pid,7));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::update_hex_quality()
{
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        update_hex_quality(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Hexmesh<M,V,E,F,P>::poly_face_opposite_to(const uint pid, const uint fid) const
{
    assert(this->poly_contains_face(pid, fid));
    for(uint f : this->adj_p2f(pid))
    {
        if(this->faces_are_disjoint(fid,f)) return f;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::poly_subdivide(const std::vector<std::vector<std::vector<uint>>> & poly_split_scheme)
{
    std::vector<vec3d> new_verts;
    std::vector<uint>  new_polys;
    std::map<std::vector<uint>,uint> v_map;

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        for(const auto & sub_poly: poly_split_scheme)
        {
            assert(sub_poly.size() == 8);
            for(uint off=0; off<8; ++off)
            {
                std::vector<uint> vids;
                for(uint i : sub_poly.at(off)) vids.push_back(this->poly_vert_id(pid,i));
                sort(vids.begin(), vids.end());

                auto query = v_map.find(vids);
                if (query != v_map.end())
                {
                    new_polys.push_back(query->second);
                }
                else
                {
                    uint fresh_id = new_verts.size();
                    new_verts.push_back(verts_average(vids));
                    v_map[vids] = fresh_id;
                    new_polys.push_back(fresh_id);
                }
            }
        }
    }
    *this = Hexmesh<M,V,E,F,P>(new_verts,new_polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double Hexmesh<M,V,E,F,P>::poly_volume(const uint pid) const
{
    return hex_unsigned_volume(this->poly_vert(pid,0),
                               this->poly_vert(pid,1),
                               this->poly_vert(pid,2),
                               this->poly_vert(pid,3),
                               this->poly_vert(pid,4),
                               this->poly_vert(pid,5),
                               this->poly_vert(pid,6),
                               this->poly_vert(pid,7));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Hexmesh<M,V,E,F,P>::poly_add(const std::vector<uint> & vlist) // vertex list
{
    assert(vlist.size()==8);

    // detect faces
    std::vector<uint> f0 = { vlist.at(HEXA_FACES[0][0]), vlist.at(HEXA_FACES[0][1]), vlist.at(HEXA_FACES[0][2]), vlist.at(HEXA_FACES[0][3]) };
    std::vector<uint> f1 = { vlist.at(HEXA_FACES[1][0]), vlist.at(HEXA_FACES[1][1]), vlist.at(HEXA_FACES[1][2]), vlist.at(HEXA_FACES[1][3]) };
    std::vector<uint> f2 = { vlist.at(HEXA_FACES[2][0]), vlist.at(HEXA_FACES[2][1]), vlist.at(HEXA_FACES[2][2]), vlist.at(HEXA_FACES[2][3]) };
    std::vector<uint> f3 = { vlist.at(HEXA_FACES[3][0]), vlist.at(HEXA_FACES[3][1]), vlist.at(HEXA_FACES[3][2]), vlist.at(HEXA_FACES[3][3]) };
    std::vector<uint> f4 = { vlist.at(HEXA_FACES[4][0]), vlist.at(HEXA_FACES[4][1]), vlist.at(HEXA_FACES[4][2]), vlist.at(HEXA_FACES[4][3]) };
    std::vector<uint> f5 = { vlist.at(HEXA_FACES[5][0]), vlist.at(HEXA_FACES[5][1]), vlist.at(HEXA_FACES[5][2]), vlist.at(HEXA_FACES[5][3]) };

    // assume faces already exist and they will be seen CW for the new element)
    std::vector<bool> w = { false, false, false, false, false, false };

    // detect face ids
    int fid0 = this->face_id(f0);
    int fid1 = this->face_id(f1);
    int fid2 = this->face_id(f2);
    int fid3 = this->face_id(f3);
    int fid4 = this->face_id(f4);
    int fid5 = this->face_id(f5);

    // add missing faces (with vertices CCW)
    if(fid0 == -1) { fid0 = this->face_add(f0); w.at(0) = true; }
    if(fid1 == -1) { fid1 = this->face_add(f1); w.at(1) = true; }
    if(fid2 == -1) { fid2 = this->face_add(f2); w.at(2) = true; }
    if(fid3 == -1) { fid3 = this->face_add(f3); w.at(3) = true; }
    if(fid4 == -1) { fid4 = this->face_add(f4); w.at(4) = true; }
    if(fid5 == -1) { fid5 = this->face_add(f5); w.at(5) = true; }

    // add hexa
    uint pid = poly_add({static_cast<uint>(fid0),
                         static_cast<uint>(fid1),
                         static_cast<uint>(fid2),
                         static_cast<uint>(fid3),
                         static_cast<uint>(fid4),
                         static_cast<uint>(fid5),},w);

    reorder_p2v(pid); // make sure p2v stores hex vertices in the standard way
    update_hex_quality(pid);
    return pid;
}

}
