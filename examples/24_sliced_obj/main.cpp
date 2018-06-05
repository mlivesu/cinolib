/* This program reads a sliced object in CLI format,
 * and create a triangle mesh containing all its slices
 * and support structures.
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/drawable_sliced_object.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/T_supported.cli";
    double hatch  = (argc>2) ? atof(argv[2]) : 0.01;

    std::cout << "load " << s << std::endl;
    std::cout << "hatch is: " << hatch << std::endl;

    DrawableSlicedObj<> obj(s.c_str(), hatch);

    GLcanvas gui;
    gui.push_obj(&obj);
    gui.show();

    // CMD+1 to show visual controls.
    SurfaceMeshControlPanel<DrawableSlicedObj<>> panel(&obj, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
