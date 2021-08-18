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
Camera<T>::Camera(const mat<3,1,T> & scene_center,
                  const T          & scene_radius,
                  const T          & asp_ratio)
{
    fit_scene(scene_center, scene_radius);
    set_persp_frustum(90, asp_ratio, -scene_radius, scene_radius);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
Camera<T>::Camera(const mat<3,1,T> & eye,
                  const mat<3,1,T> & center,
                  const mat<3,1,T> & up,
                  const T          & asp_ratio)
{
    look_at(eye, center, up);

    auto radius = eye.dist(center);
    set_persp_frustum(90, asp_ratio, -radius, radius);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::fit_scene(const mat<3,1,T> & scene_center,
                          const T          & scene_radius)
{
    // set the scene center at the WORLD origin
    // move the camera just outside the radius of scene along -Z
    model = mat<4,4,T>::TRANS(-scene_center);
    view  = mat<4,4,T>::TRANS(-mat<3,1,T>(0,0,scene_radius));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::look_at(const mat<3,1,T> & eye,
                        const mat<3,1,T> & center,
                        const mat<3,1,T> & up)
{
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
void Camera<T>::set_ortho_frustum(const T & l, // left
                                  const T & r, // right
                                  const T & b, // bottom
                                  const T & t, // top
                                  const T & n, // near
                                  const T & f) // far
{
    projection = mat4d({2/(r-l),   0,        0,       -(r+l)/(r-l),
                           0,     2/(t-b),   0,       -(t+b)/(t-b),
                           0,      0,      -2/(f-n),  -(f+n)/(f-n),
                           0,      0,        0,             1   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_persp_frustum(const T & l, // left
                                  const T & r, // right
                                  const T & b, // bottom
                                  const T & t, // top
                                  const T & n, // near
                                  const T & f) // far
{
    projection = mat4d({2*n/(r-l),     0,        (r+l)/(r-l),         0,
                           0,       2*n/(t-b),   (t+b)/(t-b),         0,
                           0,          0,       -(f+n)/(f-n), -(2*f*n)/(f-n),
                           0,          0,            -1,              0   });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Camera<T>::set_persp_frustum(const T & fovY, // vertical field of view (degrees)
                                  const T & ar,   // aspect ratio
                                  const T & n,    // near
                                  const T & f)    // far
{
    float  tangent = tanf(to_rad(fovY/2.f));
    float  height  = n * tangent;
    float  width   = height * ar;
    return set_persp_frustum(-width, width, -height, height, n, f);
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
