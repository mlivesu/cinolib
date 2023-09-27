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
#ifndef CINO_READ_WRITE_H
#define CINO_READ_WRITE_H

// SURFACE READERS
#include <cinolib/io/read_OBJ.h>
#include <cinolib/io/read_OFF.h>
#include <cinolib/io/read_IV.h>
#include <cinolib/io/read_STL.h>
// SURFACE WRITERS
#include <cinolib/io/write_OBJ.h>
#include <cinolib/io/write_OFF.h>
#include <cinolib/io/write_STL.h>
#include <cinolib/io/write_NODE_ELE.h>


// VOLUME READERS
#include <cinolib/io/read_HEDRA.h>
#include <cinolib/io/read_HYBRID.h>
#include <cinolib/io/read_MESH.h>
#include <cinolib/io/read_TET.h>
#include <cinolib/io/read_VTU.h>
#include <cinolib/io/read_VTK.h>
#include <cinolib/io/read_HEXEX.h>
// VOLUME WRITERS
#include <cinolib/io/write_HEDRA.h>
#include <cinolib/io/write_MESH.h>
#include <cinolib/io/write_TET.h>
#include <cinolib/io/write_VTU.h>
#include <cinolib/io/write_VTK.h>
#include <cinolib/io/write_OVM.h>


// SKELETON READERS
#include <cinolib/io/read_LIVESU2012.h>
#include <cinolib/io/read_TAGLIASACCHI2012.h>
#include <cinolib/io/read_DEYSUN2006.h>
#include <cinolib/io/read_CSV.h>
// SKELETON WRITERS
#include <cinolib/io/write_LIVESU2012.h>

#endif // CINO_READ_WRITE
