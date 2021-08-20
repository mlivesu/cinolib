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
#include "camera.h"
#include <cinolib/cot.h>

namespace cinolib
{

template<class T>
CINO_INLINE
Camera<T>::Camera(const int width, const int height) : width(width), height(height)
{
    scene_center = mat<3,1,T>((T)0);
    scene_radius = 0;
    fov          = 55;  // matches no zoom in orthographic mode
    zoom_factor  = 1.0;
    model        = mat<4,4,T>::DIAG(1);
    view         = mat<4,4,T>::DIAG(1);
    projection   = mat<4,4,T>::DIAG(1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::update_matrices()
{
    update_modelview();
    update_projection();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::update_modelview()
{
    // set the scene center at the WORLD origin
    // set the camera outside of the scene radius along WORLD's -Z
    model = mat<4,4,T>::TRANS(-scene_center);
    view  = mat<4,4,T>::TRANS(-mat<3,1,T>(0,0,2*scene_radius));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::update_projection()
{
    if(is_ortho()) update_projection_ortho();
    else           update_projection_persp();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::update_projection_persp()
{
    projection = frustum_persp(fov * zoom_factor,     // vertical fiel of view
                               (double)width/height,  // aspect ratio
                               scene_radius,          // near
                               3*scene_radius);       // far
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::update_projection_ortho()
{
    projection = frustum_ortho(-scene_radius * zoom_factor,  // left
                                scene_radius * zoom_factor,  // right
                               -scene_radius * zoom_factor,  // bottom
                                scene_radius * zoom_factor,  // top
                                scene_radius,                // near
                              3*scene_radius);               // far
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::look_at(const mat<3,1,T> & eye,
                              const mat<3,1,T> & center,
                              const mat<3,1,T> & up)
{
    // this is equivalent to gluLookAt()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

    mat<3,1,T> z_axis = center - eye;            z_axis.normalize();
    mat<3,1,T> x_axis = z_axis.cross(up);        x_axis.normalize();
    mat<3,1,T> y_axis = x_axis.cross(z_axis);    y_axis.normalize();

    mat<4,4,T> M({x_axis[0], y_axis[0], -z_axis[0], 0,
                  x_axis[1], y_axis[1], -z_axis[1], 0,
                  x_axis[2], y_axis[2], -z_axis[2], 0,
                     0    ,     0    ,      0    ,  1});

    mat<4,4,T> V = mat<4,4,T>::TRANS(-eye);

    return V*M;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::frustum_ortho(const T & l, // left
                                    const T & r, // right
                                    const T & b, // bottom
                                    const T & t, // top
                                    const T & n, // near
                                    const T & f) // far
{
    // this is equivalent to glOrtho()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml
    return mat<4,4,T>({2/(r-l),   0,        0,       -(r+l)/(r-l),
                          0,     2/(t-b),   0,       -(t+b)/(t-b),
                          0,      0,      -2/(f-n),  -(f+n)/(f-n),
                          0,      0,        0,             1   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::frustum_persp(const T & l, // left
                                    const T & r, // right
                                    const T & b, // bottom
                                    const T & t, // top
                                    const T & n, // near
                                    const T & f) // far
{
    // this is equivalent to glFrustum()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml
    return mat<4,4,T>({2*n/(r-l),     0,        (r+l)/(r-l),         0,
                          0,       2*n/(t-b),   (t+b)/(t-b),         0,
                          0,          0,       -(f+n)/(f-n), -(2*f*n)/(f-n),
                          0,          0,            -1,              0   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::frustum_persp(const T & fov, // vertical field of view (in degrees)
                                    const T & ar,  // aspect ratio
                                    const T & n,   // near
                                    const T & f)   // far
{
    // this is equivalent to gluPerspective()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    double c = cot(to_rad(fov)*0.5);
    return mat<4,4,T>({c/ar,  0,      0,         0,
                        0,    c,      0,         0,
                        0,    0, (f+n)/(n-f), -(2*f*n)/(f-n),
                        0,    0,     -1,         0         });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::zoom(const T & delta)
{    
    zoom_factor += delta;
    zoom_factor  = clamp(zoom_factor, 1e-20, 1e20);
    update_projection();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::rotate(const T & deg, const mat<3,1,T> & axis)
{
    mat<4,4,T> R;
    mat_set_rot_3d(R._mat, to_rad(deg), axis._vec);
    model = R * model;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::rotate_x(const T & deg)
{
    rotate(deg, mat<3,1,T>(1,0,0));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::rotate_y(const T & deg)
{
    rotate(deg, mat<3,1,T>(0,1,0));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::rotate_z(const T & deg)
{
    rotate(deg, mat<3,1,T>(0,0,1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::translate(const mat<3,1,T> & delta)
{
    mat<4,4,T> Tx;
    mat_set_trans(Tx._mat, delta._vec);
    model = Tx * model;
    // WARNING: large translations may move the scene outside of its original
    // radius and cause trouble with clipping planes. Should I do anything about it?
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::translate_x(const T & delta)
{
    translate(mat<3,1,T>(delta,0,0));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::translate_y(const T & delta)
{
    translate(mat<3,1,T>(0,delta,0));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::translate_z(const T & delta)
{
    translate(mat<3,1,T>(0,0,delta));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
std::string Camera<T>::serialize() const
{

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::deserialize(const std::string & )
{
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::toggle_persp_ortho()
{
    if(is_ortho()) update_projection_persp();
    else           update_projection_ortho();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
bool Camera<T>::is_ortho() const
{
    return (projection[14] == 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::print() const
{
    std::cout << "\nCamera M:\n"  << model
              << "\nCamera V:\n"  << view
              << "\nCamera MV:\n" << view * model
              << "\nCamera PR:\n" << projection << std::endl;
}

}
