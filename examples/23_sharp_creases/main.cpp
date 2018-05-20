/* This sample program enables to compute, edit, and export the
 * network of sharp creases af a triangular meshes.
 *
 * Creases are detected by simply thresholding the dihedral angle
 * at each edge. Additionally, the user can:
 *
 *  - pad the creases. That is, refine the mesh to make sure that each
 *    triangle has at most one edge on a crease
 *
 *  - manually adjust the crease network, selecting the edges that
 *    should change status directly on the canvas (CMD+click)
 *
 *  - export both the (possibly refined) mesh, and a text file encoding
 *    the crease network. The text file contains one line per crease edge.
 *    The line contains the vertex IDs of the two edge endpoints
 *
 * Enjoy!
*/

#include <QApplication>
#include <QWindow>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QFileDialog>
#include <fstream>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/profiler.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

uint closest_edge(const vec3d & p, const Trimesh<> & m)
{
    std::vector<std::pair<double,uint>> closest;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        closest.push_back(std::make_pair(m.edge_sample_at(eid,0.5).dist(p),eid));
    }
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/cube_minus_sphere.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_mesh_flat();
    m.show_marked_edge_width(4);

    Profiler          profiler;
    QWidget           window;
    QPushButton       but_mark_creases("Mark Creases", &window);
    QPushButton       but_pad_creases("Pad Creases", &window);
    QPushButton       but_export("Export", &window);
    QSpinBox          sb_crease_angle(&window);
    GLcanvas          gui(&window);
    QGridLayout       layout;

    sb_crease_angle.setMaximum(180);
    sb_crease_angle.setMinimum(0);
    sb_crease_angle.setValue(60);
    layout.addWidget(new QLabel("               Crease angle >=",&window),0,0);
    layout.addWidget(&sb_crease_angle,0,1);
    layout.addWidget(&but_mark_creases,0,2);
    layout.addWidget(&but_pad_creases,0,3);
    layout.addWidget(&but_export,0,4);
    layout.addWidget(&gui,1,0,1,5);
    window.setLayout(&layout);
    window.show();
    window.resize(800,600);

    gui.push_obj(&m);
    gui.push_marker(vec2i(10, gui.height()-20), "CMD + click to flag/unflag an edge", Color::BLACK(), 12, 0);

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

    QPushButton::connect(&but_pad_creases, &QPushButton::clicked, [&]()
    {
        // split triangles to make sure that each triangles has at most one crease edge
        std::vector<uint> to_split;
        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            uint count=0;
            for(uint eid : m.adj_p2e(pid)) if(m.edge_data(eid).marked) ++count;
            if(count>1) to_split.push_back(pid);
        }
        SORT_VEC(to_split, true);
        for(uint pid : to_split) m.poly_split(pid, m.poly_centroid(pid));
        std::cout << "Padding sharp creases ("<< to_split.size() << " triangles were split)" << std::endl;
        m.updateGL();
        gui.updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(&but_export, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getSaveFileName(NULL, "Export mesh + features", ".", "3D Meshes (*.off *.obj *.iv);; OBJ(*.obj);; OFF(*.off);; IV(*.iv)").toStdString();
        if(!filename.empty())
        {
            m.save(filename.c_str());
            std::ofstream f;
            f.open(filename + "sharp_creases.txt");
            assert(f.is_open());
            for(uint eid=0; eid<m.num_edges(); ++eid)
            {
                if(m.edge_data(eid).marked) f << m.edge_vert_id(eid,0) << " " << m.edge_vert_id(eid,1) << "\n";
            }
            f.close();
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    gui.callback_mouse_press = [&](GLcanvas *c, QMouseEvent *e)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            vec3d p;
            vec2i click(e->x(), e->y());
            if (c->unproject(click, p)) // transform click in a 3d point
            {
                profiler.push("Pick Edge");
                uint eid = closest_edge(p,m);
                profiler.pop();
                m.edge_data(eid).marked = !m.edge_data(eid).marked;
                m.updateGL();
                c->updateGL();
            }
        }
    };

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
