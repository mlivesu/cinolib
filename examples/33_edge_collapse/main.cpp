/* This is a base application for cinolib (https://github.com/maxicino/cinolib).
 *
 * It will iteratively collapse all internal edges of a given mesh until possible,
 * candidate collapses are checked for both topological (i.e. manifold) and geometrical
 * (i.e. no flips) consistency.
 *
 * Enjoy!
*/

#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/qt/qt_gui_tools.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/circle.obj";

    DrawableTrimesh<> m(s.c_str());

    GLcanvas gui;
    gui.push_obj(&m);
    gui.show();

    std::cout << "\n\nPress SPACE to start the collapsing sequence\n\n" << std::endl;

    gui.callback_key_press = [&](GLcanvas *c, QKeyEvent *e)
    {
        if(e->key() == Qt::Key_Space)
        {
            bool converged = false;
            do
            {
                converged = true;
                for(uint eid=0; eid<m.num_edges(); ++eid)
                {
                    if(m.edge_is_incident_to_boundary(eid)) continue;
                    if(m.edge_collapse(eid, 0.5, true, true))
                    {
                        converged = false;
                        m.updateGL();
                        c->updateGL_strict();
                    }
                }
            }
            while(!converged);
            m.updateGL();
            c->updateGL();
        }
    };

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &gui);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
