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
#ifndef READ_WRITE_H
#define READ_WRITE_H

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

#endif // READ_WRITE
