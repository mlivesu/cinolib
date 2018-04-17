/* This sample program computes n-harmonic functions on a surface mesh.
 *
 * The harmonicity index is initially set to 1, and can be controlled
 * via the spin box placed on top of the GL canvas.
 *
 * Maxima and minima can be arbitrarily placed on the surface by vertex
 * picking. Use CMD + click to add a maximum, and SHIFT + click to add
 * a minimum.
 *
 * Enjoy!
*/
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <algorithm>
#include <cinolib/meshes/meshes.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/profiler.h>
#include <cinolib/gui/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

uint closest_vertex(const cinolib::vec3d & p, const cinolib::Trimesh<> & m)
{
    std::vector<std::pair<double,uint>> closest;
    for(uint vid=0; vid<m.num_verts(); ++vid) closest.push_back(std::make_pair(m.vert(vid).dist(p),vid));
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui(&window);
    QPushButton but_harmonic("Harmonic Field",&window);
    QPushButton but_reset("Reset",&window);
    QSpinBox n_harmonicity(&window);
    n_harmonicity.setMaximum(5);
    n_harmonicity.setMinimum(1);
    QGridLayout layout;
    layout.addWidget(new QLabel("Harmonicity (harmonic, bi-harmonic, ...):"),0,0);
    layout.addWidget(&n_harmonicity,0,1);
    layout.addWidget(&but_harmonic,0,2);
    layout.addWidget(&but_reset,0,9);
    layout.addWidget(&gui,2,0,1,10);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);
    gui.push_marker(vec2i(10, gui.height()-20), "CMD + click to add a maximum", Color::BLACK(), 12, 0);
    gui.push_marker(vec2i(10, gui.height()-40), "SHIFT + click to add a minimum", Color::BLACK(), 12, 0);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    gui.push_obj(&m);

    Profiler profiler;
    std::map<uint,double> dirichlet_bcs;
    bool has_at_least_one_min = false;
    bool has_at_least_one_max = false;

    gui.callback_mouse_press = [&](GLcanvas *c, QMouseEvent *e)
    {
        if (e->modifiers() == Qt::ControlModifier)  // add maximum
        {
            vec3d p;
            vec2i click(e->x(), e->y());
            if (c->unproject(click, p))
            {
                uint vid = closest_vertex(p,m);
                dirichlet_bcs[vid] = 1.0;
                has_at_least_one_max = true;
                c->push_marker(m.vert(vid), "", Color::RED());
                c->updateGL();
            }
        }
        else if (e->modifiers() == Qt::ShiftModifier) // add minimum
        {
            vec3d p;
            vec2i click(e->x(), e->y());
            if (c->unproject(click, p))
            {
                uint vid = closest_vertex(p,m);
                dirichlet_bcs[vid] = 0.0;
                has_at_least_one_min = true;
                c->push_marker(m.vert(vid), "", Color::BLUE());
                c->updateGL();
            }
        }
        return;
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_harmonic, &QPushButton::clicked, [&]()
    {
        if (!dirichlet_bcs.empty() && has_at_least_one_min && has_at_least_one_max)
        {
            profiler.push("harmonic_map");
            harmonic_map(m, dirichlet_bcs, n_harmonicity.value(), COTANGENT).copy_to_mesh(m);
            profiler.pop();
            m.show_texture1D(TEXTURE_1D_ISOLINES);
            gui.updateGL();
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_reset, &QPushButton::clicked, [&]()
    {
        has_at_least_one_min = false;
        has_at_least_one_max = false;
        dirichlet_bcs.clear();
        m.show_poly_color();
        gui.pop_all_markers();
        gui.push_marker(vec2i(10, gui.height()-20), "CMD + click to add a maximum", Color::BLACK(), 12, 0);
        gui.push_marker(vec2i(10, gui.height()-40), "SHIFT + click to add a minimum", Color::BLACK(), 12, 0);
        gui.updateGL();
    });

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
