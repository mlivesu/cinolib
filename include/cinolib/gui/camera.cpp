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

namespace cinolib
{

template<class T>
CINO_INLINE
Camera<T>::Camera()
{
    model      = mat<4,4,T>::DIAG(1);
    view       = mat<4,4,T>::DIAG(1);
    projection = mat<4,4,T>::DIAG(1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_MV(const mat<3,1,T> & scene_center,
                       const T          & scene_radius)
{
    // set the scene center at the WORLD origin
    // set the camera outside of the scene radius along WORLD's -Z
    model = mat<4,4,T>::TRANS(-scene_center);
    view  = mat<4,4,T>::TRANS(-mat<3,1,T>(0,0,2*scene_radius));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_MV(const mat<3,1,T> & eye,
                       const mat<3,1,T> & center,
                       const mat<3,1,T> & up)
{
    // this is equivalent to gluLookAt()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

    mat<3,1,T> z_axis = center - eye;            z_axis.normalize();
    mat<3,1,T> x_axis = z_axis.cross(up);        x_axis.normalize();
    mat<3,1,T> y_axis = x_axis.cross(z_axis);    y_axis.normalize();

    model = mat4d({ x_axis[0], y_axis[0], -z_axis[0], 0,
                    x_axis[1], y_axis[1], -z_axis[1], 0,
                    x_axis[2], y_axis[2], -z_axis[2], 0,
                       0    ,     0    ,      0    ,  1});

    view = mat4d::TRANS(-eye);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_PR_ortho(const T & l, // left
                             const T & r, // right
                             const T & b, // bottom
                             const T & t, // top
                             const T & n, // near
                             const T & f) // far
{
    // this is equivalent to glOrtho()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml

    projection = mat4d({2/(r-l),   0,        0,       -(r+l)/(r-l),
                           0,     2/(t-b),   0,       -(t+b)/(t-b),
                           0,      0,      -2/(f-n),  -(f+n)/(f-n),
                           0,      0,        0,             1   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_PR_persp(const T & l, // left
                             const T & r, // right
                             const T & b, // bottom
                             const T & t, // top
                             const T & n, // near
                             const T & f) // far
{
    // this is equivalent to glFrustum()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml

    projection = mat4d({2*n/(r-l),     0,        (r+l)/(r-l),         0,
                           0,       2*n/(t-b),   (t+b)/(t-b),         0,
                           0,          0,       -(f+n)/(f-n), -(2*f*n)/(f-n),
                           0,          0,            -1,              0   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_PR_persp(const T & fovY, // vertical field of view (degrees)
                             const T & ar,   // aspect ratio
                             const T & n,    // near
                             const T & f)    // far
{
    // this is equivalent to gluPerspective()
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

    float  tangent = tanf(to_rad(fovY/2.f));
    float  height  = n * tangent;
    float  width   = height * ar;
    return set_PR_persp(-width, width, -height, height, n, f);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::zoom(const T & factor)
{

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::rotate(const mat<3,1,T> & axis, const T & deg)
{
    mat<4,4,T> R;
    mat_set_rot_3d(R._mat, to_rad(deg), axis._vec);
    model = R * model;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::translate(const mat<3,1,T> & delta)
{

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
void Camera<T>::deserialize(const std::string & s)
{

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::print() const
{
    std::cout << "Camera M:\n"  << model
              << "Camera V:\n"  << view
              << "Camera MV:\n" << view * model
              << "Camera PR:\n" << projection << std::endl;
}

}
