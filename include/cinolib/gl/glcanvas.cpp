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
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/trackball.h>
#include <cinolib/how_many_seconds.h>
#include <cinolib/fonts/droid_sans.h>
#include <../external/imgui/imgui_impl_opengl2.h>
#include <../external/imgui/imgui_impl_glfw.h>
#include <cinolib/gl/draw_arrow.h>
#include <cinolib/gl/draw_sphere.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
GLcanvas::GLcanvas(const int width, const int height)
{
    // make sure that only the first window
    // in the app creates a ImGui context
    static int window_count = 0;
    owns_ImGui = (window_count==0);
    window_count++;

    camera = Camera<double>(width,height);
    camera.toggle_persp_ortho(); // make it perspective

    glfwInit();
    window = glfwCreateWindow(width, height, "", NULL, NULL);
    if(!window) glfwTerminate();

    glfwSwapInterval(1); // enable vsync

    // pass the window handler (i.e. this whole class) to the GLFW window,
    // so that I can access the methods of GLcanvas2from within the static
    // callbakcs defined within the class itself....
    // Despite ugly, I do not foresee better ways to interact with the
    // GLFW event handling system!
    // See:
    //      https://github.com/glfw/glfw/issues/815
    //      https://stackoverflow.com/questions/55145966/what-does-glfwgetwindowuserpointer-do
    glfwSetWindowUserPointer(window, (void*)this);

    // register GLFW callbacks
    glfwSetWindowSizeCallback (window, window_size_event );
    glfwSetKeyCallback        (window, key_event         );
    glfwSetMouseButtonCallback(window, mouse_button_event);
    glfwSetCursorPosCallback  (window, cursor_event      );
    glfwSetScrollCallback     (window, scroll_event      );

    // intialize OpenGL environment
    glfwMakeContextCurrent(window);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // to handle high DPI displays
    update_DPI_factor();

    if(owns_ImGui)
    {
        // initialize ImGui backend for visual controls...
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, false);
        ImGui_ImplOpenGL2_Init();
        // NOTE: since ImGui does not support dynamic font sizing, I am using oversized fonts
        // (10x) to account for zoom factor (downscaling is visually better than upscaling)
        // https://github.com/ocornut/imgui/issues/797
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = NULL;
        io.Fonts->Clear();
        io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_data, droid_sans_size, font_size*10);
        io.FontGlobalScale = 0.1; // compensate for high-res fonts
    }

    window_size_event(window, width, height);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
