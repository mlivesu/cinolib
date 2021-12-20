/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#ifndef CINO_GLCANVAS_H
#define CINO_GLCANVAS_H

#include <cinolib/gl/gl_glu_glfw.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/visual_control.h>
#include <cinolib/gl/camera.h>
#include <cinolib/min_max_inf.h>
#include <cinolib/color.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct Marker
{
    vec2d       pos_2d      = vec2d(inf_double); // first choice to position the marker
    vec3d       pos_3d      = vec3d(inf_double); // used to position a 3D marker IFF pos_2d is INF
    std::string text        = "";                // text to render. Set to the empty string to not render the text
    Color       color       = Color::BLUE();     // color, for both the text and the disk
    uint        disk_radius =  1;                // disk radius (in pixels). Set to zero to not render the disk
    uint        font_size   = 12;                // font size;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct Trackball
{
    vec2d last_click_2d = vec2d(inf_double);
    vec3d last_click_3d = vec3d(inf_double);
    std::chrono::high_resolution_clock::time_point t_last_click;
    bool  mouse_pressed = false;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class GLcanvas
{
    public:

        GLFWwindow                        *window;
        std::vector<const DrawableObject*> drawlist;
        std::vector<Marker>                markers;
        std::vector<VisualControl*>        visual_controls;
        double                             DPI_factor;
        int                                font_size          = 13;
        bool                               show_axis          = false;
        bool                               depth_cull_markers = false; // skip occluded 3D markers, testing their depth with the Z-buffer

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Camera<double> camera;
        Trackball      trackball;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

         GLcanvas(const int width = 640, const int height = 480);
        ~GLcanvas();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int launch() const; // infinite loop, interleaves rendering and event handling

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw()          const; // single render pass
        void draw_controls() const; // render visual controls
        void draw_markers()  const; // render text labels with ImGui (3d markers are depth tested if culling is enabled)
        void draw_axis()     const; // render the global frame XYZ

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void refit_scene();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push(const DrawableObject *obj, const bool refit_scene = true);
        void push(const Marker         & m);
        void push(      VisualControl  *vc);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void push_marker(const vec2d       & p,
                         const std::string & text        = "",
                         const Color         color       = Color::BLUE(),
                         const uint          disk_radius =  5,
                         const uint          font_size   = 10);

        void push_marker(const vec3d       & p,
                         const std::string & text        = "",
                         const Color         color       = Color::BLUE(),
                         const uint          disk_radius =  5,
                         const uint          font_size   = 10);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool pop(const DrawableObject *obj);
        void pop_all_markers();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_GL_matrices()   const;
        void update_GL_modelview()  const;
        void update_GL_projection() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_DPI_factor();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec2d cursor_pos() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void    whole_Z_buffer(      GLfloat * buf) const;
        GLfloat query_Z_buffer(const vec2d   & p2d) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void project  (const vec3d & p3d, vec2d & p2d, GLdouble & depth)      const;
        bool unproject(const vec2d & p2d, vec3d & p3d)                        const;
        bool unproject(const vec2d & p2d, const GLfloat & depth, vec3d & p3d) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // internal event handlers
        static void window_size_event (GLFWwindow *w, int    width, int    height);
        static void key_event         (GLFWwindow *w, int    key,   int    unused, int action, int modif);
        static void mouse_button_event(GLFWwindow *w, int    butt,  int    action, int modif);
        static void cursor_event      (GLFWwindow *w, double x_pos, double y_pos);
        static void scroll_event      (GLFWwindow *w, double x_off, double y_off);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // these callbacks can be used to execute external code when mouse and keyboard
        // events occurr (e.g. for picking, drawing). If defined, they will be called
        // **before** the internal event handlers
        std::function<void(int    key,      int    modifiers)> callback_key_pressed        = nullptr;
        std::function<void(int    button,   int    modifiers)> callback_mouse_single_click = nullptr;
        std::function<void(int    button,   int    modifiers)> callback_mouse_double_click = nullptr;
        std::function<void(double x_pos,    double y_pos    )> callback_cursor             = nullptr;
        std::function<void(double x_offset, double y_offset )> callback_scroll             = nullptr;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
};

}

#ifndef  CINO_STATIC_LIB
#include "glcanvas.cpp"
#endif

#endif // CINO_GLCANVAS_H
