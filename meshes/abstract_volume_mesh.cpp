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
#include <cinolib/meshes/abstract_volume_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    //
    faces.clear();
    triangulated_faces.clear();
    polys_face_winding.clear();
    //
    v_on_srf.clear();
    e_on_srf.clear();
    f_on_srf.clear();
    //
    f_data.clear();
    //
    v2f.clear();
    e2f.clear();
    f2e.clear();
    f2f.clear();
    f2p.clear();
    p2v.clear();
}

}
