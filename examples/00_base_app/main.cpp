/* This is a base application for cinolib (https://github.com/maxicino/cinolib).
 * It will show a GL canvas and a control panel to interact with a triangle mesh.
 *
 * Different meshes could also be used. All you need to do is substitute DrawableTrimesh
 * with DrawableQuadmesh (if you are into quadmeshes), or with DrawablePolygonmesh
 * (if you are into general polygon meshes).
 *
 * Similarly, we support volumetric meshes too! You can use DrawableTetmesh for tetrahedral
 * meshes, DrawableHexmesh for hexahedral meshes, and DrawablePolyhedralmesh for general
 * polyhedral meshes (e.g., Voronoi cells).
 *
 * In case you don't need a GUI, you can drop the "Drawable" prefix from the data types.
 * What you will get is lighter yet fully operational mesh data structure, just without 
 * the OpenGL code we use for rendering!
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/surface_mesh_control_panel.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    cinolib::GLcanvas gui;
    gui.resize(800,600);
    gui.show();

    cinolib::DrawableTrimesh<> m;
    cinolib::SurfaceMeshControlPanel<cinolib::DrawableTrimesh<>> m_controls(&m, &gui);
    m_controls.show();

    return a.exec();
}
