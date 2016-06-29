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
#include "write_VTK.h"

#ifdef CINOLIB_USES_VTK
#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#endif

namespace cinolib
{

CINO_INLINE
void write_VTK(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets,
               const std::vector<u_int>  & hexa)
{
#ifdef CINOLIB_USES_VTK

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
    vtkSmartPointer<vtkUnstructuredGrid>       grid   = vtkSmartPointer<vtkUnstructuredGrid>::New();
    vtkSmartPointer<vtkPoints>                 points = vtkSmartPointer<vtkPoints>::New();

    // write the vertex coordinates
    //
    for(size_t i=0; i<xyz.size(); i+=3)
    {
        points->InsertNextPoint(xyz[i+0], xyz[i+1], xyz[i+2]);
    }

    // write the tetrahedra
    //
    for(size_t i=0; i<tets.size(); i+=4)
    {
        vtkIdType pid[] = { tets[i+0], tets[i+1], tets[i+2], tets[i+3] };
        grid->InsertNextCell(VTK_TETRA, 4, pid);
    }

    // write the hexahedra
    //
    for(size_t i=0; i<hexa.size(); i+=8 )
    {
        vtkIdType pid[] = { hexa[i+0], hexa[i+1], hexa[i+2], hexa[i+3],
                            hexa[i+4], hexa[i+5], hexa[i+6], hexa[i+7] };
        grid->InsertNextCell(VTK_HEXAHEDRON, 8, pid);
    }

    // create the output mesh
    //
    grid->SetPoints(points);

#if VTK_MAJOR_VERSION < 6
    writer->SetInput(grid);
#else
    writer->SetInputData(grid);
#endif
    writer->SetFileName(filename);
    writer->Write();

#else
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << endl;
    exit(-1);
#endif
}

}
