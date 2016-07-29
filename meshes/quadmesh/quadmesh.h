/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef QUADMESH_H
#define QUADMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>

#include <assert.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>


namespace cinolib
{

static const int QUAD_EDGES[4][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 3 }, // 2
    { 3, 0 }, // 3
};

class Quadmesh
{
    public:

        Quadmesh(){}

        Quadmesh(const char * filename);

        Quadmesh(const std::vector<double> & coords,
                 const std::vector<u_int>  & quads);

    protected:

        std::string filename;

        // bounding box
        //
        Bbox bb;

        // serialized xyz coordinates, quads and edges
        //
        std::vector<double> coords;
        std::vector<u_int>  quads;
        std::vector<u_int>  edges;

        // per vertex/triangle normals
        //
        std::vector<double> v_norm;
        std::vector<double> q_norm;

        // general purpose float and int scalars
        //
        std::vector<float> u_text;  // per vertex: 1D texturing
        std::vector<int>   q_label; // per triangle

        // adjacencies
        //
        std::vector< std::vector<int> > vtx2vtx;
        std::vector< std::vector<int> > vtx2edg;
        std::vector< std::vector<int> > vtx2quad;
        std::vector< std::vector<int> > edg2quad;
        std::vector< std::vector<int> > quad2edg;
        std::vector< std::vector<int> > quad2quad;

        void load(const char * filename);
        void init();

    public:

        void                        clear();
        void                        update_adjacency();
        void                        update_normals();
        void                        update_q_normals();
        void                        update_v_normals();
        void                        update_bbox();
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        std::string                 loaded_file() const;
        void                        save(const char * filename) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        int                         num_vertices()  const;
        int                         num_quads()     const;
        int                         num_elements()  const;
        int                         num_edges()     const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        const std::vector<int> &    adj_vtx2vtx(const int vid) const;
        const std::vector<int> &    adj_vtx2edg(const int vid) const;
        const std::vector<int> &    adj_vtx2quad(const int vid) const;
        const std::vector<int> &    adj_edg2quad(const int eid) const;
        const std::vector<int> &    adj_quad2edg(const int qid) const;
        const std::vector<int> &    adj_quad2quad(const int qid) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        const Bbox                & bbox()                  const;
        const std::vector<double> & vector_coords()         const;
        const std::vector<u_int>  & vector_quads()          const;
        const std::vector<u_int>  & vector_edges()          const;
        const std::vector<float>  & vector_v_float_scalar() const;
        const std::vector<int>    & vector_q_int_scalar()   const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        int                         edge_vertex_id(const int eid, const int offset) const;
        vec3d                       edge_vertex(const int eid, const int offset) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        int                         quad_vertex_id(const int qid, const int offset) const;
        vec3d                       quad_vertex(const int qid, const int offset) const;
        vec3d                       quad_normal(const int qid) const;
        int                         quad_label(const int qid) const;
        void                        quad_set_label(const int qid, const int i);
        bool                        quad_contains_vertex(const int qid, const int vid) const;
        vec3d                       element_barycenter(const int qid) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        double                      vertex_mass (const int vid) const;
        vec3d                       vertex_normal(const int vid) const;
        vec3d                       vertex(const int vid) const;
        void                        set_vertex(const int vid, const vec3d & pos);
        int                         vertex_valence(const int vid) const;
        float                       vertex_u_text(const int vid) const;
        void                        set_vertex_u_text(const int vid, const float s);
        bool                        vertex_is_singular(const int vid);
};

}

#ifndef  CINO_STATIC_LIB
#include "quadmesh.cpp"
#endif

#endif // QUADMESH_H
