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
#include <cinolib/sampling.h>

namespace cinolib
{

CINO_INLINE
std::vector<float> sample_within_interval(const int min,
                                          const int max,
                                          const int n_samples)
{
    assert(n_samples >= 2);
    std::vector<float> samples;
    float step = static_cast<float>(max-min) / static_cast<float>(n_samples-1);
    float s    = min;

    do
    {
        samples.push_back(s);
        s += step;
    }
    while ((int)samples.size() < n_samples - 1);

    samples.push_back(max); // avoid to miss the exact boundaries due to roundoff errors...

    return samples;
}

}
