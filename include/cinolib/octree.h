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
#ifndef CINO_OCTREE_H
#define CINO_OCTREE_H

#include <cinolib/geometry/spatial_data_structure_item.h>
#include <cinolib/meshes/meshes.h>
#include <queue>

namespace cinolib
{

class OctreeNode
{
    public:
        OctreeNode(const OctreeNode * father, const AABB & bbox) : father(father), bbox(bbox) {}
       ~OctreeNode();
        const OctreeNode *father;
        OctreeNode       *children[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        bool              is_inner = false;
        AABB              bbox;
        std::vector<uint> item_indices; // index Octree::items, avoiding to store a copy of the same object multiple times in each node it appears
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* Usage:
 *
 *  i)   Create an empty octree
 *  ii)  Use the add_segment/triangle/tetrahedron facilities to populate it
 *  iii) Call build to make the tree
*/

class Octree
{

    public:

        explicit Octree(const uint max_depth      = 7,
                        const uint items_per_leaf = 10);

        virtual ~Octree();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void add_segment    (const uint id, const std::vector<vec3d> & v);
        void add_triangle   (const uint id, const std::vector<vec3d> & v);
        void add_tetrahedron(const uint id, const std::vector<vec3d> & v);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void build();
        void build_item(const uint id, OctreeNode *node, const uint depth);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void build_from_mesh_polys(const AbstractPolygonMesh<M,V,E,P> & m)
        {
            assert(items.empty());
            items.reserve(m.num_polys());
            for(uint pid=0; pid<m.num_polys(); ++pid)
            {
                for(uint i=0; i<m.poly_tessellation(pid).size()/3; ++i)
                {
                    vec3d v0 = m.vert(m.poly_tessellation(pid).at(3*i+0));
                    vec3d v1 = m.vert(m.poly_tessellation(pid).at(3*i+1));
                    vec3d v2 = m.vert(m.poly_tessellation(pid).at(3*i+2));
                    add_triangle(pid, {v0,v1,v2});
                }
            }
            build();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void build_from_mesh_polys(const AbstractPolyhedralMesh<M,V,E,P> & m)
        {
            assert(items.empty());
            items.reserve(m.num_polys());
            for(uint pid=0; pid<m.num_polys(); ++pid)
            {
                switch(m.mesh_type())
                {
                    case TETMESH : add_tetrahedron(pid, m.poly_verts(pid)); break;
                    default: assert(false && "Unsupported element");
                }
            }
            build();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void build_from_mesh_edges(const AbstractMesh<M,V,E,P> & m)
        {
            assert(items.empty());
            items.reserve(m.num_edges());
            for(uint eid=0; eid<m.num_edges(); ++eid)
            {
                add_segment(eid, m.edge_verts(eid));
            }
            build();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint max_items_per_leaf() const;
        uint max_items_per_leaf(const OctreeNode *node, const uint max) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void debug_mode(const bool b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void print_query_info(const std::string & s,
                              const double        t,
                              const uint          aabb_queries,
                              const uint          item_queries) const;

        // QUERIES :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // returns pos, id and distance of the item that is closest to query point p
        void  closest_point(const vec3d & p, uint & id, vec3d & pos, double & dist) const;
        vec3d closest_point(const vec3d & p) const;

        // returns respectively the first item and the full list of items containing query point p
        bool contains(const vec3d & p, uint & id, const double eps = 1e-15) const;
        bool contains(const vec3d & p, std::unordered_set<uint> & ids, const double eps = 1e-15) const;

        // returns respectively the first and the full list of intersections
        // between items in the octree and a ray R(t) := p + t * dir
        bool intersects_ray(const vec3d & p, const vec3d & dir, double & min_t, uint & id) const; // first hit
        bool intersects_ray(const vec3d & p, const vec3d & dir, std::set<std::pair<double,uint>> & all_hits) const;

        // EXACT QUERIES: based on exact geometric predicates //
        bool contains_exact           (const vec3d & p,   std::unordered_set<uint> & ids, const bool strict) const;
        bool intersects_segment_exact (const vec3d   s[], std::unordered_set<uint> & ids) const;
        bool intersects_triangle_exact(const vec3d   t[], std::unordered_set<uint> & ids) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        // all items and aabbs live here, and tree leaf nodes only store indices of these vectors
        std::vector<SpatialDataStructureItem*> items;
        std::vector<AABB>                      aabbs;
        OctreeNode                            *root = nullptr;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint max_depth;      // maximum allowed depth of the tree
        uint items_per_leaf; // prescribed number of items per leaf (can't go deeper than max_depth anyways)
        uint tree_depth = 0; // actual depth of the tree
        uint num_leaves = 0; // actual number of leaves
        bool print_debug_info = false;

        // SUPPORT STRUCTURES ::::::::::::::::::::::::::::::::::::::::::::::::::::

        struct Obj
        {
            double      dist  = inf_double;
            OctreeNode *node  = nullptr;
            int         index = -1; // note: this is the item ID, NOT necessarily the index of vector items!!
            vec3d       pos;        // closest point
        };
        struct Greater
        {
            bool operator()(const Obj & obj1, const Obj & obj2)
            {
                return obj1.dist > obj2.dist;
            }
        };
        typedef std::priority_queue<Obj,std::vector<Obj>,Greater> PrioQueue;
};

}

#ifndef  CINO_STATIC_LIB
#include "octree.cpp"
#endif

#endif // CINO_OCTREE_H
