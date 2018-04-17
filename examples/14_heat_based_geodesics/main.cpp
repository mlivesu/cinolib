/* This sample program computes one-to-many and many-to-many
 * discrete geodesic distances on a given triangle mesh.
 * Matrix prefactorization is used to speed-up computation.
 * For details you may refer to the following article:
 *
 *   Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow
 *   KEENAN CRANE, CLARISSE WEISCHEDEL and MAX WARDETZKY
 *   ACM Transactions on Graphics, 2013
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>
#include <cinolib/profiler.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <algorithm>

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
    QPushButton but_reset("Reset");
    QGridLayout layout;
    gui.push_marker(vec2i(10, gui.height()-20), "CMD + click to add a heat source", Color::BLACK(), 12, 0);
    layout.addWidget(&but_reset,0,9);
    layout.addWidget(&gui,1,0,1,10);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    gui.push_obj(&m);

    Profiler profiler;
    std::vector<uint> sources;
    GeodesicsCache prefactored_matrices;

    gui.callback_mouse_press = [&](GLcanvas *c, QMouseEvent *e)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            vec3d p;
            vec2i click(e->x(), e->y());
            if (c->unproject(click, p)) // transform click in a 3d point
            {
                // add selected vertex to source list and re-compute heat geodesics
                uint vid = closest_vertex(p,m);
                sources.push_back(vid);
                profiler.push("compute_geodesics");
                compute_geodesics_amortized(m, prefactored_matrices, sources).copy_to_mesh(m);
                profiler.pop();
                m.show_texture1D(TEXTURE_1D_HSV_RAMP_W_ISOLINES);
                c->updateGL();
            }
        }
        return;
    };

    QPushButton::connect(&but_reset, &QPushButton::clicked, [&]()
    {
        // reset heat sources
        sources.clear();
        m.show_vert_color();
        gui.updateGL();
    });

    return a.exec();
}
