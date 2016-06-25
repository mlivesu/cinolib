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
#include "read_OBJ.h"
#include "read_OFF.h"
#include "read_IV.h"
//
// VOLUME READERS
#include "read_MESH.h"
#include "read_TET.h"
#include "read_VTU.h"
//
// SKELETON READERS
#include "read_LIVESU2012.h"
#include "read_TAGLIASACCHI2012.h"
#include "read_DEYSUN2006.h"
#include "read_CSV.h"

// SURFACE WRITERS
#include "write_OBJ.h"
#include "write_OFF.h"
//
// VOLUME WRITERS
#include "write_MESH.h"
#include "write_TET.h"
#include "write_VTU.h"
//
// SKELETON WRITERS
#include "write_LIVESU2012.h"

#endif // READ_WRITE
