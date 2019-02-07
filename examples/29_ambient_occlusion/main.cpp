/* This sample program computes the ambient occlusion of an input mesh,
 * and allows to smoothly blend it with the standard mesh shading to obtain
 * the desired shadows
 *
 * Enjoy!
*/
#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/ambient_occlusion.h>
#include <cinolib/profiler.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "bamboo_pen.obj";
    DrawableQuadmesh<> m(s.c_str());

    QWidget     window;
    QSlider     slider(Qt::Horizontal);
    QVBoxLayout layout;
    GLcanvas    canvas(&window);
    canvas.push_obj(&m);
    slider.setValue(slider.maximum());
    layout.addWidget(&slider);
    layout.addWidget(&canvas);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    Profiler profiler;
    profiler.push("Compute AO");
    AO_srf<DrawableQuadmesh<>> AO(m);
    profiler.pop();
    AO.copy_to_mesh(m);
    m.updateGL();

    QSlider::connect(&slider, &QSlider::valueChanged, [&]()
    {
        float alpha = static_cast<float>(slider.value()-slider.minimum())/static_cast<float>(slider.maximum()-slider.minimum());
        m.show_AO_alpha(alpha);
        canvas.updateGL();
    });

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableQuadmesh<>> panel(&m, &canvas);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &canvas), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
