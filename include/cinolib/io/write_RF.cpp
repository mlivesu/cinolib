/********************************************************************************
 *  This file is part of CinoLib * Copyright(C) 2019: Tommaso Sorgente
 *                                                                               *
 *  The MIT License *
 *                                                                               *
 *  Permission is hereby granted, free of charge, to any person obtaining a *
 *  copy of this software and associated documentation files (the "Software"), *
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, *
 *  and/or sell copies of the Software, and to permit persons to whom the *
 *  Software is furnished to do so, subject to the following conditions: *
 *                                                                               *
 *  The above copyright notice and this permission notice shall be included in *
 *  all copies or substantial portions of the Software. *
 *                                                                               *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, *
 *  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING *
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 ** IN THE SOFTWARE. *
 *                                                                               *
 *  Author(s): *
 *                                                                               *
 *     Tommaso Sorgente (tommaso.sorgente@.cnr.it)                         *
 *                                                                               *
 *     Italian National Research Council (CNR) * Institute for Applied
 *Mathematics and Information Technologies (IMATI)    * Via de Marini, 6 * 16149
 *Genoa,                                                              * Italy *
 *********************************************************************************/

#include <cinolib/io/write_RF.h>
#include <iostream>

namespace cinolib {

CINO_INLINE
void write_RF(const char *basename, const Polyhedralmesh<> &m)
{
  // output file for nodes coordinates
  std::string fnode = basename + std::string(".node");
  std::ofstream out_node(fnode.c_str());
  uint offset = 0;
  out_node << "# *.node file of 3D-mesh in REGN_FACE format" << std::endl;
  out_node << m.num_verts() << " 3 0 0 " << std::endl;

  // write the nodes coordinates
  for (uint vid = 0; vid < m.num_verts(); ++vid)
  {
      out_node << (vid + offset) << " " << m.vert(vid).x()
               << " " << m.vert(vid).y() << " " << m.vert(vid).z() << std::endl;
  }
  out_node.close();

  // output file for elements structure
  std::string fele = basename + std::string(".ele");
  std::ofstream out_ele(fele.c_str());
  out_ele << "# *.ele file of 3D-mesh in REGN_FACE format" << std::endl;
  out_ele << m.num_polys() << " 0" << std::endl;

  // write the elements structure
  for (uint pid = 0; pid < m.num_polys(); ++pid)
  {
    std::vector<uint> flist = m.adj_p2f(pid);
    uint nf = m.faces_per_poly(pid);
    out_ele << pid + offset << " " << nf << std::endl; // element summary

    for (uint f = 0; f < flist.size(); f++)
    {
      uint fid = flist.at(f);
      std::vector<uint> vlist = m.adj_f2v(fid);
      uint nv = vlist.size();
      out_ele << " " << f + offset << " " << nv << " "; // face summary

      for (uint vid = 0; vid < nv; ++vid) // list of nodes in CCW order
      {
          uint v = m.poly_face_is_CCW(pid, fid) ? vlist[vid] : vlist[nv - 1 - vid];
          out_ele << " " << v + offset;
      }
      out_ele << std::endl;
    }
  }
  out_ele.close();
}

} // namespace cinolib
