#ifndef CAMERA_H
#define CAMERA_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* This class implements a virtual camera that observes a 3D scene. It is
 * internally represented following the standard OpenGL's MODEL-VIEW-PROJECTION
 * paradigm, which allows to move back and forth from objet, to world, to eye
 * coordinates with a stack of 4x4 hmogeneous matrices. Differently from OpenGL,
 * where model and view matrices are condensed into a single "modelview" matrix,
 * here the two matrices are kept separated for ease of coding and understanding.
 *
 * This class does not contain a single line of OpenGL code. The idea is that here
 * we provide code to setup the matrices you need, and then these matrices can be
 * passed to your OpenGL context through the following code (which will likely be
 * included into you GLcanvas class, such as our cinolib::GLcanvas)
 *
 *   glMatrixMode(GL_MODELVIEW);
 *   glLoadMatrixd((camera.view * camera.model).transpose().ptr());
 *   glMatrixMode(GL_PROJECTION);
 *   glLoadMatrixd(camera.projection.transpose().ptr());
 *
 * NOTE: matrices are transposed because OpenGL uses a column-major ordering for
 * its matrices, whereas cinolib uses the standard row-major ordering.
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
class Camera
{
    public:

        mat<4,4,T> model;      // from MODEL space to WORLD  space
        mat<4,4,T> view;       // from WORLD space to CAMERA space
        mat<4,4,T> projection; // viewing frustum

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Camera();

        Camera(const mat<3,1,T> & scene_center,
               const T          & scene_radius,
               const T          & asp_ratio);

        Camera(const mat<3,1,T> & eye,
               const mat<3,1,T> & center,
               const mat<3,1,T> & up,
               const T          & asp_ratio);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void look_at(const mat<3,1,T> & eye,
                     const mat<3,1,T> & center,
                     const mat<3,1,T> & up);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void fit_scene(const mat<3,1,T> & scene_center,
                       const T          & scene_radius);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_ortho_frustum(const T & l,    const T & r,  const T & b, const T & t, const T & n, const T & f);
        void set_persp_frustum(const T & l,    const T & r,  const T & b, const T & t, const T & n, const T & f);
        void set_persp_frustum(const T & fovY, const T & ar, const T & n, const T & f);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void zoom     (const T & factor);
        void rotate   (const mat<3,1,T> & axis, const T & deg);
        void translate(const mat<3,1,T> & delta);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize() const;
        void        deserialize(const std::string & s);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void print() const;
};

}

#ifndef  CINO_STATIC_LIB
#include "camera.cpp"
#endif

#endif // CAMERA_H
