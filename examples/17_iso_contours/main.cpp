/* This sample program computes on-the-fly iso-contours
 * of a scalar field embedded in the vertices of a surface
 * mesh.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QSlider>
#include <QPushButton>
#include <QGridLayout>
#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>
#include <cinolib/profiler.h>
#include <cinolib/drawable_isocontour.h>
#include <cinolib/gui/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget     window;
    QSlider     sl_iso(Qt::Horizontal);
    QPushButton but_tessellate("Tessellate");
    QGridLayout layout;
    GLcanvas    canvas(&window);
    sl_iso.setMaximum(100);
    sl_iso.setMinimum(0);
    sl_iso.setValue(50);
    layout.addWidget(&sl_iso,0,0,1,9);
    layout.addWidget(&but_tessellate,0,9,1,1);
    layout.addWidget(&canvas,2,0,1,10);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/circle.obj";
    DrawableTrimesh<> m(s.c_str());
    compute_geodesics(m, {100}, COTANGENT).copy_to_mesh(m); // generate some scalar field
    m.show_texture1D(TEXTURE_1D_HSV_RAMP_W_ISOLINES);
    m.show_wireframe(true);
    m.show_wireframe_transparency(0.5);
    canvas.push_obj(&m);

    DrawableIsocontour<> iso(m, 0.5);
    iso.thickness = 2.0;
    canvas.push_obj(&iso, false);

    Profiler profiler;

    QSlider::connect(&sl_iso, &QSlider::valueChanged, [&]()
    {
        profiler.push("update iso-contour");
        iso = DrawableIsocontour<>(m, static_cast<float>(sl_iso.value())/100.0);
        profiler.pop();
        canvas.updateGL();
    });

    QSlider::connect(&but_tessellate, &QPushButton::clicked, [&]()
    {
        profiler.push("tessellate iso-contour");
        iso.tessellate(m);
        profiler.pop();
        m.updateGL();
        canvas.updateGL();
    });

    return a.exec();
}
