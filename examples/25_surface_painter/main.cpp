/* This program uses a linear brush to paint a surface mesh in red.
 * The user can choose the brush size, as well as reset the surface
 * to a white canvas.
 *
 * Enjoy!
*/

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <cinolib/gl/qt/qt_gui_tools.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

uint closest_vertex(const vec3d & p, const Trimesh<> & m)
{
    std::vector<std::pair<double,uint>> closest;
    for(uint vid=0; vid<m.num_verts(); ++vid) closest.push_back(std::make_pair(m.vert(vid).dist(p),vid));
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QWidget     window;
    QSlider     sl_size(Qt::Horizontal, &window);
    QVBoxLayout layout;
    QPushButton but_reset("Reset", &window);
    GLcanvas    canvas(&window);
    sl_size.setMaximum(100);
    sl_size.setMinimum(0);
    sl_size.setValue(10);
    layout.addWidget(&sl_size);
    layout.addWidget(&but_reset);
    layout.addWidget(&canvas);
    window.setLayout(&layout);
    window.show();
    window.resize(600,600);

    std::string s = (argc>1) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    canvas.push_obj(&m);
    m.show_wireframe(false);
    m.show_vert_color();

    GeodesicsCache prefactored_matrices;
    compute_geodesics_amortized(m, prefactored_matrices, {0}); // init heat based solver

    canvas.callback_mouse_press = [&](GLcanvas *c, QMouseEvent *e)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            vec3d p;
            if(c->unproject(vec2i(e->x(),e->y()), p))
            {
                float brush_size = static_cast<float>(sl_size.value())/100.f;
                uint  vid        = closest_vertex(p,m);
                ScalarField f    = compute_geodesics_amortized(m, prefactored_matrices, {vid});
                for(vid=0; vid<m.num_verts(); ++vid)
                {
                    float dist = 1.f - f[vid];
                    if(dist<=brush_size)
                    {
                        float val = m.vert_data(vid).color.g;
                        val -= (brush_size-dist)/brush_size;
                        if(val<0) val = 0.f;
                        m.vert_data(vid).color = Color(1,val,val);
                    }
                }
                m.updateGL();
                c->updateGL();
            }
        }
        return;
    };

    QPushButton::connect(&but_reset, &QPushButton::clicked, [&]()
    {
        m.vert_set_color(Color::WHITE());
        canvas.updateGL();
    });

    // CMD+1 to show mesh controls.
    SurfaceMeshControlPanel<DrawableTrimesh<>> panel(&m, &canvas);
    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &canvas), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}
