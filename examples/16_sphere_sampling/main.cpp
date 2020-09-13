/* This sample program computes an evenly distributed
 * sphere sampling based on spherical Fibonacci.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <cinolib/meshes/meshes.h>
#include <cinolib/sphere_coverage.h>
#include <cinolib/profiler.h>
#include <cinolib/gui/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
using namespace cinolib;
using namespace std;
#define SPHERE_COVERAGE 1000

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui(&window);
    QSpinBox sb_num_samples(&window);
    QPushButton but_compute_samples("Update samples", &window);
    
    sb_num_samples.setMinimum(10);
    sb_num_samples.setMaximum(SPHERE_COVERAGE*SPHERE_COVERAGE);
    sb_num_samples.setValue(100);
    
    QLabel label("Num samples:", &window);
    QGridLayout layout;
    layout.addWidget(&label,0,0);
    layout.addWidget(&sb_num_samples,0,1);
    layout.addWidget(&but_compute_samples,0,2);
    layout.addWidget(&gui,1,0,1,4);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    vector<vec3d> samples;
    sphere_coverage(SPHERE_COVERAGE, samples);

    vector<uint> no_polys = {};
    DrawableTrimesh<> m(samples, no_polys);
    m.vert_set_color(Color::BLACK());
    m.show_wireframe_width(3);
    m.show_mesh_points();
    gui.push_obj(&m);

    Profiler profiler;

    QPushButton::connect(&but_compute_samples, &QPushButton::clicked, [&]()
    {
        profiler.push("Sphere coverage");
        sphere_coverage(sb_num_samples.value(), samples);
        profiler.pop();
        m = DrawableTrimesh<>(samples, no_polys);
        m.vert_set_color(Color::BLACK());
        m.show_wireframe_width(3);
        m.show_mesh_points();
        gui.updateGL();
    });

    return a.exec();
}
