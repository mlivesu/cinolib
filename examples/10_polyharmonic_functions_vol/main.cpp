/* This sample program computes n-harmonic functions on a volume mesh.
 * The harmonicity index is initially set to 1, and can be controlled
 * via the spin box placed on top of the GL canvas.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui(&window);
    QLabel label("Harmonicity index (1 => harmonic, 2 => bi-harmonic, ...):", &window);
    QSpinBox n_harmonicity(&window);
    n_harmonicity.setMaximum(5);
    n_harmonicity.setMinimum(1);
    QGridLayout layout;
    layout.addWidget(&label,0,0);
    layout.addWidget(&n_harmonicity,0,1);
    layout.addWidget(&gui,1,0,1,2);
    window.setLayout(&layout);
    window.show();
    window.resize(800,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/sphere.MESH";
    DrawableTetmesh<> m(s.c_str());
    m.show_out_texture1D(TEXTURE_1D_HSV_RAMP_W_ISOLINES);
    m.show_in_texture1D (TEXTURE_1D_HSV_RAMP_W_ISOLINES);
    gui.push_obj(&m);

    VolumeMeshControlPanel<DrawableTetmesh<>> m_controls(&m, &gui);
    m_controls.show();

    // use the spin box to increase the harmonicity index
    Profiler profiler;
    QSpinBox::connect(&n_harmonicity, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&]()
    {
        std::map<uint,double> bc = {{0,0.0}, {999,1.0}}; // Dirichlet boundary conditions
        profiler.push("harmonic_map");
        harmonic_map(m, bc, n_harmonicity.value(), COTANGENT).copy_to_mesh(m);
        profiler.pop();
        m.updateGL();
        gui.updateGL();
    });

    n_harmonicity.valueChanged(1); // start with a 1-harmonic map

    return a.exec();
}