GLcanvas::~GLcanvas()
{
    if(owns_ImGui)
    {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push(const DrawableObject *obj, const bool refit_scene)
{
    drawlist.push_back(obj);

    if(obj->scene_radius()>0 && refit_scene)
    {
        this->refit_scene();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push(SideBarItem *item)
{
    side_bar_items.push_back(item);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push(const Marker & m)
{
    // ImGui uses 16 bits to index items. It cannot handle more than this
    //if(markers.size() >= (1 << 16)) return;
    markers.push_back(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_marker(const vec2d       & p,
                           const std::string & text,
                           const Color         color,
                           const uint          disk_radius,
                           const uint          font_size)
{
    Marker m;
    m.pos_2d      = p;
    m.text        = text;
    m.color       = color;
    m.disk_radius = disk_radius;
    m.font_size   = font_size;
    push(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::push_marker(const vec3d       & p,
                           const std::string & text,
                           const Color         color,
                           const uint          disk_radius,
                           const uint          font_size)
{
    Marker m;
    m.pos_3d      = p;
    m.text        = text;
    m.color       = color;
    m.disk_radius = disk_radius;
    m.font_size   = font_size;
    push(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::pop(const DrawableObject *obj)
{
    // pops the first occurrence of obj (if found)
    for(std::vector<const DrawableObject*>::iterator it=drawlist.begin(); it!=drawlist.end(); ++it)
    {
        if(obj==*it)
        {
            drawlist.erase(it);
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::pop_all_markers()
{
    markers.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::refit_scene()
{
    // update camera scene parameters
    camera.scene_center = vec3d(0.0);
    camera.scene_radius = 0;
    uint count = 0;
    for(auto obj : drawlist)
    {
        if(obj->scene_radius()>0)
        {
            camera.scene_center += obj->scene_center();
            camera.scene_radius += obj->scene_radius();
            ++count;
        }
    }
    camera.scene_center /= static_cast<double>(count);
    camera.scene_radius /= static_cast<double>(count);

    // update camera matrices and pass them to the GL system
    camera.reset_matrices();
    update_GL_matrices();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_GL_matrices() const
{
    update_GL_modelview();
    update_GL_projection();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_GL_modelview() const
{
    glfwMakeContextCurrent(window);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd((camera.view * camera.model).transpose().ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_GL_projection() const
{
    glfwMakeContextCurrent(window);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(camera.projection.transpose().ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw()
{
    glfwMakeContextCurrent(window);
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // draw your 3D scene
    for(auto obj : drawlist) obj->draw();

    if(owns_ImGui)
    {
        // draw markers and visual controls
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        draw_markers();
        draw_side_bar();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    if(show_axis) draw_axis();

    glfwSwapBuffers(window);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_axis() const
{
    vec3d  O = camera.scene_center;
    vec3d  X = O + vec3d(1,0,0)*camera.scene_radius;
    vec3d  Y = O + vec3d(0,1,0)*camera.scene_radius;
    vec3d  Z = O + vec3d(0,0,1)*camera.scene_radius;
    double r = camera.scene_radius*0.01;
    glfwMakeContextCurrent(window);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    cylinder(O, X, r, r, Color::RED().rgba);
    cylinder(O, Y, r, r, Color::GREEN().rgba);
    cylinder(O, Z, r, r, Color::BLUE().rgba);
    sphere(O._vec, r*2, Color::WHITE().rgba);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// WARNING: depth culling 3D markers seems buggy and may crash!
// There is some weird memory leak involving buffer allocation
// and Z-buffer access. I could not entirely fix it...
//
CINO_INLINE
void GLcanvas::draw_markers() const
{
    assert(owns_ImGui && "Only the first canvas created handles the ImGui context");

    if(markers.empty()) return;

    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    bool visible = true;
    ImGui::Begin("Markers", &visible, ImGuiWindowFlags_NoTitleBar            |
                                      ImGuiWindowFlags_NoResize              |
                                      ImGuiWindowFlags_NoMove                |
                                      ImGuiWindowFlags_NoScrollbar           |
                                      ImGuiWindowFlags_NoBringToFrontOnFocus |
                                      ImGuiWindowFlags_NoScrollWithMouse     |
                                      ImGuiWindowFlags_NoCollapse            |
                                      ImGuiWindowFlags_NoSavedSettings       |
                                      ImGuiWindowFlags_NoInputs);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // if marker culling is enabled, read the Z buffer to depth-test 3D markers
    GLint    W     = camera.width  * DPI_factor;
    GLint    H     = camera.height * DPI_factor;
    GLfloat *z_buf = (depth_cull_markers) ? new GLfloat[W*H] : nullptr;
    if(depth_cull_markers) whole_Z_buffer(z_buf);

    for(const Marker & m : markers)
    {
        vec2d pos = m.pos_2d;
        if(pos.is_inf()) // 3D marker
        {
            assert(!m.pos_3d.is_inf());
            GLdouble z;
            project(m.pos_3d, pos, z);
            int x  = pos.x()*DPI_factor;
            int y  = pos.y()*DPI_factor;
            // marker is outside the frustum
            if(z<0 || z>=1 || x<=0 || x>=W || y<=0 || y>=H) continue;
            // marker is occluded in the current view
            assert((x+W*(H-y-1)<W*H));
            if(depth_cull_markers && fabs(z-z_buf[x+W*(H-y-1)])>0.01) continue;
        }
        // adjust marker size based on zoom
        auto zoom_factor = clamp(camera.zoom_factor, 1e-5, 1e10); // avoids overflow inside ImGui radius calculation
        uint zoom_radius = 0.5*m.disk_radius/zoom_factor;
        uint zoom_font_s = 0.5*m.font_size/zoom_factor;
        //
        if(m.disk_radius>0)
        {
            // ImGui uses 16bits to index vertices, hence it is likely to overflow if there are too many markers.
            //  - I noticed that the automatic segment count immediately triggers overflow with large amounts of zoom,
            //    so I am always approximating circles with 20-gons
            //  - Besides, I should probably use something like AddSquare or AddRect to save limit polygon vertices....
            drawList->AddCircleFilled(ImVec2(pos.x(),pos.y()),
                                      zoom_radius,
                                      ImGui::GetColorU32(ImVec4(m.color.r, m.color.g, m.color.b, m.color.a)), 20);
        }
        if(m.font_size>0 && m.text.length()>0)
        {
            drawList->AddText(ImGui::GetFont(),
                              zoom_font_s,
                              ImVec2(pos.x()+zoom_radius, pos.y()-2*zoom_radius),
                              ImGui::GetColorU32(ImVec4(m.color.r, m.color.g, m.color.b, m.color.a)),
                              &m.text[0],
                              &m.text[0] + m.text.size());
        }
    }
    if(depth_cull_markers) delete[] z_buf;
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::draw_side_bar()
{
    if(callback_app_controls==nullptr && side_bar_items.empty()) return;

    assert(owns_ImGui && "Only the first canvas created handles the ImGui context");

    ImGui::SetNextWindowPos({0,0}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({camera.width*side_bar_width, camera.height*1.f}, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(side_bar_alpha);
    ImGui::Begin("MENU");
    if(callback_app_controls!=nullptr)
    {
        ImGui::SetNextItemOpen(true,ImGuiCond_Once);
        if(ImGui::TreeNode("APP CONTROLS"))
        {
            callback_app_controls();
            ImGui::TreePop();
        }
    }
    for(auto item : side_bar_items)
    {
        ImGui::SetNextItemOpen(item->show_open,ImGuiCond_Once);
        if(ImGui::TreeNode(item->name.c_str()))
        {
            item->draw();
            ImGui::TreePop();
        }
    }    
    // this allows the user to interactively resize the width of the side bar
    side_bar_width = ImGui::GetWindowWidth() / camera.width;
    ImGui::End();        
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int GLcanvas::launch(std::initializer_list<GLcanvas*> additional_windows)
{
    while(true)
    {
        glfwMakeContextCurrent(window);
        draw();
        if(glfwWindowShouldClose(window)) return EXIT_SUCCESS;

        for(auto it=additional_windows.begin(); it!=additional_windows.end(); ++it)
        {
            glfwMakeContextCurrent((*it)->window);
            (*it)->draw();
            if(glfwWindowShouldClose((*it)->window)) return EXIT_SUCCESS;
        }

        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::update_DPI_factor()
{
    // https://www.glfw.org/docs/latest/intro_guide.html#coordinate_systems
    // https://www.glfw.org/docs/latest/window_guide.html#window_fbsize
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    DPI_factor = (float)fb_width/camera.width;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec2d GLcanvas::cursor_pos() const
{
    vec2d pos;
    glfwGetCursorPos(window, &pos.x(), &pos.y());
    return pos;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::whole_Z_buffer(GLfloat * buf) const
{
    glfwMakeContextCurrent(window);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, buf);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
GLfloat GLcanvas::query_Z_buffer(const vec2d & p) const
{
    glfwMakeContextCurrent(window);
    GLint x = p.x()         * DPI_factor;
    GLint y = p.y()         * DPI_factor;
    GLint H = camera.height * DPI_factor;
    GLfloat depth;
    glReadPixels(x, H-1-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool GLcanvas::unproject(const vec2d & p2d, vec3d & p3d) const
{
    // retrieve the missing z coordinate from the Z buffer, then unproject
    return unproject(p2d, query_Z_buffer(p2d), p3d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this is equivalent to gluUnproject
// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluUnProject.xml
//
CINO_INLINE
bool GLcanvas::unproject(const vec2d & p2d, const GLfloat & depth, vec3d & p3d) const
{
    // depth must be in the closed range (0,1)
    if(depth<=0 || depth>=1) return false;

    GLint viewport[4] = { 0,             camera.height,
                          camera.width, -camera.height};

    vec4d p({(p2d.x()-viewport[0])/viewport[2]*2 - 1,
             (p2d.y()-viewport[1])/viewport[3]*2 - 1,
                                         depth*2 - 1,
                                                   1});
    vec4d res = camera.MVP().inverse() * p;

    // convert from homogeneous to cartesian coordinates
    res /= res[3];
    p3d  = res.rem_coord();
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this is equivalent to gluProject
// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluProject.xml
// https://www.khronos.org/opengl/wiki/GluProject_and_gluUnProject_code
// https://stackoverflow.com/questions/54385727/gluunproject-giving-incorrect-values-on-macretina-display-but-works-correctl
//
CINO_INLINE
void GLcanvas::project(const vec3d & p3d, vec2d & p2d, GLdouble & depth) const
{
    GLint viewport[4] = { 0,             camera.height,
                          camera.width, -camera.height};

    vec4d res = (camera.MVP() * p3d.add_coord(1)); // add one (homogeneous) coordinate to p3d
    res /= res[3];                                 // convert from homogeneous to cartesian coordinates

    p2d.x() = viewport[0] + (viewport[2]*(res[0]+1))/2.0;
    p2d.y() = viewport[1] + (viewport[3]*(res[1]+1))/2.0;
    depth   = (res[2]+1)/2.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::: STATIC GLFW CALLBACKS :::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::window_size_event(GLFWwindow *window, int width, int height)
{
    GLcanvas* v = static_cast<GLcanvas*>(glfwGetWindowUserPointer(window));
    v->camera.height           = height;
    v->camera.width            = width;
    v->trackball.last_click_2d = vec2d(inf_double); // fixes crazy translation deltas after window resizing!
    v->camera.reset_projection();                   // update the camera frustum
    v->update_GL_projection();                      // update OpenGL's projection matrix
    v->draw();                                      // refresh canvas while resizing
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::key_event(GLFWwindow *window, int key, int /*scancode*/, int action, int modifiers)
{
    // if visual controls claim the event, let them handle it!
    if(ImGui::GetIO().WantCaptureKeyboard) return;

    // handle repeated keys as if they were a sequence of single key press events
    if(action==GLFW_PRESS || action==GLFW_REPEAT)
    {
        GLcanvas* v = static_cast<GLcanvas*>(glfwGetWindowUserPointer(window));

        if(v->callback_key_pressed) v->callback_key_pressed(key,modifiers);

        if(modifiers & GLFW_MOD_SHIFT) // handle SHIFT + KEY events...
        {
            double delta_t = v->camera.scene_radius*0.01; // translation factor

            switch(key)
            {
                // translations along major axes
                case GLFW_KEY_LEFT  : v->camera.translate_x(-delta_t); break;
                case GLFW_KEY_RIGHT : v->camera.translate_x(+delta_t); break;
                case GLFW_KEY_UP    : v->camera.translate_y(+delta_t); break;
                case GLFW_KEY_DOWN  : v->camera.translate_y(-delta_t); break;
                case GLFW_KEY_1     : v->camera.translate_z(+delta_t); break;
                case GLFW_KEY_2     : v->camera.translate_z(-delta_t); break;
                // camera control
                case GLFW_KEY_Z     : v->camera.zoom(0.01); break; // zoom out
            }                        
        }
        else if(modifiers & (GLFW_MOD_CONTROL|GLFW_MOD_SUPER)) // handle CTRL/CMD + KEY events...
        {
            switch(key)
            {
                // camera control (copy/paste POV)
                case GLFW_KEY_C : glfwSetClipboardString(window, v->camera.serialize().c_str()); break;
                case GLFW_KEY_V : v->camera.deserialize(glfwGetClipboardString(window));
                                  // interesting resrouce to manage to correctly switch from full screen to windowed mode
                                  // https://stackoverflow.com/questions/47402766/switching-between-windowed-and-full-screen-in-opengl-glfw-3-2
                                  // meanwhile....
                                  glfwSetWindowSize(window, v->camera.width, v->camera.height);
                                  break;
            }
        }
        else // handle KEY events wihtout modifiers...
        {
            switch(key)
            {
                // rotations around major axes (in degrees)
                case GLFW_KEY_LEFT  : v->camera.rotate_y(-3); break;
                case GLFW_KEY_RIGHT : v->camera.rotate_y(+3); break;
                case GLFW_KEY_UP    : v->camera.rotate_x(-3); break;
                case GLFW_KEY_DOWN  : v->camera.rotate_x(+3); break;
                case GLFW_KEY_1     : v->camera.rotate_z(+3); break;
                case GLFW_KEY_2     : v->camera.rotate_z(-3); break;
                // camera control
                case GLFW_KEY_O     : v->camera.toggle_persp_ortho(); break;
                case GLFW_KEY_Z     : v->camera.zoom(-0.01);          break; // zoom in
                case GLFW_KEY_R     : v->camera.reset();              break;
                case GLFW_KEY_A     : v->show_axis = !v->show_axis;   break; // toggle show axis
            }
        }
        v->update_GL_matrices();
        v->draw();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::mouse_button_event(GLFWwindow *window, int button, int action, int modifiers)
{
    // if visual controls claim the event, let them handle it
    if(ImGui::GetIO().WantCaptureMouse) return;

    GLcanvas* v = static_cast<GLcanvas*>(glfwGetWindowUserPointer(window));

    if(action==GLFW_RELEASE)
    {
        v->trackball.mouse_pressed = false;
    }
    else // GLFW_PRESS
    {
        assert(action==GLFW_PRESS);

        // thanks GLFW for asking me to handle the single/double click burden...
        auto double_click = [&]() -> bool
        {
            auto   t  = std::chrono::high_resolution_clock::now();
            double dt = how_many_seconds(v->trackball.t_last_click,t);
            v->trackball.t_last_click = t;
            return (dt < 0.2);
        };

        vec2d click = v->cursor_pos();

        if(double_click())
        {
            if(button==GLFW_MOUSE_BUTTON_LEFT)
            {
                if(v->callback_mouse_left_click2) v->callback_mouse_left_click2(modifiers);

                vec3d click_3d;
                if(v->unproject(click, click_3d))
                {
                    v->camera.set_focus_point(click_3d);
                    v->update_GL_matrices();
                }
            }
            else if(button==GLFW_MOUSE_BUTTON_RIGHT)
            {
                if(v->callback_mouse_right_click2) v->callback_mouse_right_click2(modifiers);
            }
        }
        else // single click
        {
            if(button==GLFW_MOUSE_BUTTON_LEFT)
            {
                if(v->callback_mouse_left_click) v->callback_mouse_left_click(modifiers);
                v->trackball.mouse_pressed = true;
                v->trackball.last_click_2d = click;
                v->trackball.last_click_3d = trackball_to_sphere(click, v->camera.width, v->camera.height);
            }
            else if(button==GLFW_MOUSE_BUTTON_RIGHT)
            {
                if(v->callback_mouse_right_click) v->callback_mouse_right_click(modifiers);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::cursor_event(GLFWwindow *window, double x_pos, double y_pos)
{
    GLcanvas* v = static_cast<GLcanvas*>(glfwGetWindowUserPointer(window));

    if(v->callback_cursor) v->callback_cursor(x_pos,y_pos);

    // mouse move + left click => rotate
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS)
    {
        vec2d click_2d(x_pos, y_pos);
        vec3d click_3d = trackball_to_sphere(click_2d, v->camera.width, v->camera.height);
        if(v->trackball.mouse_pressed)
        {
            vec3d  axis;
            double angle;
            trackball_to_rotations(v->trackball.last_click_3d, click_3d, axis, angle);
            v->camera.rotate(angle,axis);
            v->update_GL_matrices();
            v->draw();
        }
        v->trackball.last_click_2d = click_2d;
        v->trackball.last_click_3d = click_3d;
    }
    // mouse move + right click => translate
    else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS)
    {
        vec2d click(x_pos, y_pos);
        if(!v->trackball.last_click_2d.is_inf())
        {
            vec2d delta = (v->trackball.last_click_2d - click);
            delta.normalize();
            delta *= v->camera.scene_radius * 0.01;
            v->camera.translate(vec3d(-delta.x(), delta.y(), 0));
            v->update_GL_matrices();
            v->draw();
        }
        v->trackball.last_click_2d = click;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void GLcanvas::scroll_event(GLFWwindow *window, double x_offset, double y_offset)
{
    GLcanvas* v = static_cast<GLcanvas*>(glfwGetWindowUserPointer(window));

    // if visual controls claim the event, let them handle it
    if(ImGui::GetIO().WantCaptureMouse) return;

    if(v->callback_scroll) v->callback_scroll(x_offset, y_offset);

    v->camera.zoom(y_offset*0.01);
    v->camera.reset_projection();
    v->update_GL_projection();
    // TODO: add a zooming function to change speed according to the amount of current zoom
}

}
