/* This sample program computes on-the-fly iso-surfaces
 * of a scalar field embedded in the vertices of a volume
 * mesh by using a variant of marching cubes algorithm for
 * tetrahedal meshes.
 *
 * The marching method is implemented so as to generate
 * "topologically clean" iso-surfaces, meaning that the
 * resulting triangular meshes do not contain duplicated
 * vertices or degenerate edges/triangles.
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
#include <cinolib/drawable_isosurface.h>
#include <cinolib/gui/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget     window;
    QSlider     sl_iso(Qt::Horizontal);
    QSlider     sl_slice(Qt::Horizontal);
    QPushButton but_tessellate("Tessellate");
    QGridLayout layout;
    GLcanvas    gui(&window);
    sl_iso.setMaximum(100);
    sl_iso.setMinimum(0);
    sl_iso.setValue(50);
    sl_slice.setMaximum(100);
    sl_slice.setMinimum(0);
    sl_slice.setValue(50);
    layout.addWidget(&sl_iso,0,0,1,9);
    layout.addWidget(&sl_slice,1,0,1,9);
    layout.addWidget(&but_tessellate,0,9,2,1);
    layout.addWidget(&gui,2,0,1,10);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/sphere.mesh";
    DrawableTetmesh<> m(s.c_str());
    compute_geodesics(m, {0, 10}, COTANGENT).copy_to_mesh(m); // generate some scalar field
    m.show_in_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
    m.show_out_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
    gui.push_obj(&m);

    SlicerState ss;
    ss.Z_thresh = 0.5;
    m.slice(ss);

    DrawableIsosurface<> iso(m, 0.5);
    gui.push_obj(&iso, false);

    Profiler profiler;

    QSlider::connect(&sl_iso, &QSlider::valueChanged, [&]()
    {
        profiler.push("update iso-surface");
        iso = DrawableIsosurface<>(m, static_cast<float>(sl_iso.value())/100.0);
        profiler.pop();
        gui.updateGL();
    });

    QSlider::connect(&sl_slice, &QSlider::valueChanged, [&]()
    {
        ss.Z_thresh = static_cast<float>(sl_slice.value())/100.0;
        profiler.push("update slicing");
        m.slice(ss);
        profiler.pop();
        gui.updateGL();
    });

    QSlider::connect(&but_tessellate, &QPushButton::clicked, [&]()
    {
        profiler.push("tessellate iso-surface");
        iso.tessellate(m);
        profiler.pop();
        m.slice(ss);
        gui.updateGL();
    });

    // CMD+1 to show mesh controls.
    VolumeMeshControlPanel<DrawableTetmesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
