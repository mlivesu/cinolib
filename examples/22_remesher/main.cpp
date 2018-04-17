/* This sample program reads a triangle mesh and performs
 * an area equalizing remeshing scheme. This is a variant 
 * of the algorithm described in
 *
 *    A Remeshing Approach to Multiresolution Modeling
 *    M.Botsch, L.Kobbelt
 *    Symposium on Geomtry Processing, 2004
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/profiler.h>
#include <cinolib/remesh_BotschKobbelt2004.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication   a(argc, argv);
    QWidget        window;
    QPushButton    but_mark_color("Mark Colors", &window);
    QPushButton    but_mark_creases("Mark Creases", &window);
    QPushButton    but_mark_boundary("Mark Boundary", &window);
    QPushButton    but_unmark_all("Unmark all", &window);
    QPushButton    but_remesh("Remesh", &window);
    QSpinBox       sb_crease_angle(&window);
    QSpinBox       sb_niters(&window);
    QDoubleSpinBox sb_target_length(&window);
    GLcanvas       gui(&window);
    QGridLayout    layout;

    sb_crease_angle.setMaximum(180);
    sb_crease_angle.setMinimum(0);
    sb_crease_angle.setValue(60);
    sb_niters.setValue(1);
    sb_target_length.setDecimals(4);
    sb_target_length.setMinimum(0.0001);
    sb_target_length.setMaximum(9999);
    layout.addWidget(&but_mark_color,0,0,1,2);
    layout.addWidget(&but_mark_creases,0,2,1,2);
    layout.addWidget(new QLabel("Crease angle >=",&window),1,2);
    layout.addWidget(&sb_crease_angle,1,3);
    layout.addWidget(&but_mark_boundary,0,4,1,2);
    layout.addWidget(&but_unmark_all,0,6,1,2);
    layout.addWidget(&but_remesh,0,8,1,2);
    layout.addWidget(new QLabel("#iters: ",&window),1,8);
    layout.addWidget(&sb_niters,1,9);
    layout.addWidget(new QLabel("target length: ",&window),2,8);
    layout.addWidget(&sb_target_length,2,9);
    layout.addWidget(&gui,3,0,1,10);
    window.setLayout(&layout);
    window.show();
    window.resize(800,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/pyramid.off";
    cinolib::DrawableTrimesh<> m(s.c_str());
    m.show_marked_edge(true);
    m.show_wireframe(true);
    gui.push_obj(&m);
    sb_target_length.setValue(m.edge_avg_length());

    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m,&gui);
    panel.show();

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_mark_color, &QPushButton::clicked, [&]()
    {
        m.edge_mark_color_discontinuities();
        m.updateGL();
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_mark_boundary, &QPushButton::clicked, [&]()
    {
        m.edge_mark_boundaries();
        m.updateGL();
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_mark_creases, &QPushButton::clicked, [&]()
    {
        double thresh_rad = static_cast<double>(sb_crease_angle.value()) * M_PI/180.0;
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if(m.edge_crease_angle(eid) > thresh_rad)
            {
                m.edge_data(eid).marked = true;
            }
        }
        m.updateGL();
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_unmark_all, &QPushButton::clicked, [&]()
    {
        m.edge_unmark_all();
        m.updateGL();
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_remesh, &QPushButton::clicked, [&]()
    {
        Profiler profiler;
        for(int i=0; i<sb_niters.value(); ++i)
        {
            profiler.push("Remesh iteration");
            remesh_Botsch_Kobbelt_2004(m, sb_target_length.value(),true);
            profiler.pop();
            m.updateGL();
            gui.updateGL();
        }
    });

    return a.exec();
}
