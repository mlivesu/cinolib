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
#include <cinolib/timer.h>

#include <time.h>
#include <iostream>
#include <map>

namespace cinolib
{

static std::map<int,clock_t>      t_start;
static std::map<int,clock_t>      t_stop;
static std::map<int,std::string>  msgs;
static int                        first_call = 0;

////////////////////////////////////////////////////////////////////////////////////////

// See: http://stackoverflow.com/questions/98153/whats-the-best-hashing-algorithm-to-use-on-a-stl-string-when-using-hash-map
//
CINO_INLINE
unsigned int hash(const char * s, unsigned int seed = 0)
{
    unsigned hash = seed;
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
}

inline float secs(unsigned int key)
{
    return ((float)(t_stop[key] - t_start[key])) / ((float)CLOCKS_PER_SEC);
}

////////////////////////////////////////////////////////////////////////////////////////

CINO_INLINE
void timer_start(const char * msg)
{
    unsigned int key = hash(msg);
    t_start[key] = clock();
    msgs[key]        = msg;

    if (first_call == 0) first_call = key;
}

CINO_INLINE
void timer_stop(const char * msg)
{
    int key = hash(msg);
    t_stop[key]  = clock();

    if (key == first_call)
    {
        logger << "\n" << msgs[key] << " (" << secs(key) << " secs)" << endl;

        for(auto obj : t_start)
        {
            if (obj.first != first_call)
            {
                logger << "   - " << msgs[key] << " (" << secs(key) << " secs)" << endl;
            }
        }
        logger << endl;

        // reset timer
        //
        t_start.clear();
        t_stop.clear();
        msgs.clear();
        first_call = 0;
    }
}

}
