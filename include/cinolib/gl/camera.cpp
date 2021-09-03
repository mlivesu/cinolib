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
#include <cinolib/deg_rad.h>

namespace cinolib
{

template<class T>
CINO_INLINE
Camera<T>::Camera(const int width, const int height) : width(width), height(height)
{
    scene_center = mat<3,1,T>((T)0);
    scene_radius = 1;
    reset();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset()
{    
    zoom_factor = 0.5;
    reset_matrices();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset_matrices()
{
    reset_modelview();
    reset_projection();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset_modelview()
{
    // set the scene center at the WORLD origin
    // set the camera outside of the scene radius along WORLD's -Z
    model = mat<4,4,T>::TRANS(-scene_center);
    view  = mat<4,4,T>::TRANS(-mat<3,1,T>(0,0,4*scene_radius));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset_projection()
{
    if(is_ortho()) reset_projection_ortho();
    else           reset_projection_persp();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset_projection_persp()
{
    static constexpr double fov = 55; // vertical field of view, in degrees.
                                      // A 55 degrees fov in perspective mode
                                      // is roughly equivalent to no zoom in
                                      // orthographic mode

    projection = frustum_persp(fov * zoom_factor,    // vertical field of view
                               (double)width/height, // aspect ratio
                               2*scene_radius,       // near
                               6*scene_radius);      // far
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::reset_projection_ortho()
{
    auto ar = (double)width/height; // aspect ratio

    projection = frustum_ortho(-2 * scene_radius * zoom_factor * ar, // left
                                2 * scene_radius * zoom_factor * ar, // right
                               -2 * scene_radius * zoom_factor,      // bottom
                                2 * scene_radius * zoom_factor,      // top
                                2 * scene_radius,                    // near
                                6 * scene_radius);                   // far
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_rotation_pivot(const mat<3,1,T> & p)
{
    // map the pivot to the WORLD space, then position its mapped
    // coordinates at the origin the coordinate reference system
    //
    mat<3,1,T> Mp = (model * p.add_coord(1)).rem_coord();
    model = mat<4,4,T>::TRANS(-Mp) * model;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_focus_point(const mat<3,1,T> & p)
{
    set_rotation_pivot(p);
    zoom(-zoom_factor*0.5);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::zoom(const T & delta)
{    
    zoom_factor += delta;
    zoom_factor  = clamp(zoom_factor, 1e-20, 1e3);
    reset_projection();
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
mat<4,4,T> Camera<T>::MV() const
{
    return view * model;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::MVP() const
{
    return projection * MV();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
std::string Camera<T>::serialize() const
{
    std::stringstream ss;
    ss << width           << " "
       << height          << " "
       << scene_center[0] << " " << scene_center[1] << " " << scene_center[2] << " "
       << scene_radius    << " "
       << zoom_factor     << " "
       << model[ 0]       << " " << model[ 1]      << " " << model[ 2]      << " " << model[ 3]      << " "
       << model[ 4]       << " " << model[ 5]      << " " << model[ 6]      << " " << model[ 7]      << " "
       << model[ 8]       << " " << model[ 9]      << " " << model[10]      << " " << model[11]      << " "
       << model[12]       << " " << model[13]      << " " << model[14]      << " " << model[15]      << " "
       << view[ 0]        << " " << view[ 1]       << " " << view[ 2]       << " " << view[ 3]       << " "
       << view[ 4]        << " " << view[ 5]       << " " << view[ 6]       << " " << view[ 7]       << " "
       << view[ 8]        << " " << view[ 9]       << " " << view[10]       << " " << view[11]       << " "
       << view[12]        << " " << view[13]       << " " << view[14]       << " " << view[15]       << " "
       << projection[ 0]  << " " << projection[ 1] << " " << projection[ 2] << " " << projection[ 3] << " "
       << projection[ 4]  << " " << projection[ 5] << " " << projection[ 6] << " " << projection[ 7] << " "
       << projection[ 8]  << " " << projection[ 9] << " " << projection[10] << " " << projection[11] << " "
       << projection[12]  << " " << projection[13] << " " << projection[14] << " " << projection[15];
    return ss.str();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::deserialize(const std::string & s)
{
    std::stringstream ss(s);
    ss >> width
       >> height
       >> scene_center[0] >> scene_center[1] >> scene_center[2]
       >> scene_radius
       >> zoom_factor
       >> model[ 0]       >> model[ 1]      >> model[ 2]      >> model[ 3]
       >> model[ 4]       >> model[ 5]      >> model[ 6]      >> model[ 7]
       >> model[ 8]       >> model[ 9]      >> model[10]      >> model[11]
       >> model[12]       >> model[13]      >> model[14]      >> model[15]
       >> view[ 0]        >> view[ 1]       >> view[ 2]       >> view[ 3]
       >> view[ 4]        >> view[ 5]       >> view[ 6]       >> view[ 7]
       >> view[ 8]        >> view[ 9]       >> view[10]       >> view[11]
       >> view[12]        >> view[13]       >> view[14]       >> view[15]
       >> projection[ 0]  >> projection[ 1] >> projection[ 2] >> projection[ 3]
       >> projection[ 4]  >> projection[ 5] >> projection[ 6] >> projection[ 7]
       >> projection[ 8]  >> projection[ 9] >> projection[10] >> projection[11]
       >> projection[12]  >> projection[13] >> projection[14] >> projection[15];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::toggle_persp_ortho()
{
    if(is_ortho()) reset_projection_persp();
    else           reset_projection_ortho();
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

// STATIC FACILITIES FOR MATRIX SYNTHESIS ::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
mat<4,4,T> Camera<T>::look_at(const mat<3,1,T> & eye,    // camera pos
                              const mat<3,1,T> & center, // center of scene (rotation pivot)
                              const mat<3,1,T> & up)     // upwards direction
{
    // this is equivalent to gluLookAt()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

    mat<3,1,T> z_axis = center - eye;            z_axis.normalize();
    mat<3,1,T> x_axis = z_axis.cross(up);        x_axis.normalize();
    mat<3,1,T> y_axis = x_axis.cross(z_axis);    y_axis.normalize();

    mat<4,4,T> M({x_axis[0], y_axis[0], -z_axis[0], 0,
                  x_axis[1], y_axis[1], -z_axis[1], 0,
                  x_axis[2], y_axis[2], -z_axis[2], 0,
                      0    ,     0    ,      0    , 1});

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

}
