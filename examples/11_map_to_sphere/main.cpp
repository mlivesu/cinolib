/* This program takes as input a genus zero shape, and maps
 * it to a sphere through a conformalized mean curvature flow
 * (cMCF). For technical details one may refer to this paper:
 *
 *     Can Mean-Curvature Flow be Modified to be Non-singular?
 *     Michael Kazhdan, Jake Solomon and Mirela Ben-Chen
 *     Computer Graphics Forum, 31(5), 2012.
*/

#include <QApplication>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <cinolib/meshes/meshes.h>
#include <cinolib/mean_curv_flow.h>
#include <cinolib/gl/qt/qt_gui_tools.h>
#include <cinolib/profiler.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication app(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m_xyz(s.c_str());
    DrawableTrimesh<> m_uvw = m_xyz;

    QWidget  window;
    QGridLayout layout;
    window.setLayout(&layout);
    GLcanvas gui_xyz(&window);
    GLcanvas gui_uvw(&window);
    gui_xyz.push_obj(&m_xyz);
    gui_uvw.push_obj(&m_uvw);
    QLabel lt("Time step:");
    QDoubleSpinBox t;
    t.setMinimum(0.0000001);
    t.setMaximum(100);
    t.setDecimals(7);
    t.setValue(0.005);
    QLabel l_iter("#iters:");
    QSpinBox iters;
    iters.setMinimum(1);
    iters.setMaximum(100);
    iters.setValue(10);
    QCheckBox conformalized("Conformalized", &window);
    conformalized.setChecked(true);
    QPushButton but("MCF", &window);
    QPushButton reset("Reset", &window);
    QVBoxLayout mid;
    mid.addWidget(&lt);
    mid.addWidget(&t);
    mid.addWidget(&l_iter);
    mid.addWidget(&iters);
    mid.addWidget(&conformalized);
    mid.addWidget(&but);
    mid.addWidget(&reset);
    mid.addStretch();
    layout.addWidget(&gui_xyz,0,0);
    layout.addLayout(&mid,0,1);
    layout.addWidget(&gui_uvw,0,2);
    window.show();
    gui_xyz.setMinimumSize(400,400);
    gui_uvw.setMinimumSize(400,400);

    m_xyz.show_wireframe(true);
    m_uvw.show_wireframe(true);

    Profiler profiler;

    QPushButton::connect(&but, &QPushButton::clicked, [&]()
    {
        profiler.push("Compute MCF");
        MCF(m_uvw, iters.value(), t.value(), conformalized.isChecked());
        profiler.pop();
        m_uvw.updateGL();
        gui_uvw.fit_scene();
    });

    QPushButton::connect(&reset, &QPushButton::clicked, [&]()
    {
        m_uvw = m_xyz;
        m_uvw.updateGL();
        gui_uvw.fit_scene();
    });

    // CMD+1 to show XYZ mesh controls.
    // CMD+2 to show UVW mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_xyz(&m_xyz, &gui_xyz);
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel_uvw(&m_uvw, &gui_uvw);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui_xyz), &QShortcut::activated, [&](){panel_xyz.show();});
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_2), &gui_uvw), &QShortcut::activated, [&](){panel_uvw.show();});

    return app.exec();
}
