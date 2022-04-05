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
#include <cinolib/meshes/hexmesh.h>
#include <cinolib/cino_inline.h>
#include <cinolib/quality.h>
#include <cinolib/io/read_write.h>
#include <cinolib/min_max_inf.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/io/io_utilities.h>

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
    this->init(verts, polys_from_serialized_vids(polys,8));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,8));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    this->init(verts, polys);
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
void Hexmesh<M,V,E,F,P>::save(const char * filename) const
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
void Hexmesh<M,V,E,F,P>::update_f_normal(const uint fid)
{
    // STEAL BETTER NORMAL ESTIMATION FROM QUADMESH!
    vec3d v0 = this->face_vert(fid,0);
    vec3d v1 = this->face_vert(fid,1);
    vec3d v2 = this->face_vert(fid,2);

    vec3d u = v1 - v0;     if(!u.is_deg()) u.normalize();
    vec3d v = v2 - v0;     if(!v.is_deg()) v.normalize();
    vec3d n = u.cross(v);  if(!n.is_deg()) n.normalize();

    this->face_data(fid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::print_quality(const bool list_folded_elements)
{
    if(list_folded_elements) std::cout << "Folded Hexa: ";

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
uint Hexmesh<M,V,E,F,P>::poly_vert_opposite_to(const uint pid, const uint fid, const uint vid) const
{
    for(uint nbr : this->poly_v2v(pid,vid))
    {
        if(!this->face_contains_vert(fid,nbr)) return nbr;
    }
    assert(false);
    return 0; // warning killer
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
    return 0; // warning killer
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
bool Hexmesh<M,V,E,F,P>::poly_fix_orientation()
{
    if(AbstractPolyhedralMesh<M,V,E,F,P>::poly_fix_orientation())
    {
        uint bad = 0;
        for(uint pid=0; pid<this->num_polys(); ++pid)
        {
            this->poly_reorder_p2v(pid);
            this->update_p_quality(pid);
            if(this->poly_data(pid).quality < 0.0) ++bad;
        }
        if(bad > 0.5*this->num_polys())
        {
            for(uint pid=0; pid<this->num_polys(); ++pid)
            {
                this->poly_flip_winding(pid);
                this->poly_reorder_p2v(pid);
            }
            this->update_quality();
        }
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::poly_local_frame(const uint    pid,
                                                mat3d & xyz)
{
    auto verts = this->poly_verts(pid);
    mat3d m({(verts[0]+verts[1]+verts[2]+verts[3])/4 - (verts[4]+verts[5]+verts[6]+verts[7])/4,
             (verts[1]+verts[2]+verts[6]+verts[5])/4 - (verts[0]+verts[3]+verts[7]+verts[4])/4,
             (verts[0]+verts[1]+verts[5]+verts[4])/4 - (verts[3]+verts[2]+verts[6]+verts[7])/4});

    // find the local orthonormal frame that is closest to the segments joining the centroids of opposite hex faces
    xyz = m.closest_orthogonal_matrix(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::poly_local_frame(const uint    pid,
                                                vec3d & x,
                                                vec3d & y,
                                                vec3d & z)
{
    mat3d xyz;
    poly_local_frame(pid, xyz);
    x = xyz.col(0);
    y = xyz.col(1);
    z = xyz.col(2);
}

}
