/* This sample program computes the coarse block
 * decomposition of a given hexhedral mesh.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QSlider>
#include <QPushButton>
#include <QGridLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/qt/qt_gui_tools.h>
#include <cinolib/coarse_layout.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/ellipsoid.mesh";
    DrawableHexmesh<> m(s.c_str());

    Profiler profiler;
    profiler.push("coarse layout");
    compute_coarse_hex_layout(m);
    profiler.pop();
    m.poly_color_wrt_label();
    m.updateGL();

    GLcanvas gui;
    gui.push_obj(&m);
    gui.show();

    // CMD+1 to show mesh controls.
    VolumeMeshControlPanel<DrawableHexmesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
