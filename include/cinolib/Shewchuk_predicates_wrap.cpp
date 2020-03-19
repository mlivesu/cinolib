#include <cinolib/Shewchuk_predicates_wrap.h>

namespace cinolib
{

CINO_INLINE
double orient2d(const vec2d & pa, const vec2d & pb, const vec2d & pc)
{
    return orient2d(pa.ptr(), pb.ptr(), pc.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double orient2dfast(const vec2d & pa, const vec2d & pb, const vec2d & pc)
{
    return orient2dfast(pa.ptr(), pb.ptr(), pc.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double orient3d(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd)
{
    return orient3d(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double orient3dfast(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd)
{
    return orient3dfast(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


CINO_INLINE
double incircle(const vec2d & pa, const vec2d & pb, const vec2d & pc, const vec2d & pd)
{
    return incircle(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double incirclefast(const vec2d & pa, const vec2d & pb, const vec2d & pc, const vec2d & pd)
{
    return incirclefast(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double insphere(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd, const vec3d & pe)
{
    return insphere(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr(), pe.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double inspherefast(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd, const vec3d & pe)
{
    return inspherefast(pa.ptr(), pb.ptr(), pc.ptr(), pd.ptr(), pe.ptr());
}

}

