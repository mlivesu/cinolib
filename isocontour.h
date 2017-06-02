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
#ifndef CINO_ISOCONTOUR_H
#define CINO_ISOCONTOUR_H

#include <cinolib/cinolib.h>
#include <cinolib/meshes/trimesh/trimesh.h>

namespace cinolib
{

class Isocontour
{
    public:

        Isocontour();
        Isocontour(Trimesh<> & m, float iso_value, bool discard_boundary_edges = false);

        int num_curves() const;

        // split triangles and edit the mesh connectivity in such a way
        // that there will be a chain of edges corresponding to isocurve
        //
        std::vector<int> tessellate(Trimesh<> & m) const;

        // returns the centroid of the cc^th isocurve
        //
        vec3d curve_centroid(int cid) const;

        // returns true if the cid^th curve is closed
        //
        bool curve_is_closed(int cid) const;

        // returns true if the cid^th isocurve has less than <size> points
        //
        bool curve_is_shorter_than(int cid, int size) const;

        // TRUE if at least one of its curves is not closed or it is very short (< 5 samples)
        // FALSE otherwise
        //
        bool is_suspicious() const;

        // matches the curves of the current isoline with the curves of another isoline
        //
        void match(Isocontour & contour, std::set< std::pair<int,int> > & curve_matches) const;


        inline float value() const { return iso_value; }

    protected:

        void compute_edges2samples_map();
        void make_iso_curve();

        std::map<int,vec3d>::const_iterator next_edge(int eid, const std::set<int> & visited) const;

        std::map<int,vec3d> edges2samples;
        std::map<int,int>   edges2curves;

        std::vector< std::vector<vec3d> > curves_vertices;
        std::vector< std::vector<int> >   curves_edges; // edge-id for each edge of each curve

        Trimesh<> *m_ptr;
        float      iso_value;
        bool       discard_edges_incident_to_boundary;
};

}

#ifndef  CINO_STATIC_LIB
#include "isocontour.cpp"
#endif

#endif // CINO_ISOCONTOUR_H
