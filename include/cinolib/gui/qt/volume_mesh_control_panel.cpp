/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/gui/qt/volume_mesh_control_panel.h>
#include <cinolib/textures/textures.h>
#include <cinolib/meshes/mesh_slicer.h>
#include <cinolib/gradient.h>
#include <cinolib/symbols.h>
#include <iostream>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
VolumeMeshControlPanel<Mesh>::VolumeMeshControlPanel(Mesh *m, GLcanvas *canvas, QWidget *parent)
    : m(m)
    , canvas(canvas)
{
    assert(canvas != NULL);

    widget                  = new QWidget(parent);
    global_layout           = new QHBoxLayout(widget);
    QVBoxLayout *left_col   = new QVBoxLayout();
    QVBoxLayout *middle_col = new QVBoxLayout();
    QVBoxLayout *right_col  = new QVBoxLayout();
    global_layout->addLayout(left_col);
    global_layout->addLayout(middle_col);
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

    // COLORS / TEXTURES (OUTSIDE)
    {
        QGroupBox *gbox        = new QGroupBox("Colors/Textures (OUTSIDE)",widget);
        rb_out_vert_color      = new QRadioButton("Vert color", gbox);
        rb_out_face_color      = new QRadioButton("Face color", gbox);
        rb_out_poly_color      = new QRadioButton("Poly color", gbox);
        rb_out_qual_color      = new QRadioButton("Quality", gbox);
        rb_out_tex1D           = new QRadioButton("Text 1D", gbox);
        rb_out_tex2D           = new QRadioButton("Text 2D", gbox);
        but_set_out_vert_color = new QPushButton("Set", gbox);
        but_set_out_face_color = new QPushButton("Set", gbox);
        but_set_out_poly_color = new QPushButton("Set", gbox);
        cb_out_tex1D_type      = new QComboBox(gbox);
        cb_out_tex2D_type      = new QComboBox(gbox);
        sl_out_tex2D_density   = new QSlider(Qt::Horizontal, gbox);
        but_load_out_tex2d     = new QPushButton("Load Tex2D", gbox);
        out_tex2d_filename     = "";
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_face_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_qual_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_tex1D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_tex2D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_out_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_out_face_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_out_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_out_tex1D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_out_tex2D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_out_tex2D_density->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_load_out_tex2d->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_out_poly_color->setChecked(true);
        cb_out_tex1D_type->insertItem(0,"ISO");
        cb_out_tex1D_type->insertItem(1,"RAMP");
        cb_out_tex1D_type->insertItem(2,"RAMP + ISO");
        cb_out_tex2D_type->insertItem(0,"ISO");
        cb_out_tex2D_type->insertItem(1,"CB");
        cb_out_tex2D_type->insertItem(2,"IMG");
        sl_out_tex2D_density->setMinimum(1);
        sl_out_tex2D_density->setMaximum(200);
        sl_out_tex2D_density->setValue(10);
        sl_out_tex2D_density->setSliderPosition(10);
        gbox->setFont(global_font);
        rb_out_face_color->setEnabled(false); // not supported yet
        but_set_out_face_color->setEnabled(false); // not supported yet
        rb_out_vert_color->setFont(global_font);
        rb_out_face_color->setFont(global_font);
        rb_out_poly_color->setFont(global_font);
        rb_out_qual_color->setFont(global_font);
        rb_out_tex1D->setFont(global_font);
        rb_out_tex2D->setFont(global_font);
        but_set_out_vert_color->setFont(global_font);
        but_set_out_face_color->setFont(global_font);
        but_set_out_poly_color->setFont(global_font);
        but_load_out_tex2d->setFont(global_font);
        cb_out_tex1D_type->setFont(global_font);
        cb_out_tex2D_type->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(rb_out_vert_color,0,0);
        layout->addWidget(rb_out_face_color,1,0);
        layout->addWidget(rb_out_poly_color,2,0);
        layout->addWidget(rb_out_qual_color,3,0);
        layout->addWidget(rb_out_tex1D,4,0);
        layout->addWidget(rb_out_tex2D,5,0);
        layout->addWidget(but_set_out_vert_color,0,1);
        layout->addWidget(but_set_out_face_color,1,1);
        layout->addWidget(but_set_out_poly_color,2,1);
        layout->addWidget(cb_out_tex1D_type,4,1);
        layout->addWidget(cb_out_tex2D_type,5,1);
        layout->addWidget(but_load_out_tex2d, 6,0);
        layout->addWidget(sl_out_tex2D_density, 6,1);
        gbox->setLayout(layout);
        middle_col->addWidget(gbox);
    }

    // COLORS / TEXTURES (INSIDE)
    {
        QGroupBox *gbox        = new QGroupBox("Colors/Textures (INSIDE)",widget);
        rb_in_vert_color      = new QRadioButton("Vert color", gbox);
        rb_in_face_color      = new QRadioButton("Face color", gbox);
        rb_in_poly_color      = new QRadioButton("Poly color", gbox);
        rb_in_qual_color      = new QRadioButton("Quality", gbox);
        rb_in_tex1D           = new QRadioButton("Text 1D", gbox);
        rb_in_tex2D           = new QRadioButton("Text 2D", gbox);
        but_set_in_vert_color = new QPushButton("Set", gbox);
        but_set_in_face_color = new QPushButton("Set", gbox);
        but_set_in_poly_color = new QPushButton("Set", gbox);
        cb_in_tex1D_type      = new QComboBox(gbox);
        cb_in_tex2D_type      = new QComboBox(gbox);
        sl_in_tex2D_density   = new QSlider(Qt::Horizontal, gbox);
        but_load_in_tex2d     = new QPushButton("Load Tex2D", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_face_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_qual_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_tex1D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        rb_in_tex2D->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_in_vert_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_in_face_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_set_in_poly_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_in_tex1D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_in_tex2D_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_in_tex2D_density->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_load_in_tex2d->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        in_tex2d_filename     = "";
        rb_in_poly_color->setChecked(true);
        cb_in_tex1D_type->insertItem(0,"ISO");
        cb_in_tex1D_type->insertItem(1,"RAMP");
        cb_in_tex1D_type->insertItem(2,"RAMP + ISO");
        cb_in_tex2D_type->insertItem(0,"ISO");
        cb_in_tex2D_type->insertItem(1,"CB");
        cb_in_tex2D_type->insertItem(2,"IMG");
        sl_in_tex2D_density->setMinimum(1);
        sl_in_tex2D_density->setMaximum(200);
        sl_in_tex2D_density->setValue(10);
        sl_in_tex2D_density->setSliderPosition(10);
        gbox->setFont(global_font);
        rb_in_face_color->setEnabled(false); // not supported yet
        but_set_in_face_color->setEnabled(false); // not supported yet
        rb_in_vert_color->setFont(global_font);
        rb_in_face_color->setFont(global_font);
        rb_in_poly_color->setFont(global_font);
        rb_in_qual_color->setFont(global_font);
        rb_in_tex1D->setFont(global_font);
        rb_in_tex2D->setFont(global_font);
        but_set_in_vert_color->setFont(global_font);
        but_set_in_face_color->setFont(global_font);
        but_set_in_poly_color->setFont(global_font);
        but_load_in_tex2d->setFont(global_font);
        cb_in_tex1D_type->setFont(global_font);
        cb_in_tex2D_type->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(rb_in_vert_color,0,0);
        layout->addWidget(rb_in_face_color,1,0);
        layout->addWidget(rb_in_poly_color,2,0);
        layout->addWidget(rb_in_qual_color,3,0);
        layout->addWidget(rb_in_tex1D,4,0);
        layout->addWidget(rb_in_tex2D,5,0);
        layout->addWidget(but_set_in_vert_color,0,1);
        layout->addWidget(but_set_in_face_color,1,1);
        layout->addWidget(but_set_in_poly_color,2,1);
        layout->addWidget(cb_in_tex1D_type,4,1);
        layout->addWidget(cb_in_tex2D_type,5,1);
        layout->addWidget(but_load_in_tex2d, 6,0);
        layout->addWidget(sl_in_tex2D_density, 6,1);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
    }

    // WIREFRAME (OUTSIDE)
    {
        QGroupBox *gbox         = new QGroupBox(widget);
        cb_out_wireframe        = new QCheckBox("Wireframe (OUTSIDE)", gbox);
        sl_out_wireframe_width  = new QSlider(Qt::Horizontal, gbox);
        sl_out_wireframe_alpha  = new QSlider(Qt::Horizontal, gbox);
        but_out_wireframe_color = new QPushButton("Color", gbox);
        QLabel *l_width         = new QLabel("Width: ", gbox);
        QLabel *l_alpha         = new QLabel("Transp:", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_out_wireframe->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_out_wireframe_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_out_wireframe_alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_out_wireframe_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_out_wireframe->setChecked(true);
        sl_out_wireframe_width->setMinimum(1);
        sl_out_wireframe_width->setMaximum(4);
        sl_out_wireframe_width->setTickPosition(QSlider::TicksBelow);
        sl_out_wireframe_width->setTickInterval(1);
        sl_out_wireframe_alpha->setMinimum(0);
        sl_out_wireframe_alpha->setMaximum(99);
        sl_out_wireframe_alpha->setValue(99);
        gbox->setFont(global_font);
        cb_out_wireframe->setFont(global_font);
        sl_out_wireframe_width->setFont(global_font);
        sl_out_wireframe_alpha->setFont(global_font);
        but_out_wireframe_color->setFont(global_font);
        l_width->setFont(global_font);
        l_alpha->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_out_wireframe,0,1);
        layout->addWidget(l_width,1,0);
        layout->addWidget(l_alpha,2,0);
        layout->addWidget(sl_out_wireframe_width,1,1);
        layout->addWidget(sl_out_wireframe_alpha,2,1);
        layout->addWidget(but_out_wireframe_color,3,1);
        gbox->setLayout(layout);
        middle_col->addWidget(gbox);
    }

    // WIREFRAME (INSIDE)
    {
        QGroupBox *gbox        = new QGroupBox(widget);
        cb_in_wireframe        = new QCheckBox("Wireframe (INSIDE)", gbox);
        sl_in_wireframe_width  = new QSlider(Qt::Horizontal, gbox);
        sl_in_wireframe_alpha  = new QSlider(Qt::Horizontal, gbox);
        but_in_wireframe_color = new QPushButton("Color", gbox);
        QLabel *l_width        = new QLabel("Width: ", gbox);
        QLabel *l_alpha        = new QLabel("Transp:", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_in_wireframe->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_in_wireframe_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_in_wireframe_alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_in_wireframe_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_width->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_in_wireframe->setChecked(true);
        sl_in_wireframe_width->setMinimum(1);
        sl_in_wireframe_width->setMaximum(4);
        sl_in_wireframe_width->setTickPosition(QSlider::TicksBelow);
        sl_in_wireframe_width->setTickInterval(1);
        sl_in_wireframe_alpha->setMinimum(0);
        sl_in_wireframe_alpha->setMaximum(99);
        sl_in_wireframe_alpha->setValue(99);
        gbox->setFont(global_font);
        cb_in_wireframe->setFont(global_font);
        sl_in_wireframe_width->setFont(global_font);
        sl_in_wireframe_alpha->setFont(global_font);
        but_in_wireframe_color->setFont(global_font);
        l_width->setFont(global_font);
        l_alpha->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_in_wireframe,0,1);
        layout->addWidget(l_width,1,0);
        layout->addWidget(l_alpha,2,0);
        layout->addWidget(sl_in_wireframe_width,1,1);
        layout->addWidget(sl_in_wireframe_alpha,2,1);
        layout->addWidget(but_in_wireframe_color,3,1);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
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
        layout->addWidget(cb_gradient,0,1);
        layout->addWidget(sl_gradient_size,1,1,1,2);
        layout->addWidget(but_gradient_color,2,1,1,2);
        layout->addWidget(but_gradient_serialize,3,1);
        layout->addWidget(but_gradient_deserialize,3,2);
        gbox->setLayout(layout);
        middle_col->addWidget(gbox);
    }

    // ISO-SURFACES
    {
        QGroupBox *gbox       = new QGroupBox(widget);
        cb_isosurface         = new QCheckBox("Isosurface", gbox);
        sl_isovalue           = new QSlider(Qt::Horizontal, gbox);
        but_isosurface_color  = new QPushButton("Color", gbox);
        but_isosurface_save   = new QPushButton("Save", gbox);
        but_serialize_field   = new QPushButton("Serialize", gbox);
        but_deserialize_field = new QPushButton("Deserialize", gbox);
        QLabel *l_value       = new QLabel("Isoval: ", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_isosurface->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_isovalue->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_isosurface_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_isosurface_save->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_serialize_field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_deserialize_field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        l_value->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sl_isovalue->setMaximum(999);
        sl_isovalue->setValue(499);
        sl_isovalue->setSliderPosition(499);
        gbox->setFont(global_font);
        but_serialize_field->setFont(global_font);
        but_deserialize_field->setFont(global_font);
        but_isosurface_color->setFont(global_font);
        but_isosurface_save->setFont(global_font);
        l_value->setFont(global_font);
        cb_isosurface->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_isosurface,0,1,1,2);
        layout->addWidget(l_value,1,0);
        layout->addWidget(sl_isovalue,1,1,1,2);
        layout->addWidget(but_isosurface_color,2,1);
        layout->addWidget(but_isosurface_save,2,2);
        layout->addWidget(but_serialize_field,3,1);
        layout->addWidget(but_deserialize_field,3,2);
        gbox->setLayout(layout);
        right_col->addWidget(gbox);
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
        left_col->addWidget(gbox);
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
        cb_marked_edges->setChecked(true);
        sl_marked_edges_width->setMinimum(1);
        sl_marked_edges_width->setMaximum(10);
        sl_marked_edges_width->setTickPosition(QSlider::TicksBelow);
        sl_marked_edges_width->setTickInterval(1);
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
        left_col->addWidget(gbox);
    }

    // MARKED FACES
    {
        QGroupBox *gbox        = new QGroupBox(widget);
        cb_marked_faces        = new QCheckBox("Marked Faces", gbox);
        but_marked_faces_color = new QPushButton("Color", gbox);
        gbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_marked_faces->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        but_marked_faces_color->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        cb_marked_faces->setChecked(true);
        gbox->setFont(global_font);
        but_marked_faces_color->setFont(global_font);
        cb_marked_faces->setFont(global_font);
        QGridLayout *layout = new QGridLayout();
        layout->addWidget(cb_marked_faces,0,0);
        layout->addWidget(but_marked_faces_color,0,1);
        gbox->setLayout(layout);
        left_col->addWidget(gbox);
    }

    global_layout->addStretch();
    left_col->addStretch();
    middle_col->addStretch();
    right_col->addStretch();
    set_title();
    connect();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void VolumeMeshControlPanel<Mesh>::show()
{
    widget->show();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void VolumeMeshControlPanel<Mesh>::set_title()
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
void VolumeMeshControlPanel<Mesh>::set_tex1d(const int in_out)
{
    if (m == NULL || canvas == NULL) return;
    if (in_out == OUTSIDE)
    {
        rb_out_tex1D->setChecked(true);
        m->show_out_texture1D(cb_out_tex1D_type->currentIndex());
    }
    else if (in_out == INSIDE)
    {
        rb_in_tex1D->setChecked(true);
        m->show_in_texture1D(cb_in_tex1D_type->currentIndex());
    }

    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void VolumeMeshControlPanel<Mesh>::set_tex2d(const int in_out)
{
    if (m == NULL || canvas == NULL) return;
    if (in_out == OUTSIDE)
    {
        rb_out_tex2D->setChecked(true);
        int    tex_type = cb_out_tex2D_type->currentIndex() + 3; // first three are for tex1D
        double density  = (double)sl_out_tex2D_density->value()/10.0;
        if (tex_type == TEXTURE_2D_BITMAP && out_tex2d_filename.empty())
        {
            out_tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
        }
        m->show_out_texture2D(tex_type, density, out_tex2d_filename.c_str());
    }
    else if (in_out == INSIDE)
    {
        rb_in_tex2D->setChecked(true);
        int    tex_type = cb_in_tex2D_type->currentIndex() + 3; // first three are for tex1D
        double density  = (double)sl_in_tex2D_density->value()/10.0;
        if (tex_type == TEXTURE_2D_BITMAP && out_tex2d_filename.empty())
        {
            out_tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
        }
        m->show_in_texture2D(tex_type, density, in_tex2d_filename.c_str());
    }
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void VolumeMeshControlPanel<Mesh>::set_slice()
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
void VolumeMeshControlPanel<Mesh>::set_isosurface()
{
    if (m == NULL || canvas == NULL || m->mesh_type() != TETMESH) return;
    canvas->pop(&isosurface);
    if (cb_isosurface->isChecked())
    {
        Color  color            = isosurface.color;
        double isovalue         = static_cast<double>(sl_isovalue->value())/999.0;
        Tetmesh<M,V,E,F,P> *ptr = dynamic_cast<Tetmesh<M,V,E,F,P>*>(m);
        isosurface              = DrawableIsosurface<M,V,E,F,P>(*ptr, isovalue);
        isosurface.color        = color;
        canvas->push_obj(&isosurface,false);
    }
    canvas->updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
void VolumeMeshControlPanel<Mesh>::connect()
{
    QPushButton::connect(but_load, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getOpenFileName(NULL, "Load mesh", ".", "3D Meshes (*.MESH *.HEDRA *.VTU *.VTK)").toStdString();
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
        std::string filename = QFileDialog::getSaveFileName(NULL, "Save mesh", ".", "3D Meshes (*.MESH *.TET *.HEDRA *.VTU *.VTK)").toStdString();
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

    QRadioButton::connect(rb_out_vert_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_out_vert_color->isChecked()) m->show_out_vert_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_out_face_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_out_face_color->isChecked()) m->show_out_face_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_out_poly_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_out_poly_color->isChecked()) m->show_out_poly_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_out_qual_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_out_qual_color->isChecked()) m->show_out_poly_quality();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_out_tex1D, &QPushButton::toggled, [&]()
    {
        if (rb_out_tex1D->isChecked()) set_tex1d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_out_tex2D, &QPushButton::toggled, [&]()
    {
        if (rb_out_tex2D->isChecked()) set_tex2d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_out_tex1D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex1d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_out_tex2D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex2d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_out_tex2D_density, &QSlider::valueChanged, [&]()
    {
        if (rb_out_tex2D->isChecked()) set_tex2d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_load_out_tex2d, &QPushButton::clicked, [&]()
    {
        out_tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
        cb_out_tex2D_type->setCurrentIndex(2);
        set_tex2d(OUTSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_out_vert_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_out_vert_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->vert_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_out_face_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_out_face_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->face_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_out_poly_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_out_poly_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->poly_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_vert_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_in_vert_color->isChecked()) m->show_in_vert_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_face_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_in_face_color->isChecked()) m->show_in_face_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_poly_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_in_poly_color->isChecked()) m->show_in_poly_color();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_qual_color, &QPushButton::toggled, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        if (rb_in_qual_color->isChecked()) m->show_in_poly_quality();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_tex1D, &QPushButton::toggled, [&]()
    {
        if (rb_in_tex1D->isChecked()) set_tex1d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QRadioButton::connect(rb_in_tex2D, &QPushButton::toggled, [&]()
    {
        if (rb_in_tex2D->isChecked()) set_tex2d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_in_tex1D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex1d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QComboBox::connect(cb_in_tex2D_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [&]()
    {
        set_tex2d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_in_tex2D_density, &QSlider::valueChanged, [&]()
    {
        if (rb_in_tex2D->isChecked()) set_tex2d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_load_in_tex2d, &QPushButton::clicked, [&]()
    {
        out_tex2d_filename = QFileDialog::getOpenFileName(NULL, "Load 2D Texture", ".", "").toStdString();
        cb_in_tex2D_type->setCurrentIndex(2);
        set_tex2d(INSIDE);
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_in_vert_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_in_vert_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->vert_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_in_face_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_in_face_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->face_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_set_in_poly_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        rb_in_poly_color->setChecked(true);
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->poly_set_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF())); // both IN and OUT!
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
            if (sf.size() == m->num_verts()) sf.copy_to_mesh(*m);
            else std::cerr << "Could not load scalar field " << filename << " - array size mismatch!" << std::endl;
        }
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_out_wireframe, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL) return;
        m->show_out_wireframe(cb_out_wireframe->isChecked());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_out_wireframe_width, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_out_wireframe_width(sl_out_wireframe_width->value());
        cb_out_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_out_wireframe_alpha, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        float alpha = float(sl_out_wireframe_alpha->value()) / 99.0;
        m->show_out_wireframe_transparency(alpha);
        cb_out_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_out_wireframe_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->show_out_wireframe_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        cb_out_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_in_wireframe, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL) return;
        m->show_in_wireframe(cb_in_wireframe->isChecked());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_in_wireframe_width, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_in_wireframe_width(sl_in_wireframe_width->value());
        cb_in_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_in_wireframe_alpha, &QSlider::valueChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        float alpha = float(sl_in_wireframe_alpha->value()) / 99.0;
        m->show_in_wireframe_transparency(alpha);
        cb_in_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_in_wireframe_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->show_in_wireframe_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        cb_in_wireframe->setChecked(true);
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_isosurface, &QCheckBox::stateChanged, [&]()
    {
        set_isosurface();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QSlider::connect(sl_isovalue, &QSlider::valueChanged, [&]()
    {
        set_isosurface();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_isosurface_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        isosurface.color = Color(c.redF(), c.greenF(), c.blueF(), c.alphaF());
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_isosurface_save, &QPushButton::clicked, [&]()
    {
        std::string filename = QFileDialog::getSaveFileName(NULL, "Save Isosurface", ".", "").toStdString();
        if (!filename.empty()) isosurface.export_as_trimesh().save(filename.c_str());
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QCheckBox::connect(cb_gradient, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        canvas->pop(&gradient);
        if (cb_gradient->isChecked())
        {
            float  size    = gradient.get_arrow_size();
            Color  color   = gradient.get_arrow_color();
            gradient = DrawableVectorField<Mesh>(*m);
            ScalarField f(m->serialize_uvw(U_param));
            gradient = gradient_matrix(*m) * f;
            gradient.normalize();
            gradient.set_arrow_size(size);
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
        gradient.set_arrow_size(2.0 * static_cast<float>(sl_gradient_size->value())/100.0);
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

    QCheckBox::connect(cb_marked_faces, &QCheckBox::stateChanged, [&]()
    {
        if (m == NULL || canvas == NULL) return;
        m->show_marked_face(cb_marked_faces->isChecked());
        m->updateGL();
        canvas->updateGL();
    });

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    QPushButton::connect(but_marked_faces_color, &QPushButton::clicked, [&]()
    {
        if (m == NULL) return;
        QColor c = QColorDialog::getColor(Qt::white, widget, "", QColorDialog::ShowAlphaChannel);
        m->show_marked_face_color(Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()));
        canvas->updateGL();
    });
}

}
