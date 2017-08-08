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
#include <cinolib/quality.h>
#include <cinolib/io/read_write.h>
#include <cinolib/common.h>

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
    this->verts = verts;
    from_serialized_vids_to_general_polyhedra(polys);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    from_serialized_vids_to_general_polyhedra(polys);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    this->verts = verts;
    from_serialized_vids_to_general_polyhedra(polys);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Hexmesh<M,V,E,F,P>::Hexmesh(const std::vector<vec3d>             & verts,
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
Hexmesh<M,V,E,F,P>::Hexmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::from_serialized_vids_to_general_polyhedra(const std::vector<uint> & hexa)
{
    this->faces.clear();
    this->polys.clear();
    this->polys_face_winding.clear();

    uint n_hexa = hexa.size()/8;

    std::map<std::vector<uint>,uint> f_map;
    for(uint hid=0; hid<n_hexa; ++hid)
    {
        std::vector<uint> p_faces;
        std::vector<bool> p_winding;

        for(uint i=0; i<6; ++i)
        {
            uint base = hid*8;
            std::vector<uint> f =
            {
                hexa.at(base + HEXA_FACES[i][0]),
                hexa.at(base + HEXA_FACES[i][1]),
                hexa.at(base + HEXA_FACES[i][2]),
                hexa.at(base + HEXA_FACES[i][3]),
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
void Hexmesh<M,V,E,F,P>::from_serialized_vids_to_general_polyhedra(const std::vector<std::vector<uint>> & hexa)
{
    this->faces.clear();
    this->polys.clear();
    this->polys_face_winding.clear();

    std::map<std::vector<uint>,uint> f_map;
    for(uint hid=0; hid<hexa.size(); ++hid)
    {
        assert(hexa.at(hid).size() == 8);

        std::vector<uint> p_faces;
        std::vector<bool> p_winding;

        for(uint i=0; i<6; ++i)
        {
            std::vector<uint> f =
            {
                hexa.at(hid).at(HEXA_FACES[i][0]),
                hexa.at(hid).at(HEXA_FACES[i][1]),
                hexa.at(hid).at(HEXA_FACES[i][2]),
                hexa.at(hid).at(HEXA_FACES[i][3]),
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
void Hexmesh<M,V,E,F,P>::print_quality(const bool list_folded_elements)
{
    if (list_folded_elements) logger << "Folded Hexa: ";

    double asj = 0.0;
    double msj = FLT_MAX;
    uint    inv = 0;

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        double q = this->poly_data(pid).quality;

        asj += q;
        msj = std::min(msj, q);

        if (q <= 0.0)
        {
            ++inv;
            if (list_folded_elements) logger << pid << " - ";
        }
    }
    asj /= static_cast<double>(this->num_polys());

    if (list_folded_elements) logger << endl << endl;

    logger << endl;
    logger << "MIN SJ : " << msj << endl;
    logger << "AVG SJ : " << asj << endl;
    logger << "INV EL : " << inv << " (out of " << this->num_polys() << ")" << endl;
    logger << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();

    std::vector<std::vector<uint>> hexa;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        read_MESH(filename, this->verts, hexa);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        read_VTU(filename, this->verts, hexa);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        read_VTK(filename, this->verts, hexa);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    from_serialized_vids_to_general_polyhedra(hexa);

    logger << this->num_polys() << " hexahedra read" << endl;
    logger << this->num_verts() << " vertices  read" << endl;

    this->mesh_data().filename = std::string(filename);
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
        write_MESH(filename, this->verts, this->export_hex_connectivity());
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        write_VTU(filename, this->verts, this->export_hex_connectivity());
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        write_VTK(filename, this->verts, this->export_hex_connectivity());
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
void Hexmesh<M,V,E,F,P>::init()
{
    AbstractPolyhedralMesh<M,V,E,F,P>::init();
    update_hex_quality();
    print_quality();
    this->copy_xyz_to_uvw(UVW_param);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::update_normals()
{
    // STEAL BETTER NORMAL ESTIMATION FROM QUADMESH!
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
vec3d Hexmesh<M,V,E,F,P>::verts_average(const std::vector<uint> &vids) const
{
    vec3d res(0,0,0);
    for(uint vid: vids) res += this->vert(vid);
    res /= static_cast<double>(vids.size());
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Hexmesh<M,V,E,F,P>::face_tessellation(const uint fid) const
{
    std::vector<uint> triangulation =
    {
        this->face_vert_id(fid,0),
        this->face_vert_id(fid,1),
        this->face_vert_id(fid,2),
        this->face_vert_id(fid,0),
        this->face_vert_id(fid,2),
        this->face_vert_id(fid,3),
    };
    return triangulation;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Hexmesh<M,V,E,F,P>::update_hex_quality(const uint pid)
{
    std::vector<uint> vids = this->poly_as_hex_vlist(pid);
    this->poly_data(pid).quality = hex_scaled_jacobian(this->vert(vids.at(0)),
                                                       this->vert(vids.at(1)),
                                                       this->vert(vids.at(2)),
                                                       this->vert(vids.at(3)),
                                                       this->vert(vids.at(4)),
                                                       this->vert(vids.at(5)),
                                                       this->vert(vids.at(6)),
                                                       this->vert(vids.at(7)));
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
void Hexmesh<M,V,E,F,P>::poly_subdivide(const std::vector<std::vector<std::vector<uint>>> & cell_split_scheme)
{
    std::vector<vec3d> new_verts;
    std::vector<uint>  new_cells;
    std::map<std::vector<uint>,uint> v_map;

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        std::vector<uint> vlist = this->poly_as_hex_vlist(pid);

        for(const auto & sub_cell: cell_split_scheme)
        {
            assert(sub_cell.size() == 8);
            for(uint off=0; off<8; ++off)
            {
                std::vector<uint> vids;
                for(uint i : sub_cell.at(off)) vids.push_back(vlist.at(i));
                sort(vids.begin(), vids.end());

                auto query = v_map.find(vids);
                if (query != v_map.end())
                {
                    new_cells.push_back(query->second);
                }
                else
                {
                    uint fresh_id = new_verts.size();
                    new_verts.push_back(verts_average(vids));
                    v_map[vids] = fresh_id;
                    new_cells.push_back(fresh_id);
                }
            }
        }
    }
    *this = Hexmesh<M,V,E,F,P>(new_verts,new_cells);
}

}
