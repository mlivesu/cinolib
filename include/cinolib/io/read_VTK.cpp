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
#include <cinolib/io/read_VTK.h>

#ifdef CINOLIB_USES_VTK
#include <vtkGenericDataObjectReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>
#endif

namespace cinolib
{

#ifdef CINOLIB_USES_VTK

CINO_INLINE
void read_VTK(const char                      * filename,
               std::vector<vec3d>             & verts,
               std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkUnstructuredGrid> grid(reader->GetUnstructuredGridOutput());

    for(uint i=0; i<grid->GetNumberOfPoints(); ++i)
    {
        double pnt[3];
        grid->GetPoint(i, pnt);

        verts.push_back(vec3d(pnt[0],pnt[1],pnt[2]));
    }

    for(uint i=0; i<grid->GetNumberOfCells(); ++i)
    {
        vtkCell *c = grid->GetCell(i);

        std::vector<uint> polyhedron;
        switch (c->GetCellType())
        {
            case VTK_TETRA:      for(uint j=0; j<4; ++j) polyhedron.push_back(c->GetPointId(j)); break;
            case VTK_HEXAHEDRON: for(uint j=0; j<8; ++j) polyhedron.push_back(c->GetPointId(j)); break;
        }

        if(!polyhedron.empty()) poly.push_back(polyhedron);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_VTK(const char                      * filename,
               std::vector<double>            & xyz,
               std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkUnstructuredGrid> grid(reader->GetUnstructuredGridOutput());

    for(uint i=0; i<grid->GetNumberOfPoints(); ++i)
    {
        double pnt[3];
        grid->GetPoint(i, pnt);

        xyz.push_back(pnt[0]);
        xyz.push_back(pnt[1]);
        xyz.push_back(pnt[2]);
    }

    for(uint i=0; i<grid->GetNumberOfCells(); ++i)
    {
        vtkCell *c = grid->GetCell(i);

        std::vector<uint> polyhedron;
        switch (c->GetCellType())
        {
            case VTK_TETRA:      for(uint j=0; j<4; ++j) polyhedron.push_back(c->GetPointId(j)); break;
            case VTK_HEXAHEDRON: for(uint j=0; j<8; ++j) polyhedron.push_back(c->GetPointId(j)); break;
        }

        if(!polyhedron.empty()) poly.push_back(polyhedron);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_VTK(const char          * filename,
               std::vector<double> & xyz,
               std::vector<uint>  & tets,
               std::vector<uint>  & hexa)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkUnstructuredGrid> grid(reader->GetUnstructuredGridOutput());

    for(uint i=0; i<grid->GetNumberOfPoints(); ++i)
    {
        double pnt[3];
        grid->GetPoint(i, pnt);

        xyz.push_back(pnt[0]);
        xyz.push_back(pnt[1]);
        xyz.push_back(pnt[2]);
    }

    for(uint i=0; i<grid->GetNumberOfCells(); ++i)
    {
        vtkCell *c = grid->GetCell(i);

        switch (c->GetCellType())
        {
            case VTK_TETRA:      for(uint j=0; j<4; ++j) tets.push_back(c->GetPointId(j)); break;
            case VTK_HEXAHEDRON: for(uint j=0; j<8; ++j) hexa.push_back(c->GetPointId(j)); break;
        }
    }

}

#else

CINO_INLINE
void read_VTK(const char          *,
               std::vector<double> &,
               std::vector<uint>   &,
               std::vector<uint>   &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << std::endl;
    exit(-1);
}

CINO_INLINE
void read_VTK(const char                      *,
               std::vector<double>            &,
               std::vector<std::vector<uint>> &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << std::endl;
    exit(-1);
}

CINO_INLINE
void read_VTK(const char                      *,
               std::vector<vec3d>             &,
               std::vector<std::vector<uint>> &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << std::endl;
    exit(-1);
}

#endif

}
