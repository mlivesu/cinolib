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
#ifndef CINO_READ_WRITE_H
#define CINO_READ_WRITE_H

// SURFACE READERS
#include <cinolib/io/read_OBJ.h>
#include <cinolib/io/read_OFF.h>
#include <cinolib/io/read_IV.h>
//
// VOLUME READERS
#include <cinolib/io/read_MESH.h>
#include <cinolib/io/read_TET.h>
#include <cinolib/io/read_VTU.h>
#include <cinolib/io/read_VTK.h>
//
// SKELETON READERS
#include <cinolib/io/read_LIVESU2012.h>
#include <cinolib/io/read_TAGLIASACCHI2012.h>
#include <cinolib/io/read_DEYSUN2006.h>
#include <cinolib/io/read_CSV.h>

// SURFACE WRITERS
#include <cinolib/io/write_OBJ.h>
#include <cinolib/io/write_OFF.h>
//
// VOLUME WRITERS
#include <cinolib/io/write_MESH.h>
#include <cinolib/io/write_TET.h>
#include <cinolib/io/write_VTU.h>
#include <cinolib/io/write_VTK.h>
//
// SKELETON WRITERS
#include <cinolib/io/write_LIVESU2012.h>

#endif // CINO_READ_WRITE
