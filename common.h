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
#ifndef CINO_COMMON_H
#define CINO_COMMON_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <eigen3/Eigen/Dense>

#include <cinolib/cinolib.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

typedef std::pair<uint,uint> ipair;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ipair unique_pair(uint v0, uint v1)
{
    ipair e;
    e.first  = std::min(v0,v1);
    e.second = std::max(v0,v1);
    return e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const ipair & p)
{
    in << "(" << p.first << "," << p.second << ") ";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef NDEBUG
template<typename C>
CINO_INLINE
void CHECK_BOUNDS(const C & container, int index)
{
    assert(index >= 0);
    assert(index < (int)container.size());
}
#else
template<typename C>
CINO_INLINE void CHECK_BOUNDS(const C &, int) {}
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool DOES_NOT_CONTAIN(const C & container, const E & element)
{
    return (container.find(element) == container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename C,typename E>
CINO_INLINE
bool CONTAINS(const C & container, const E & element)
{
    return (container.find(element) != container.end());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_std_3x3_to_Eigen_3x3(const double stdM[3][3], Eigen::Matrix3d & eigenM)
{
    eigenM.coeffRef(0,0) = stdM[0][0];  eigenM.coeffRef(0,1) = stdM[0][1];  eigenM.coeffRef(0,2) = stdM[0][2];
    eigenM.coeffRef(1,0) = stdM[1][0];  eigenM.coeffRef(1,1) = stdM[1][1];  eigenM.coeffRef(1,2) = stdM[1][2];
    eigenM.coeffRef(2,0) = stdM[2][0];  eigenM.coeffRef(2,1) = stdM[2][1];  eigenM.coeffRef(2,2) = stdM[2][2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_eigen_3x3_to_std_3x3(const Eigen::Matrix3d & eigenM, double stdM[3][3])
{
    stdM[0][0] = eigenM.coeffRef(0,0);  stdM[0][1] = eigenM.coeffRef(0,1);  stdM[0][2] = eigenM.coeffRef(0,2);
    stdM[1][0] = eigenM.coeffRef(1,0);  stdM[1][1] = eigenM.coeffRef(1,1);  stdM[1][2] = eigenM.coeffRef(1,2);
    stdM[2][0] = eigenM.coeffRef(2,0);  stdM[2][1] = eigenM.coeffRef(2,1);  stdM[2][2] = eigenM.coeffRef(2,2);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int serialize_2D_index(const int r, const int c, const int n_cols)
{
    return r * n_cols + c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ipair deserialize_2D_index(const int i, const int n_cols)
{
    return std::make_pair(i/n_cols, i%n_cols);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords[3*vid+0], coords[3*vid+1], coords[3*vid+2]);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xyz_from_vec3d(const std::vector<vec3d> & verts)
{
    uint nv = verts.size();
    std::vector<double> tmp(3*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(3*vid  ) = verts.at(vid).x();
        tmp.at(3*vid+1) = verts.at(vid).y();
        tmp.at(3*vid+2) = verts.at(vid).z();
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<std::vector<uint>> faces_from_serialized_vids(const std::vector<uint> & vids, const uint vids_per_face)
{
    uint nf = vids.size()/vids_per_face;
    std::vector<std::vector<uint>> tmp(nf);
    for(uint fid=0; fid<nf; ++fid)
    {
        for(uint off=0; off<vids_per_face; ++off) tmp.at(fid).push_back(vids.at(fid*vids_per_face+off));
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> serialized_vids_from_faces(const std::vector<std::vector<uint>> & faces)
{
    std::vector<uint> tmp;
    for(const auto & f   : faces)
    for(const auto & vid : f    )
    {
        tmp.push_back(vid);
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_hexahedra_to_general_polyhedra(const std::vector<uint>        & hexa,
                                         std::vector<std::vector<uint>> & faces,
                                         std::vector<std::vector<uint>> & polys,
                                         std::vector<std::vector<bool>> & polys_face_winding)
{
    faces.clear();
    polys.clear();
    polys_face_winding.clear();

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
                faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        polys.push_back(p_faces);
        polys_face_winding.push_back(p_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_hexahedra_to_general_polyhedra(const std::vector<std::vector<uint>> & hexa,
                                               std::vector<std::vector<uint>> & faces,
                                               std::vector<std::vector<uint>> & polys,
                                               std::vector<std::vector<bool>> & polys_face_winding)
{
    faces.clear();
    polys.clear();
    polys_face_winding.clear();

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
                faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        polys.push_back(p_faces);
        polys_face_winding.push_back(p_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_tetrahedra_to_general_polyhedra(const std::vector<uint>              & tets,
                                                std::vector<std::vector<uint>> & faces,
                                                std::vector<std::vector<uint>> & polys,
                                                std::vector<std::vector<bool>> & polys_face_winding)
{
    faces.clear();
    polys.clear();
    polys_face_winding.clear();

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
                faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        polys.push_back(p_faces);
        polys_face_winding.push_back(p_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_tetrahedra_to_general_polyhedra(const std::vector<std::vector<uint>> & tets,
                                                std::vector<std::vector<uint>> & faces,
                                                std::vector<std::vector<uint>> & polys,
                                                std::vector<std::vector<bool>> & polys_face_winding)
{
    faces.clear();
    polys.clear();
    polys_face_winding.clear();

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
                faces.push_back(f);
                p_faces.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p_faces.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        polys.push_back(p_faces);
        polys_face_winding.push_back(p_winding);
    }
}

}

#endif // CINO_COMMON_H
