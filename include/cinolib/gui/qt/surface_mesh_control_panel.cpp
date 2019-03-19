/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/gui/qt/surface_mesh_control_panel.h>
#include <cinolib/textures/textures.h>
#include <cinolib/meshes/mesh_slicer.h>
#include <cinolib/gradient.h>
#include <cinolib/symbols.h>
#include <cinolib/ambient_occlusion.h>
#include <cinolib/deg_rad.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
SurfaceMeshControlPanel<Mesh>::SurfaceMeshControlPanel(Mesh *m, GLcanvas *canvas, QWidget *parent)
    : m(m)
    , canvas(canvas)
{
    assert(canvas != NULL);

    widget                 = new QWidget(parent);
    global_layout          = new QHBoxLayout(widget);
    QVBoxLayout *left_col  = new QVBoxLayout();
    QVBoxLayout *right_col = new QVBoxLayout();
    global_layout->addLayout(left_col);
    global_layout->addLayout(right_col);
    global_font.setPointSize(8);
    widget->setFont(global_font);
    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // LOAD/SAVE buttons and SHADING
    {
        but_load     = new QPushButton("Load", widget);
        but_save     = new QPushButton("Save", widget);
        cb_show_mesh = new QCheckBox("Show", widget);
        but_load->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_save->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_show_mesh->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_load->setFont(global_font);
        but_save->setFont(global_font);
        cb_show_mesh->setFont(global_font);
        cb_show_mesh->setChecked(true);
        QVBoxLayout *layout0 = new QVBoxLayout();
        layout0->addWidget(but_load);
        layout0->addWidget(but_save);
        layout0->addWidget(cb_show_mesh);
        //
        QGroupBox *gbox   = new QGroupBox("Shading",widget);
        rb_point_shading  = new QRadioButton("Point",  gbox);
        rb_flat_shading   = new QRadioButton("Flat",   gbox);
        rb_smooth_shading = new QRadioButton("Smooth", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_point_shading->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_flat_shading->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_smooth_shading->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        gbox->setFont(global_font);
        rb_point_shading->setFont(global_font);
        rb_flat_shading->setFont(global_font);
        rb_smooth_shading->setFont(global_font);
        rb_smooth_shading->setChecked(true);
        QVBoxLayout *layout1 = new QVBoxLayout();
        layout1->addWidget(rb_point_shading);
        layout1->addWidget(rb_flat_shading);
        layout1->addWidget(rb_smooth_shading);
        gbox->setLayout(layout1);
        //
        QHBoxLayout *layout = new QHBoxLayout();
        layout->addLayout(layout0);
        layout->addWidget(gbox);
        left_col->addLayout(layout);
    }

    // COLORS / TEXTURES
    {
        QGroupBox *gbox       = new QGroupBox("Colors/Textures",widget);
        rb_vert_color         = new QRadioButton("Vert Color", gbox);
        rb_poly_color         = new QRadioButton("Poly Color", gbox);
        rb_tex1D              = new QRadioButton("Text 1D", gbox);
        rb_tex2D              = new QRadioButton("Text 2D", gbox);
        but_set_vert_color    = new QPushButton("Set", gbox);
        but_set_poly_color    = new QPushButton("Set", gbox);
        cb_tex1D_type         = new QComboBox(gbox);
        cb_tex2D_type         = new QComboBox(gbox);
        but_serialize_field   = new QPushButton("Serialize", gbox);
        but_deserialize_field = new QPushButton("Deserialize", gbox);
        sl_tex2D_density      = new QSlider(Qt::Horizontal, gbox);
        but_load_tex2d        = new QPushButton("Load Tex2D", gbox);
        tex2d_filename        = "";
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_tex1D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_tex2D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_tex1D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_tex2D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_serialize_field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_deserialize_field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_tex2D_density->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_load_tex2d->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_poly_color->setChecked(true);
        cb_tex1D_type->insertItem(0,"ISO");
        cb_tex1D_type->insertItem(1,"RAMP");
        cb_tex1D_type->insertItem(2,"RAMP + ISO");
        cb_tex1D_type->insertItem(3,"PARULA");
        cb_tex1D_type->insertItem(4,"PARULA + ISO");
        cb_tex2D_type->insertItem(0,"ISO");
        cb_tex2D_type->insertItem(1,"CB");
        cb_tex2D_type->insertItem(2,"IMG");
        sl_tex2D_density->setMinimum(1);
        sl_tex2D_density->setMaximum(200);
        sl_tex2D_density->setValue(10);
        sl_tex2D_density->setSliderPosition(10);        
        gbox->setFont(global_font);
        rb_vert_color->setFont(global_font);
        rb_poly_color->setFont(global_font);
        rb_tex1D->setFont(global_font);
        rb_tex2D->setFont(global_font);
        but_set_vert_color->setFont(global_font);
        but_set_poly_color->setFont(global_font);
        but_load_tex2d->setFont(global_font);
        cb_tex1D_type->setFont(global_font);
        cb_tex2D_type->setFont(global_font);
        but_serialize_field->setFont(global_font);
        but_deserialize_field->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(rb_vert_color,0,0);
        layout->addWidget(rb_poly_color,1,0);
        layout->addWidget(rb_tex1D,2,0);
        layout->addWidget(rb_tex2D,3,0);
        layout->addWidget(but_set_vert_color,0,1);
        layout->addWidget(but_set_poly_color,1,1);
        layout->addWidget(cb_tex1D_type,2,1);
        layout->addWidget(cb_tex2D_type,3,1);
        layout->addWidget(but_load_tex2d, 4,0);
        layout->addWidget(sl_tex2D_density, 4,1);
        layout->addWidget(but_serialize_field,5,0);
        layout->addWidget(but_deserialize_field,5,1);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
    }

    // WIREFRAME
    {
        QGroupBox *gbox     = new QGroupBox(widget);
        cb_wireframe        = new QCheckBox("Wireframe", gbox);
        sl_wireframe_width  = new QSlider(Qt::Horizontal, gbox);
        sl_wireframe_alpha  = new QSlider(Qt::Horizontal, gbox);
        but_wireframe_color = new QPushButton("Color", gbox);
        QLabel *l_width     = new QLabel("Width: ", gbox);
        QLabel *l_alpha     = new QLabel("Transp:", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_wireframe->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_wireframe_width->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sl_wireframe_alpha->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        but_wireframe_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_wireframe_width->setMinimum(1);
        sl_wireframe_width->setMaximum(4);
        sl_wireframe_width->setTickPosition(QSlider::TicksBelow);
        sl_wireframe_width->setTickInterval(1);
        sl_wireframe_alpha->setMinimum(0);
        sl_wireframe_alpha->setMaximum(99);
        sl_wireframe_alpha->setValue(50);
        gbox->setFont(global_font);
        cb_wireframe->setFont(global_font);
        cb_wireframe->setChecked(true);
        sl_wireframe_width->setFont(global_font);
        sl_wireframe_alpha->setFont(global_font);
        but_wireframe_color->setFont(global_font);
        l_width->setFont(global_font);
        l_alpha->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_wireframe,0,0,1,2);
        layout->addWidget(l_width,1,0);
        layout->addWidget(l_alpha,2,0);
        layout->addWidget(sl_wireframe_width,1,1);
        layout->addWidget(sl_wireframe_alpha,2,1);
        layout->addWidget(but_wireframe_color,3,1);
        gbox->setLayout(layout);
        left_col->addWidget(gbox);
    }

    // ISO-CURVES
    {
        QGroupBox *gbox    = new QGroupBox(widget);
        cb_isocurve        = new QCheckBox("Isocurve", gbox);
        sl_isovalue        = new QSlider(Qt::Horizontal, gbox);
        sl_isocurve_width  = new QSlider(Qt::Horizontal, gbox);
        but_isocurve_color = new QPushButton("Color", gbox);
        QLabel *l_value    = new QLabel("Isoval: ", gbox);
        QLabel *l_width    = new QLabel("Width:  ", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_isocurve->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_isovalue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sl_isocurve_width->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        but_isocurve_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_value->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_isocurve_width->setMinimum(1);
        sl_isocurve_width->setMaximum(10);
        sl_isovalue->setMaximum(999);
        sl_isovalue->setValue(499);
        sl_isovalue->setSliderPosition(499);
        gbox->setFont(global_font);
        but_isocurve_color->setFont(global_font);
        l_width->setFont(global_font);
        l_value->setFont(global_font);
        cb_isocurve->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_isocurve,0,0,1,2);
        layout->addWidget(l_value,1,0);
        layout->addWidget(sl_isovalue,1,1);
        layout->addWidget(l_width,2,0);
        layout->addWidget(sl_isocurve_width,2,1);
        layout->addWidget(but_isocurve_color,3,1);
        gbox->setLayout(layout);
        left_col->addWidget(gbox);
    }

    // GRADIENT
    {
        QGroupBox *gbox          = new QGroupBox(widget);
        cb_gradient              = new QCheckBox("Gradient", gbox);
        sl_gradient_size         = new QSlider(Qt::Horizontal, gbox);
        but_gradient_color       = new QPushButton("Color", gbox);
        but_gradient_serialize   = new QPushButton("Serialize", gbox);
        but_gradient_deserialize = new QPushButton("Deserialize", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_gradient->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_gradient_size->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_gradient_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_gradient_serialize->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_gradient_deserialize->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_gradient_size->setMinimum(1);
        sl_gradient_size->setMaximum(100);
        sl_gradient_size->setValue(20);
        gbox->setFont(global_font);
        cb_gradient->setFont(global_font);
        but_gradient_color->setFont(global_font);
        but_gradient_serialize->setFont(global_font);
        but_gradient_deserialize->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_gradient,0,0);
        layout->addWidget(sl_gradient_size,1,0,1,2);
        layout->addWidget(but_gradient_color,2,0,1,2);
        layout->addWidget(but_gradient_serialize,3,0);
        layout->addWidget(but_gradient_deserialize,3,1);
        gbox->setLayout(layout);
        left_col->addWidget(gbox);
    }

    // AMBIENT OCCLUSION
    {
        QGroupBox *gbox      = new QGroupBox(widget);
        sl_ambient_occlusion = new QSlider(Qt::Horizontal, gbox);
        but_compute_AO       = new QPushButton("Compute AO", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_ambient_occlusion->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_compute_AO->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_ambient_occlusion->setMinimum(0);
        sl_ambient_occlusion->setMaximum(99);
        sl_ambient_occlusion->setValue(99);
        gbox->setFont(global_font);
        but_compute_AO->setFont(global_font);
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(sl_ambient_occlusion);
        layout->addWidget(but_compute_AO);
        gbox->setLayout(layout);
        left_col->addWidget(gbox);
    }

    // SLICING
    {
        QGroupBox *gbox = new QGroupBox("Slice" ,widget);
        sl_slice_x      = new QSlider(Qt::Horizontal, gbox);
        sl_slice_y      = new QSlider(Qt::Horizontal, gbox);
        sl_slice_z      = new QSlider(Qt::Horizontal, gbox);
        sl_slice_q      = new QSlider(Qt::Horizontal, gbox);
        sl_slice_l      = new QSlider(Qt::Horizontal, gbox);
        cb_slice_flip_x = new QCheckBox("flip", gbox);
        cb_slice_flip_y = new QCheckBox("flip", gbox);
        cb_slice_flip_z = new QCheckBox("flip", gbox);
        cb_slice_flip_q = new QCheckBox("flip", gbox);
        cb_slice_flip_l = new QCheckBox("flip", gbox);
        rb_slice_AND    = new QRadioButton("&&", gbox);
        rb_slice_OR     = new QRadioButton("||",  gbox);
        but_slice_reset = new QPushButton("Reset", gbox);
        QLabel *l_x     = new QLabel("X");
        QLabel *l_y     = new QLabel("Y");
        QLabel *l_z     = new QLabel("Z");
        QLabel *l_q     = new QLabel("Q");
        QLabel *l_l     = new QLabel("L");
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_x->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_y->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_z->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_slice_flip_x->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_slice_flip_y->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_slice_flip_z->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_slice_flip_q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_slice_flip_l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_slice_AND->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_slice_OR->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_slice_reset->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_x->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_y->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_z->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_q->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_slice_l->setMinimum(-1);
        sl_slice_l->setMaximum(20);
        sl_slice_l->setOrientation(Qt::Horizontal);
        sl_slice_l->setTickPosition(QSlider::TicksBelow);
        sl_slice_x->setValue(sl_slice_x->maximum());
        sl_slice_y->setValue(sl_slice_y->maximum());
        sl_slice_z->setValue(sl_slice_z->maximum());
        sl_slice_q->setValue(sl_slice_q->maximum());
        sl_slice_l->setValue(sl_slice_l->minimum());
        rb_slice_AND->setChecked(true);
        cb_slice_flip_x->setChecked(false);
        cb_slice_flip_y->setChecked(false);
        cb_slice_flip_z->setChecked(false);
        cb_slice_flip_q->setChecked(false);
        gbox->setFont(global_font);
        cb_slice_flip_x->setFont(global_font);
        cb_slice_flip_y->setFont(global_font);
        cb_slice_flip_z->setFont(global_font);
        cb_slice_flip_q->setFont(global_font);
        cb_slice_flip_l->setFont(global_font);
        rb_slice_AND->setFont(global_font);
        rb_slice_OR->setFont(global_font);
        but_slice_reset->setFont(global_font);
        l_x->setFont(global_font);
        l_y->setFont(global_font);
        l_z->setFont(global_font);
        l_q->setFont(global_font);
        l_l->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(but_slice_reset, 0,1);
        layout->addWidget(rb_slice_AND,0,2);
        layout->addWidget(rb_slice_OR,0,3);
        layout->addWidget(l_x,1,0);
        layout->addWidget(l_y,2,0);
        layout->addWidget(l_z,3,0);
        layout->addWidget(l_q,4,0);
        layout->addWidget(l_l,5,0);
        layout->addWidget(sl_slice_x,1,1,1,3);
        layout->addWidget(sl_slice_y,2,1,1,3);
        layout->addWidget(sl_slice_z,3,1,1,3);
        layout->addWidget(sl_slice_q,4,1,1,3);
        layout->addWidget(sl_slice_l,5,1,1,3);
        layout->addWidget(cb_slice_flip_x,1,4);
        layout->addWidget(cb_slice_flip_y,2,4);
        layout->addWidget(cb_slice_flip_z,3,4);
        layout->addWidget(cb_slice_flip_q,4,4);
        layout->addWidget(cb_slice_flip_l,5,4);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
    }

    // MARKED EDGES
    {
        QGroupBox *gbox        = new QGroupBox(widget);
        cb_marked_edges        = new QCheckBox("Marked Edges", gbox);
        sl_marked_edges_width  = new QSlider(Qt::Horizontal, gbox);
        but_marked_edges_color = new QPushButton("Color", gbox);
        QLabel *l_width        = new QLabel("Width:  ", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_marked_edges->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_marked_edges_width->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        but_marked_edges_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_marked_edges_width->setMinimum(1);
        sl_marked_edges_width->setMaximum(10);
        sl_marked_edges_width->setTickPosition(QSlider::TicksBelow);
        sl_marked_edges_width->setTickInterval(1);
        cb_marked_edges->setChecked(true);
        gbox->setFont(global_font);
        but_marked_edges_color->setFont(global_font);
        l_width->setFont(global_font);
        cb_marked_edges->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_marked_edges,0,0,1,2);
        layout->addWidget(l_width,1,0);
        layout->addWidget(sl_marked_edges_width,1,1);
        layout->addWidget(but_marked_edges_color,2,1);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
    }

    // actions
    {
        QGroupBox *gbox = new QGroupBox(widget);
        cb_actions      = new QComboBox(gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_actions->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_actions->insertItem(0,"Action:");
        cb_actions->insertItem(1,"Unmark all edges");
        cb_actions->insertItem(2,"Color wrt label");
        cb_actions->insertItem(3,"Label wrt color");
        cb_actions->insertItem(4,"Mark color discontinuities");
        cb_actions->insertItem(5,"Mark sharp creases (>60deg)");
        cb_actions->insertItem(6,"Mark sharp creases (>30deg)");
        cb_actions->insertItem(7,"Mark boundary edges");
        cb_actions->setFont(global_font);
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(cb_actions);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
    }

    global_layout->addStretch();
    left_col->addStretch();
    right_col->addStretch();
    set_title();
    connect();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::show()
{
    widget->show();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::set_title()
{
    if (m != NULL)
    {
        std::string title = m->mesh_data().filename.c_str();
        title = title.substr(title.find_last_of("/")+1, title.size());
        widget->setWindowTitle(title.c_str());
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::set_tex1d()
{
    if (m == NULL || canvas == NULL) return;
    rb_tex1D->setChecked(true);
    m->show_texture1D(cb_tex1D_type->currentIndex());
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::set_tex2d()
{
    if (m == NULL || canvas == NULL) return;
    rb_tex2D->setChecked(true);
    int    tex_type = cb_tex2D_type->currentIndex() + 5; // first five are for tex1D
    double density  = (double)sl_tex2D_density->value()/10.0;
    if (tex_type == TEXTURE_2D_BITMAP && tex2d_filename.empty())
    {
        tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
    }
    m->show_texture2D(tex_type, density, tex2d_filename.c_str());
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::set_slice()
{
    if (m == NULL || canvas == NULL) return;
    SlicerState s;
    s.X_thresh = float(sl_slice_x->value()) / float(sl_slice_x->maximum());
    s.Y_thresh = float(sl_slice_y->value()) / float(sl_slice_y->maximum());
    s.Z_thresh = float(sl_slice_z->value()) / float(sl_slice_z->maximum());
    s.Q_thresh = float(sl_slice_q->value()) / float(sl_slice_q->maximum());
    s.L_filter = sl_slice_l->value();
    s.X_sign   = cb_slice_flip_x->isChecked() ? GEQ : LEQ;
    s.Y_sign   = cb_slice_flip_y->isChecked() ? GEQ : LEQ;
    s.Z_sign   = cb_slice_flip_z->isChecked() ? GEQ : LEQ;
    s.Q_sign   = cb_slice_flip_q->isChecked() ? GEQ : LEQ;
    s.L_mode   = cb_slice_flip_l->isChecked() ? IS  : IS_NOT;
    s.mode     = rb_slice_AND->isChecked()    ? AND : OR;
    m->slice(s);
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void SurfaceMeshControlPanel<Mesh>::set_isocurve()
{
    if (m == NULL || canvas == NULL) return;
    canvas->pop(&isocontour);
    if (cb_isocurve->isChecked())
    {
        double max           = m->vert_max_uvw_value(U_param);
        double min           = m->vert_min_uvw_value(U_param);
        double isovalue      = min + (max-min) * static_cast<double>(sl_isovalue->value())/99.0;
        int    thickness     = isocontour.thickness;
        Color  color         = isocontour.color;
        isocontour           = DrawableIsocontour<M,V,E,P>(*m, isovalue);
        isocontour.thickness = thickness;
        isocontour.color     = color;
        canvas->push_obj(&isocontour,false);

        std::cout << "Isocontour " << isovalue << std::endl;
    }
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
void SurfaceMeshControlPanel<Mesh>::connect()
{
    QPushButton::connect(but_load, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getOpenFileName(NULL, "Load mesh", ".", "3D Meshes (*.off *.obj *.iv);; OBJ(*.obj);; OFF(*.off);; IV(*.iv)").toStdString();
        if (!filename.empty())
        {
            *m = Mesh(filename.c_str());
            canvas->push_obj(m);
            set_title();
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_save, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        std::string filename = QFileDialog::getSaveFileName(NULL, "Save mesh", ".", "3D Meshes (*.off *.obj *.iv *.stl);; OBJ(*.obj);; OFF(*.off);; IV(*.iv);; STL(*.stl)").toStdString();
        if (!filename.empty()) m->save(filename.c_str());
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_show_mesh, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL) return;
        m->show_mesh(cb_show_mesh->isChecked());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_point_shading, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_point_shading->isChecked()) m->show_mesh_points();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_flat_shading, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_flat_shading->isChecked()) m->show_mesh_flat();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_smooth_shading, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_smooth_shading->isChecked()) m->show_mesh_smooth();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_vert_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_vert_color->isChecked()) m->show_vert_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_poly_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_poly_color->isChecked()) m->show_poly_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_tex1D, &QPushButton::toggled, [&]()
    {
        if (rb_tex1D->isChecked()) set_tex1d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_tex2D, &QPushButton::toggled, [&]()
    {
        if (rb_tex2D->isChecked()) set_tex2d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_tex1D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex1d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_tex2D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex2d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_tex2D_density, &QSlider::valueChanged, [&]()
    {
        if (rb_tex2D->isChecked()) set_tex2d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_load_tex2d, &QPushButton::clicked, [&]()
    {
        tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
        cb_tex2D_type->setCurrentIndex(2);
        set_tex2d();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_vert_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_vert_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->vert_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_poly_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_poly_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->poly_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_serialize_field, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getSaveFileName(NULL, "Serialize field", ".", "").toStdString();
        if (!filename.empty())
        {
            ScalarField sf(m->serialize_uvw(U_param));
            sf.serialize(filename.c_str());
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_deserialize_field, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getOpenFileName(NULL, "Serialize field", ".", "").toStdString();
        if (!filename.empty())
        {
            ScalarField sf(filename.c_str());
            sf.normalize_in_01();
            if ((uint)sf.size() == m->num_verts()) sf.copy_to_mesh(*m);
            else std::cerr << "Could not load scalar field " << filename << " - array size mismatch!" << std::endl;
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_wireframe, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL) return;
        m->show_wireframe(cb_wireframe->isChecked());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_wireframe_width, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_wireframe_width(sl_wireframe_width->value());
        cb_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_wireframe_alpha, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        float alpha = float(sl_wireframe_alpha->value()) / 99.0;
        m->show_wireframe_transparency(alpha);
        cb_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_wireframe_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->show_wireframe_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        cb_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_isocurve, &QCheckBox::stateChanged, [&]()
    {
        set_isocurve();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_isovalue, &QSlider::valueChanged, [&]()
    {
        set_isocurve();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_isocurve_width, &QSlider::valueChanged, [&]()
    {
        isocontour.thickness = sl_isocurve_width->value();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_isocurve_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        isocontour.color = Color(c.redF(), c.greenF(), c.blueF(), c.alphaF());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_gradient, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        canvas->pop(&gradient);
        if (cb_gradient->isChecked())
        {
            Color  color = gradient.get_arrow_color();
            gradient = DrawableVectorField(*m);
            ScalarField f(m->serialize_uvw(U_param));
            gradient = gradient_matrix(*m) * f;
            gradient.normalize();
            gradient.set_arrow_color(color);
            canvas->push_obj(&gradient,false);
        }
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_gradient_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        gradient.set_arrow_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_gradient_size, &QSlider::valueChanged, [&]()
    {
        double l = m->edge_avg_length();
        gradient.set_arrow_size(2.0*l*static_cast<float>(sl_gradient_size->value())/100.0);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_gradient_serialize, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getSaveFileName(NULL, "Serialize gradient", ".", "").toStdString();
        if (!filename.empty()) gradient.serialize(filename.c_str());
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_gradient_deserialize, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getOpenFileName(NULL, "Deserialize gradient", ".", "").toStdString();
        if (!filename.empty()) gradient.deserialize(filename.c_str());
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_slice_x, &QSlider::valueChanged, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_slice_y, &QSlider::valueChanged, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_slice_z, &QSlider::valueChanged, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_slice_q, &QSlider::valueChanged, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_slice_l, &QSlider::valueChanged, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_slice_flip_x, &QCheckBox::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_slice_flip_y, &QCheckBox::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_slice_flip_z, &QCheckBox::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_slice_flip_q, &QCheckBox::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_slice_flip_l, &QCheckBox::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_slice_AND, &QPushButton::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_slice_OR, &QPushButton::toggled, [&]()
    {
        set_slice();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_slice_reset, &QPushButton::clicked, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->slicer_reset();
        canvas->updateGL();
        sl_slice_x->setValue(sl_slice_x->maximum());
        sl_slice_y->setValue(sl_slice_y->maximum());
        sl_slice_z->setValue(sl_slice_z->maximum());
        sl_slice_q->setValue(sl_slice_q->maximum());
        sl_slice_l->setValue(sl_slice_l->minimum());
        rb_slice_AND->setChecked(true);
        cb_slice_flip_x->setChecked(false);
        cb_slice_flip_y->setChecked(false);
        cb_slice_flip_z->setChecked(false);
        cb_slice_flip_q->setChecked(false);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_marked_edges, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_marked_edge(cb_marked_edges->isChecked());
        m->updateGL();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_marked_edges_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->show_marked_edge_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_marked_edges_width, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_marked_edge_width(sl_marked_edges_width->value());
        cb_marked_edges->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_ambient_occlusion, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        float alpha = float(sl_ambient_occlusion->value()) / 99.0;
        m->show_AO_alpha(alpha);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_compute_AO, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        AO_srf<Mesh> ao(*m);
        ao.copy_to_mesh(*m);
        m->updateGL();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_actions, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        switch(cb_actions->currentIndex())
        {
            case 1: m->edge_unmark_all(); break;
            case 2: m->poly_color_wrt_label(); break;
            case 3: m->poly_label_wrt_color(); break;
            case 4: m->edge_mark_color_discontinuities(); break;
            case 5: m->edge_mark_sharp_creases(to_rad(60.0)); break;
            case 6: m->edge_mark_sharp_creases(to_rad(30.0)); break;
            case 7: m->edge_mark_boundaries(); break;
        }
        m->updateGL();
        canvas->updateGL();
    });
}

}
