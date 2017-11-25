/* This sample program computes n-harmonic functions on a surface mesh.
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
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/surface_mesh_control_panel.h>
#include <cinolib/harmonic_map.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui(&window);
    QLabel label("Harmonicity index (1 => harmonic, 2 => bi-harmonic, ...):", &window);
    QSpinBox n_harmonicity(&window);
    n_harmonicity.setMaximum(10);
    n_harmonicity.setMinimum(1);
    QGridLayout layout;
    layout.addWidget(&label,0,0);
    layout.addWidget(&n_harmonicity,0,1);
    layout.addWidget(&gui,1,0,1,2);
    window.setLayout(&layout);
    window.show();

    std::string s = std::string(DATA_PATH) + "/circle.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_texture1D(TEXTURE_1D_HSV_RAMP_W_ISOLINES);
    gui.push_obj(&m);

    SurfaceMeshControlPanel<DrawableTrimesh<>> m_controls(&m, &gui);
    m_controls.show();

    // use the spin box to increase the harmonicity index
    QSpinBox::connect(&n_harmonicity, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&]()
    {

        std::map<uint,double> bc = {{7,0.0}, {99,1.0}}; // Dirichlet boundary conditions
        harmonic_map(m, bc, n_harmonicity.value(), COTANGENT, SIMPLICIAL_LDLT).copy_to_mesh(m);
        m.updateGL();
        gui.updateGL();
    });

    n_harmonicity.valueChanged(1); // start with a 1-harmonic map

    a.setActiveWindow(&gui);
    return a.exec();
}
