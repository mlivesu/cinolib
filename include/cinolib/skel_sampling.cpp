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
#include <cinolib/skel_sampling.h>
#include <cinolib/stl_container_utilities.h>
#include <set>

namespace cinolib
{

CINO_INLINE
void recursive_mid_sampling(Skel          & skel,
                            int             bid,
                            int             beg_vid,
                            int             end_vid,
                            float           beg_sample,
                            float           end_sample,
                            std::set<int> & samples)
{
    assert(beg_sample < end_sample);

    float mid_sample = beg_sample + ((end_sample - beg_sample) * 0.5);
    int   mid_vid    = skel.sample_bone_at(bid, mid_sample);

    vec3d beg_pos = skel.vertex(beg_vid);
    vec3d mid_pos = skel.vertex(mid_vid);
    vec3d end_pos = skel.vertex(end_vid);

    double beg_r = skel.max_sphere_radius(beg_vid);
    double mid_r = skel.max_sphere_radius(mid_vid);
    double end_r = skel.max_sphere_radius(end_vid);

    if ((beg_pos - mid_pos).length() > beg_r + mid_r || // <= logic OR, remember that!
        (mid_pos - end_pos).length() > mid_r + end_r)   //    note to myself ;)
    {
        samples.insert(mid_vid);

        recursive_mid_sampling(skel, bid, beg_vid, mid_vid, beg_sample, mid_sample, samples);
        recursive_mid_sampling(skel, bid, mid_vid, end_vid, mid_sample, end_sample, samples);
    }
}

CINO_INLINE
void radius_based_mid_sampling(Skel & skel)
{
    // compute the new per bone sampling
    //
    std::set<int> new_samples;
    for(int bid=0; bid<skel.num_bones(); ++bid)
    {
        std::vector<int> bone = skel.vertex_bone(bid);

        new_samples.insert(bone.front());
        new_samples.insert(bone.back());

        recursive_mid_sampling(skel, bid, bone.front(), bone.back(), 0.0, 1.0, new_samples);
    }

    // remove old sampling
    //
    std::vector<int> to_remove;
    for(int vid=0; vid<skel.num_vertices(); ++vid)
    {
        if (DOES_NOT_CONTAIN(new_samples, vid)) to_remove.push_back(vid);
    }
    skel.remove_vertices(to_remove);

    // make sure that each bone has at least one subdivision
    //
    for(int bid=0; bid<skel.num_bones(); ++bid)
    {
        std::vector<int> bone = skel.vertex_bone(bid);
        if (bone.size() == 2) skel.sample_bone_at(bid, 0.5);
    }
}

}
