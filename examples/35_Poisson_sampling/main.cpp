/* This sample program computes a Poisson sampling
 * in nD as described in the seminal paper:
 *
 *   Fast Poisson Disk Sampling in Arbitrary Dimensions
 *   Robert Bridson
 *   SIGGRAPH Technical Sketch, 2007
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <cinolib/meshes/meshes.h>
#include <cinolib/Poisson_sampling.h>
#include <cinolib/profiler.h>
#include <cinolib/gl/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    Profiler profiler;
    QWidget window;
    GLcanvas gui_2d;
    GLcanvas gui_3d;
    gui_2d.setMinimumSize(400,400);
    gui_3d.setMinimumSize(400,400);
    QDoubleSpinBox sb_radius(&window);
    QPushButton but_compute_samples("Update samples", &window);
    sb_radius.setMinimum(1e-4);
    sb_radius.setMaximum(1);
    sb_radius.setValue(0.02);
    sb_radius.setDecimals(3);
    QLabel label("Radius:", &window);
    QGridLayout layout;
    layout.addWidget(&label,0,0);
    layout.addWidget(&sb_radius,0,1);
    layout.addWidget(&but_compute_samples,0,2);
    layout.addWidget(&gui_2d,1,0,1,3);
    layout.addWidget(&gui_3d,1,3,1,3);
    window.setLayout(&layout);
    window.show();

    DrawableTrimesh<> m_2d;
    DrawableTrimesh<> m_3d;
    m_2d.show_mesh_points();
    m_3d.show_mesh_points();
    m_2d.show_wireframe_width(6);
    m_3d.show_wireframe_width(3);

    QPushButton::connect(&but_compute_samples, &QPushButton::clicked, [&]()
    {
        m_2d.clear();
        m_3d.clear();

        std::vector<vec2d> samples_2d;
        profiler.push("Poisson sampling 2D");
        Poisson_sampling<2,vec2d>(sb_radius.value(), vec2d(0,0), vec2d(1,1), samples_2d);
        profiler.pop();
        for(auto p : samples_2d) m_2d.vert_add(vec3d(p.x(),p.y(),0));
        m_2d.vert_set_color(Color::BLACK());
        gui_2d.push_obj(&m_2d);

        std::vector<vec3d> samples_3d;
        profiler.push("Poisson sampling 3D");
        Poisson_sampling<3,vec3d>(sb_radius.value(), vec3d(0,0,0), vec3d(1,1,1), samples_3d);
        profiler.pop();
        for(auto p : samples_3d) m_3d.vert_add(p);
        m_3d.vert_set_color(Color::BLACK());
        gui_3d.push_obj(&m_3d);
    });

    emit(but_compute_samples.click());

    return a.exec();
}
