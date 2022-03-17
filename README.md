# CinoLib
CinoLib is a C++ library for processing polygonal and polyhedral meshes. It supports surface meshes made of triangles, quads or general polygons as well as volumetric meshes made of tetrahedra, hexahedra or general polyhedra. 

A distinctive feature of the library is that all supported meshes inherit from a unique base class that implements their common traits, permitting to deploy algorithms that operate on _abstract_ meshes that may be any of the above. This allows to implement algorithms just once and run the same code on any possible mesh, thus avoiding code duplication and reducing the debugging effort.

<p align="center"><img src="cinolib_rep_image.png" width="500"></p>

## Getting started
CinoLib is header only. It does not need to be installed, all you have to do is to clone the repo with
```
git clone https://github.com/mlivesu/cinolib.git
```
and include in your C++ application the header files you need. For small projects this could already be done by instructing the compiler on where to find the library sources, e.g. with the `-I` option. For more convoluted projects it is suggested to rely on a building system such as [CMake](https://cmake.org), that can also handle optional external dependencies and compilation flags or symbols.

## Build a sample project (with CMake)
Here is an example of a toy program that reads a triangle mesh and displays it on a window
```c++
#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/gl/glcanvas.h>

int main()
{
    using namespace cinolib;
    DrawableTrimesh<> m("bunny.obj");
    GLcanvas gui;
    gui.push(&m);
    return gui.launch();
}
```
and this is the `CMakeLists.txt` that can be used to compile it
```cmake
cmake_minimum_required(VERSION 3.2)
project(cinolib_demo)
add_executable(${PROJECT_NAME} main.cpp)
set(CINOLIB_USES_OPENGL_GLFW_IMGUI ON)
find_package(cinolib REQUIRED)
target_link_libraries(${PROJECT_NAME} cinolib)
```
Compiling should be as easy as opening a terminal in the folder containing the two files above and type
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -Dcinolib_DIR=<absolute-path-to-cinolib>
make
```
Note that for the rendering part CinoLib uses [GLFW](https://www.glfw.org), which will be automatically installed and linked by the script `cinolib-config.cmake`, contained in the main directory of the library. The same script can automatically download and install any other external dependency, meaning that if you want to access a functionality that depends on some external library `XXX`, all you have to do is setting to `ON` a cmake variable that looks like `CINOLIB_USES_XXX`. 
Valid options are:
* `CINOLIB_USES_OPENGL_GLFW_IMGUI`, used for rendering with OpenGL
* `CINOLIB_USES_OPENGL_TRIANGLE`, used for polygon triangulation
* `CINOLIB_USES_OPENGL_TETGEN`, used for tetrahedralization
* `CINOLIB_USES_SHEWCHUK_PREDICATES`, used for exact geometric tests on input points
* `CINOLIB_USES_INDIRECT_PREDICATES`, used for exact geometric tests on implicit points
* `CINOLIB_USES_GRAPH_CUT`, used for graph clustering
* `CINOLIB_USES_BOOST`, used for 2D polygon operations (e.g. thickening, clipping, 2D booleans...)
* `CINOLIB_USES_VTK`, used just to support VTK file formats

## GUI
CinoLib is designed for researchers in computer graphics and geometry processing that need to quickly realize software prototypes that demonstate a novel algorithm or technique. In this context a simple OpenGL window and a side bar containing a few buttons and sliders are often more than enough. The library uses [ImGui](https://github.com/ocornut/imgui) for the GUI and [GLFW](https://www.glfw.org) for OpenGL rendering. Typical visual controls for the rendering of a mesh (e.g. shading, wireframe, texturing, planar slicing, ecc) are all encoded in two classes `cinolib::SurfaceMeshControls` and `cinolib::VolumeMeshControls`, that operate on surface and volume meshes respectively. To add a side bar that displays all such controls one can modify the sample progam above as follows:
```c++
#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

int main()
{
    using namespace cinolib;
    DrawableTrimesh<> m("bunny.obj");
    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> mesh_controls(&m, &gui);
    gui.push(&m);
    gui.push(&mesh_controls);
    return gui.launch();
}
```
The canvas can host multiple mesh controls, ideally one of each mesh in the scene. Additional GUI elements that may be necessary to control the application (e.g. the parameters of your algorithm) can be added by implementing a dedicated callback:
```c++
#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

int main()
{
    using namespace cinolib;
    DrawableTrimesh<> m("bunny.obj");
    GLcanvas gui;
    SurfaceMeshControls<DrawableTrimesh<>> mesh_controls(&m, &gui);
    gui.push(&m);
    gui.push(&mesh_controls);
    float val = 1.0, min = 0.0, max = 10.0;
    gui.callback_app_controls = [&]()
    {
        if(ImGui::Button("MyButton"))
        {
            // button clicked: do something
        }
        if(ImGui::SliderFloat("MySlider", &val, min, max))
        {
            // slider moved: do something
        }       
    };
    return gui.launch();
}
```
The full list of callbacks exposed by `GLcanvas` to interact with user events (e.g. for scene navigation, mouse picking, ecc) are:
* `callback_key_pressed(int key, int modifiers)`
* `callback_mouse_left_click(int modifiers)`
* `callback_mouse_left_click2(int modifiers)` => double click
* `callback_mouse_right_click(int modifiers)` 
* `callback_mouse_right_click2(int modifiers)` => double click
* `callback_mouse_moved(double x_pos, double y_pos)`
* `callback_mouse_scroll(double x_offset, double y_offset)`
* `callback_app_controls(void)`


## Other examples
A tutorial with detailed info on how to use the library is under developement. In the meanwhile, you can explore the [**examples**](https://github.com/mlivesu/cinolib/tree/master/examples#examples)  folder, which contains a constantly growing number of sample projects that showcase the core features of the library, and will be the backbone of the forthcoming tutorial.

## Contributors
Marco Livesu is the creator and lead developer of the library. Over the years various friends and colleagues have helped me to improve the codebase, either submitting code or helping me to spot and fix bugs. A big thanks goes to: Claudio Mancinelli (University of Genoa), Daniela Cabiddu (CNR IMATI), Chrystiano Araújo (UBC), Thomas Alderighi (CNR ISTI), Fabrizio Corda (University of Cagliari), Gianmarco Cherchi (University of Cagliari) and Tommaso Sorgente (CNR IMATI)

## Citing us
If you use CinoLib in your academic projects, please consider citing the library using the following 
BibTeX entry:

```bibtex
@article{cinolib,
  title   = {cinolib: a generic programming header only C++ library for processing polygonal and polyhedral meshes},
  author  = {Livesu, Marco},
  journal = {Transactions on Computational Science XXXIV},
  series  = {Lecture Notes in Computer Science},
  editor  = {Springer},
  note    = {https://github.com/mlivesu/cinolib/},
  year    = {2019},
  doi     = {10.1007/978-3-662-59958-7_4}}
```

## Acknowledgment
The software collected in CinoLib spans across a large period of time, starting from the beginning of my PhD to today. Since 2015, this work has been partly supported by various research projects, such as
* [CAxMan](https://cordis.europa.eu/project/id/680448)
* [CHANGE](https://cordis.europa.eu/project/rcn/204834/en)

