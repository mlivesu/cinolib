/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2019: Tommaso Sorgente
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
*     Tommaso Sorgente (tommaso.sorgente@.cnr.it)                         *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/

#include <cinolib/io/write_RF.h>
#include <iostream>

using std::cout;
using std::endl;
using std::flush;
using std::setprecision;
using std::setw;

namespace cinolib
{

CINO_INLINE
void write_RF(const char *basename, const Polyhedralmesh<> &m)
{
  cout << "writing \"" << basename << "\"..." << flush;

  // open output file for node's coordinates
  std::string fnode = basename + std::string(".node");
  std::ofstream out_node(fnode.c_str());
  uint offset = 0;

  // output the node file's header
  out_node << "# *.node file of 3D-mesh in REGN_FACE format " << endl;
  out_node << m.num_verts() << "  3  0  0 " << endl;

  // output the node's coordinates
  for (uint vid = 0; vid < m.num_verts(); ++vid)
    out_node << setw(8) << vid + offset << "     " << setw(10)
             << setprecision(12) << m.vert(vid).x() << "   " << setw(10)
             << setprecision(12) << m.vert(vid).y() << "   " << setw(10)
             << setprecision(12) << m.vert(vid).z() << endl;
  out_node.close();

  // open output file for element's structure
  std::string fele = basename + std::string(".ele");
  std::ofstream out_ele(fele.c_str());

  // output the element file's header
  out_ele << "# *.ele file of 3D-mesh in REGN_FACE format " << endl;
  out_ele << m.num_polys() << "  0 " << endl;

  // output the element's structure
  for (uint pid = 0; pid < m.num_polys(); ++pid) {
    std::vector<uint> flist = m.adj_p2f(pid); // get the list of face IDs
    uint nf = m.faces_per_poly(pid); // set the number of the local faces
    out_ele << pid + offset << "  " << nf << endl; // write region header

    for (uint i = 0; i < flist.size(); i++) {   // write the face's node
      uint fid = flist.at(i);
      std::vector<uint> vlist = m.adj_f2v(fid); // get face's node list
      uint nv = vlist.size();                   // get the face size
      out_ele << "  " << i + offset << "  " << nv << "  ";

      if (m.poly_face_is_CCW(pid, fid)) // write nodes counterclockwise
        for (uint vid = 0; vid < nv; ++vid)
          out_ele << "  " << vlist[vid] + offset;
      else
        for (uint vid = 0; vid < nv; ++vid)
          out_ele << "  " << vlist[nv - 1 - vid] + offset;
      out_ele << endl;
    }
  }
  out_ele.close();
  cout << "done" << endl << flush;
}

}
