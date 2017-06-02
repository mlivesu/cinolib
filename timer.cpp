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
