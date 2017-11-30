/* This sample program computes one-to-many and many-to-many
 * discrete geodesic distances on a given triangle mesh. For
 * details you may refer to the following article:
 *
 *   Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow
 *   KEENAN CRANE, CLARISSE WEISCHEDEL and MAX WARDETZKY
 *   ACM Transactions on Graphics, 2013
 *
 * Enjoy!
*/

#include <QApplication>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <cinolib/meshes/meshes.h>
#include <cinolib/geodesics.h>
#include <cinolib/profiler.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/gui/qt/surface_mesh_control_panel.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    QWidget window;
    GLcanvas gui(&window);
    QLabel label("Heat sources:", &window);
    QLineEdit le_heat_sources(&window);
    QPushButton but_compute_geod("Compute Geodesics");
    QGridLayout layout;
    layout.addWidget(&label,0,0);
    layout.addWidget(&le_heat_sources,0,1);
    layout.addWidget(&but_compute_geod,0,2,1,2);
    layout.addWidget(&gui,1,0,1,4);
    window.setLayout(&layout);
    window.show();

    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/bunny.obj";
    DrawableTrimesh<> m(s.c_str());
    m.show_texture1D(TEXTURE_1D_HSV_RAMP_W_ISOLINES);
    gui.push_obj(&m);

    SurfaceMeshControlPanel<DrawableTrimesh<>> m_controls(&m, &gui);
    m_controls.show();

    Profiler profiler;

    QPushButton::connect(&but_compute_geod, &QPushButton::clicked, [&]()
    {
        std::string       str_sources = le_heat_sources.text().toStdString();
        std::stringstream ss(str_sources);
        std::vector<uint> sources;
        uint              vid;
        while(ss >> vid) sources.push_back(vid);
        if (!sources.empty())
        {
            profiler.push("compute_geodesics");
            compute_geodesics(m, sources).copy_to_mesh(m);
            profiler.pop();
            m.updateGL();
            gui.updateGL();
        }
    });

    le_heat_sources.setText("0"); // start by placing heat on vertex #0...
    but_compute_geod.click();

    a.setActiveWindow(&gui);
    return a.exec();
}
