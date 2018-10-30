/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/io/write_VTK.h>


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

#ifdef CINOLIB_USES_VTK

CINO_INLINE
void write_VTK(const char                           * filename,
               const std::vector<vec3d>             & verts,
               const std::vector<std::vector<uint>> & polys)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
    vtkSmartPointer<vtkUnstructuredGrid>       grid   = vtkSmartPointer<vtkUnstructuredGrid>::New();
    vtkSmartPointer<vtkPoints>                 points = vtkSmartPointer<vtkPoints>::New();

    // generate some arrays that allow each element type to be viewed alone by thresholding
    //
    vtkSmartPointer<vtkIntArray> tetselector = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> hexselector = vtkSmartPointer<vtkIntArray>::New();
    tetselector->SetName("tet_selector");
    hexselector->SetName("hex_selector");

    for(const vec3d & v : verts)
    {
        points->InsertNextPoint(v.x(), v.y(), v.z());
    }

    bool has_tets = false;
    bool has_hexa = false;

    for(auto p : polys)
    {
        switch (p.size())
        {
            case 4:
            {
                vtkIdType pid[] = { p.at(0), p.at(1), p.at(2), p.at(3) };
                grid->InsertNextCell(VTK_TETRA, 4, pid);
                tetselector->InsertNextValue(1);
                hexselector->InsertNextValue(0);
                has_tets = true;
                break;
            }

            case 8:
            {
                vtkIdType pid[] = { p.at(0), p.at(1), p.at(2), p.at(3),
                                    p.at(4), p.at(5), p.at(6), p.at(7) };
                grid->InsertNextCell(VTK_HEXAHEDRON, 8, pid);
                tetselector->InsertNextValue(0);
                hexselector->InsertNextValue(1);
                has_hexa = true;
                break;
            }

            default: assert(false && "Unsupported Polyhedron!");
        }
    }

    grid->SetPoints(points);
    if (has_tets) grid->GetCellData()->AddArray(tetselector);
    if (has_hexa) grid->GetCellData()->AddArray(hexselector);

#if VTK_MAJOR_VERSION < 6
    writer->SetInput( grid );
#else
    writer->SetInputData(grid);
#endif
    writer->SetFileName(filename);
    writer->Write();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_VTK(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<uint>   & tets,
               const std::vector<uint>   & hexa)
{
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
}

#else

CINO_INLINE
void write_VTK(const char                *,
               const std::vector<double> &,
               const std::vector<uint>   &,
               const std::vector<uint>   &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << std::endl;
    exit(-1);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_VTK(const char                           *,
               const std::vector<vec3d>             &,
               const std::vector<std::vector<uint>> &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << std::endl;
    exit(-1);
}

#endif

}
