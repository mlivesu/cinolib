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
#ifndef CINO_TEXTURES_H
#define CINO_TEXTURES_H

#include <cinolib/textures/isolines_texture.h>
#include <cinolib/textures/quality_ramp_texture.h>
#include <cinolib/textures/quality_ramp_texture_plus_isolines.h>

enum
{
    TEXTURE_ISOLINES,
    TEXTURE_QUALITY_RAMP,
    TEXTURE_QUALITY_RAMP_W_ISOLINES
};

#endif // CINO_TEXTURES_H
