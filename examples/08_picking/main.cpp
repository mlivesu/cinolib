/* This sample program performs vertex picking on a triangular mesh.
 *
 * Picking is performed by transforming a mouse click on the canvas
 * into a point in space. The mesh point that minimizes the distance
 * from it is then selected and colored in RED for visual feedback.
 *
 * Notice that despite this implementation of picking is sub-optimal
 * (its complexity is O(n), with n being the number of mesh vertices)
 * it is fast enough to work in real time with meshes of moderate size.
 *
 * For huge meshes, better performances can be achieved by using a spatial
 * data structure, such as Octrees or AABB trees.
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/qt/qt_gui_tools.h>
#include <cinolib/profiler.h>
#include <algorithm>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/maxFace.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_wireframe(true);
    m.show_vert_color();

    GLcanvas gui;
    gui.show();
    gui.push_obj(&m);
    gui.push_marker(vec2i(10, gui.height()-20), "CMD + click to select a vertex", Color::BLACK(), 12, 0);

    Profiler profiler;

    gui.callback_mouse_press = [&](GLcanvas *c, QMouseEvent *e)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            vec3d p;
            vec2i click(e->x(), e->y());
            if (c->unproject(click, p)) // transform click in a 3d point
            {
                profiler.push("Pick Vertex");
                uint vid = m.pick_vert(p);
                profiler.pop();
                m.vert_data(vid).color = Color::RED();
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
