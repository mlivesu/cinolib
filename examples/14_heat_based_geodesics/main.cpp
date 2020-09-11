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
using namespace cinolib;
using namespace std;

uint closest_vertex(const vec3d & p, const Trimesh<> & m)
{
    vector<pair<float,uint>> closest;
    for(uint vid=0; vid<m.num_verts(); ++vid) closest.push_back(make_pair(m.vert(vid).dist(p),vid));
    sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
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

    string s = (argc==2) ? string(argv[1]) : string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(false);
    gui.push_obj(&m);

    Profiler profiler;
    vector<uint> sources;
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
                m.show_texture1D(TEXTURE_1D_HSV_W_ISOLINES);
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

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
