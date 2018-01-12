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
#ifndef CINO_OCTREE_H
#define CINO_OCTREE_H

#include <cinolib/geometry/vec3.h>
#include <set>
#include <vector>
#include <assert.h>

namespace cinolib
{

template<typename item_type> class Octree
{
    private:

        vec3d                  bb_min;
        vec3d                  bb_max;
        std::vector<Octree>    children;
        std::vector<item_type> item_list;

    public:

        explicit Octree() {}

        explicit Octree(const vec3d & minim, const vec3d & maxim)
        {
            set_extents(minim, maxim);
        }

        void set_extents(const vec3d & minim, const vec3d & maxim )
        {
            bb_min = minim;
            bb_max = maxim;
        }

        void subdivide()
        {
            assert(children.empty());

            vec3d bb_avg = (bb_min + bb_max)/2.0;

            children.resize(8);
            children[0].set_extents(vec3d(bb_min[0], bb_min[1], bb_min[2]),
                                    vec3d(bb_avg[0], bb_avg[1], bb_avg[2]));
            children[1].set_extents(vec3d(bb_avg[0], bb_min[1], bb_min[2]),
                                    vec3d(bb_max[0], bb_avg[1], bb_avg[2]));
            children[2].set_extents(vec3d(bb_avg[0], bb_avg[1], bb_min[2]),
                                    vec3d(bb_max[0], bb_max[1], bb_avg[2]));
            children[3].set_extents(vec3d(bb_min[0], bb_avg[1], bb_min[2]),
                                    vec3d(bb_avg[0], bb_max[1], bb_avg[2]));
            children[4].set_extents(vec3d(bb_min[0], bb_min[1], bb_avg[2]),
                                    vec3d(bb_avg[0], bb_avg[1], bb_max[2]));
            children[5].set_extents(vec3d(bb_avg[0], bb_min[1], bb_avg[2]),
                                    vec3d(bb_max[0], bb_avg[1], bb_max[2]));
            children[6].set_extents(vec3d(bb_avg[0], bb_avg[1], bb_avg[2]),
                                    vec3d(bb_max[0], bb_max[1], bb_max[2]));
            children[7].set_extents(vec3d(bb_min[0], bb_avg[1], bb_avg[2]),
                                    vec3d(bb_avg[0], bb_max[1], bb_max[2]));
        }

        void subdivide_n_levels(const uint n)
        {
            assert(children.empty());

            subdivide();

            if (n > 1)
            {
                for(auto & child : children) child.subdivide_n_levels(n-1);
            }
        }

        void get_items(const vec3d & query, std::set<item_type> & items) const
        {
            double eps = 1e-3;

            if (query[0] >= bb_min[0] - eps && query[1] >= bb_min[1] - eps && query[2] >= bb_min[2] - eps &&
                query[0] <= bb_max[0] + eps && query[1] <= bb_max[1] + eps && query[2] <= bb_max[2] + eps)
            {
                if (children.empty())
                {
                    for(auto it : item_list) items.insert(it);
                }
                else
                {
                    assert(children.size() == 8);
                    for(const auto & child : children) child.get_items(query, items);
                }
            }
        }

        void add_item(const item_type & item, const vec3d & minim, const vec3d & maxim)
        {
            if (minim[0] <= bb_max[0] && minim[1] <= bb_max[1] && minim[2] <= bb_max[2] &&
                maxim[0] >= bb_min[0] && maxim[1] >= bb_min[1] && maxim[2] >= bb_min[2] )
            {
                item_list.push_back(item);

                if (!children.empty())
                {
                    assert(children.size() == 8);
                    for(auto & child : children) child.add_item(item, minim, maxim);
                }
            }
        }
};

}

#endif // CINO_OCTREE_H
