/* This is a base application for cinolib (https://github.com/maxicino/cinolib).
 * It will show two GL canvases (and associated control panels), one to interact
 * with a triangle mesh, and the other to interact with a tetrahedral mesh.
 *
 * Different meshes could also be used. All you need to do is substitute DrawableTrimesh
 * with DrawableQuadmesh (if you are into quadmeshes), or with DrawablePolygonmesh
 * (if you are into general polygon meshes).
 *
 * Similarly, you can use DrawableHexmesh for hexahedral meshes, and DrawablePolyhedralmesh
 * for general polyhedral meshes (e.g., Voronoi cells).
 *
 * In case you don't need a GUI, you can drop the "Drawable" prefix from the data types.
 * What you will get is a set of lighter yet fully operational mesh data structures, just
 * without the burden of OpenGL code necessary for rendering!
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/surface_mesh_control_panel.h>
#include <cinolib/gui/qt/volume_mesh_control_panel.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::string s0 = std::string(DATA_PATH) + "/circle.obj";
    std::string s1 = std::string(DATA_PATH) + "/sphere.mesh";
    cinolib::DrawableTrimesh<> m_srf(s0.c_str());
    cinolib::DrawableTetmesh<> m_vol(s1.c_str());

    cinolib::GLcanvas gui_srf;
    gui_srf.resize(400,300);
    gui_srf.push_obj(&m_srf);
    gui_srf.show();

    cinolib::GLcanvas gui_vol;
    gui_vol.resize(400,300);
    gui_vol.push_obj(&m_vol);
    gui_vol.show();

    cinolib::VolumeMeshControlPanel<cinolib::DrawableTetmesh<>> vol_panel(&m_vol, &gui_vol);
    vol_panel.show();

    cinolib::SurfaceMeshControlPanel<cinolib::DrawableTrimesh<>> srf_panel(&m_srf, &gui_srf);
    srf_panel.show();

    return a.exec();
}
