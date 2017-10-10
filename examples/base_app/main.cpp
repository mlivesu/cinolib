/* This is a base application for cinolib (https://github.com/maxicino/cinolib).
 * It will show a GL canvas and a control panel to interact with a triangle mesh.
 *
 * Different meshes could also be used. All you need to do is substitute DrawableTrimesh
 * with DrawableQuadmesh (if you are into quadmeshes), or with DrawablePolygonmesh
 * (if you are into general polygon meshes). Enjoy!
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
