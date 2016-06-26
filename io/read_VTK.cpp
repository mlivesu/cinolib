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
#include "read_VTK.h"

#ifdef CINOLIB_USES_VTK
#include <vtkGenericDataObjectReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>
#endif

namespace cinolib
{

CINO_INLINE
void read_VTK(const char          * filename,
               std::vector<double> & xyz,
               std::vector<u_int>  & tets,
               std::vector<u_int>  & hexa)
{
#ifdef CINOLIB_USES_VTK

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkUnstructuredGrid> grid(reader->GetUnstructuredGridOutput());

    for(int i=0; i<grid->GetNumberOfPoints(); ++i)
    {
        double pnt[3];
        grid->GetPoint(i, pnt);

        xyz.push_back(pnt[0]);
        xyz.push_back(pnt[1]);
        xyz.push_back(pnt[2]);
    }

    for(int i=0; i<grid->GetNumberOfCells(); ++i)
    {
        vtkCell *c = grid->GetCell(i);

        switch (c->GetCellType())
        {
            case VTK_TETRA:      for(int j=0; j<4; ++j) tets.push_back(c->GetPointId(j)); break;
            case VTK_HEXAHEDRON: for(int j=0; j<8; ++j) hexa.push_back(c->GetPointId(j)); break;
        }
    }

#else
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << endl;
    exit(-1);
#endif
}

}
