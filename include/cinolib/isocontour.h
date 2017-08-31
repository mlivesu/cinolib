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
#ifndef CINO_ISOCONTOUR_H
#define CINO_ISOCONTOUR_H

#include <cinolib/cinolib.h>
#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

class Isocontour
{
    public:

        explicit Isocontour();
        explicit Isocontour(Trimesh<> & m, float iso_value, bool discard_boundary_edges = false);

        uint num_curves() const;

        // split triangles and edit the mesh connectivity in such a way
        // that there will be a chain of edges corresponding to isocurve
        //
        std::vector<uint> tessellate(Trimesh<> & m) const;

        // returns the centroid of the cc^th isocurve
        //
        vec3d curve_centroid(uint pid) const;

        // returns true if the cid^th curve is closed
        //
        bool curve_is_closed(uint pid) const;

        // returns true if the cid^th isocurve has less than <size> points
        //
        bool curve_is_shorter_than(uint pid, uint size) const;

        // TRUE if at least one of its curves is not closed or it is very short (< 5 samples)
        // FALSE otherwise
        //
        bool is_suspicious() const;

        // matches the curves of the current isoline with the curves of another isoline
        //
        void match(Isocontour & contour, std::set<std::pair<uint,uint>> & curve_matches) const;

        inline float value() const { return iso_value; }

        const std::vector<vec3d> & curve_vertices(const uint id) const { return curves_vertices.at(id); }

    protected:

        void compute_edges2samples_map();
        void make_iso_curve();

        std::map<uint,vec3d>::const_iterator next_edge(uint eid, const std::set<uint> & visited) const;

        std::map<uint,vec3d> edges2samples;
        std::map<uint,uint>  edges2curves;

        std::vector<std::vector<vec3d>> curves_vertices;
        std::vector<std::vector<uint>>  curves_edges; // edge-id for each edge of each curve

        Trimesh<> *m_ptr;
        float      iso_value;
        bool       discard_edges_incident_to_boundary;
};

}

#ifndef  CINO_STATIC_LIB
#include "isocontour.cpp"
#endif

#endif // CINO_ISOCONTOUR_H
