/* This sample program computes a Least Squares Conformal Map (LSCM)
 * of a disk-like triangulated surface.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QHBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/lscm.h>
#include <cinolib/gui/load_texture.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/maxFace.obj";
    DrawableTrimesh<> m_xyz(s.c_str());

    // solve for the interior vertices via harmonic map
    Profiler profiler;
    profiler.push("LSCM");
    ScalarField uv = LSCM(m_xyz);
    profiler.pop();
    uv.copy_to_mesh(m_xyz, UV_param);

    // create a mesh of the uv parametric space (same connectivity, different embedding)
    DrawableTrimesh<> m_uv = m_xyz;
    m_uv.copy_uvw_to_xyz(UV_param);
    for(uint vid=0; vid<m_uv.num_verts(); ++vid) m_uv.vert(vid).z() = 0.0;
    m_uv.normalize_bbox();
    m_uv.update_normals();
    m_uv.updateGL();

    // visualize original and parametric mesh in two separated windows
    QWidget window;
    GLcanvas gui_xyz;
    GLcanvas gui_uvw;
    QHBoxLayout layout;
    layout.addWidget(&gui_xyz);
    layout.addWidget(&gui_uvw);
    window.setLayout(&layout);
    m_xyz.show_wireframe(true);
    m_xyz.show_wireframe_transparency(0.2);
    m_xyz.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1.0);
    gui_xyz.push_obj(&m_xyz);
    m_uv.show_wireframe(true);
    m_uv.show_wireframe_transparency(0.2);
    m_uv.show_texture2D(TEXTURE_2D_CHECKERBOARD, 1.0);
    gui_uvw.push_obj(&m_uv);
    window.resize(800,600);
    window.show();

    // CMD+1 to show XYZ mesh controls.
    // CMD+2 to show UVW mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_xyz(&m_xyz, &gui_xyz);
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_uvw(&m_uv, &gui_uvw);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui_xyz), &QShortcut::activated, [&](){panel_xyz.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_2), &gui_uvw), &QShortcut::activated, [&](){panel_uvw.show();});

    return a.exec();
}
