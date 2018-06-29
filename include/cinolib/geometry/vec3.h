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
#ifndef CINO_VEC3_H
#define CINO_VEC3_H

#include <cinolib/cino_inline.h>
#include <cinolib/ANSI_color_codes.h>
#include <cmath>
#include <iostream>
#include <assert.h>

namespace cinolib
{

template<typename real> class vec3
{
    private:

        union
        {
            struct
            {
                real m_x;
                real m_y;
                real m_z;
            };
            real m_v[3];
        };

    public:

        explicit vec3()
        {
            set( 0.0, 0.0, 0.0 );
        }

        explicit vec3(const real x, const real y=0.0, const real z=0.0 )
        {
            set(x, y, z);
        }

        const real *ptr() const
        {
            return m_v;
        }

        void set( const real x, const real y, const real z )
        {
            m_x = x;
            m_y = y;
            m_z = z;
        }

        real dot(const vec3<real> & in) const
        {
            return m_x * in.m_x +
                   m_y * in.m_y +
                   m_z * in.m_z;
        }

        vec3<real> cross(const vec3<real> & in) const
        {
            return vec3<real>(m_y * in.m_z - m_z * in.m_y,
                              m_z * in.m_x - m_x * in.m_z,
                              m_x * in.m_y - m_y * in.m_x);
        }

        real length_squared() const
        {
            return m_x * m_x +
                   m_y * m_y +
                   m_z * m_z;
        }

        real length() const
        {
            return sqrt(m_x * m_x +
                        m_y * m_y +
                        m_z * m_z);
        }

        real normalize()
        {
            real len = length();
            if (len == 0)
            {
                len = 1e-10;
                std::cout << ANSI_fg_color_red << "WARNING: normalization of zero length vector!" << ANSI_fg_color_default << std::endl;
            }
            m_x /= len;
            m_y /= len;
            m_z /= len;
            return len;
        }

        real &x() { return m_x; }
        real &y() { return m_y; }
        real &z() { return m_z; }

        const real &x() const { return m_x; }
        const real &y() const { return m_y; }
        const real &z() const { return m_z; }

        real &operator[](const int id)
        {
            return m_v[id];
        }

        const real &operator[](const int id) const
        {
            return m_v[id];
        }

        vec3<real> operator-() const
        {
            return vec3<real>(-m_x,
                              -m_y,
                              -m_z);
        }

        vec3<real> operator+(const vec3<real> b) const
        {
            return vec3<real>(m_x + b.m_x,
                              m_y + b.m_y,
                              m_z + b.m_z);
        }

        vec3<real> operator-(const vec3<real> b) const
        {
            return vec3<real>(m_x - b.m_x,
                              m_y - b.m_y,
                              m_z - b.m_z);
        }

        vec3<real> operator*(const real b) const
        {
            return vec3<real>(m_x * b,
                              m_y * b,
                              m_z * b);
        }


        vec3<real> operator*(const vec3<real> b) const
        {
            return vec3<real>(m_x * b.x(),
                              m_y * b.y(),
                              m_z * b.z());
        }


        vec3<real> operator/(const real b) const
        {
            return vec3<real>(m_x / b,
                              m_y / b,
                              m_z / b);
        }

        vec3<real> operator+=(const vec3<real> b)
        {
            vec3<real> tmp(m_x + b.m_x,
                           m_y + b.m_y,
                           m_z + b.m_z);
            *this=tmp;
            return tmp;
        }

        vec3<real> operator-=(const vec3<real> b)
        {
            vec3<real> tmp(m_x - b.m_x,
                           m_y - b.m_y,
                           m_z - b.m_z);
            *this = tmp;
            return tmp;
        }

        vec3<real> operator*=(const real b)
        {
            vec3<real> tmp(m_x * b,
                           m_y * b,
                           m_z * b);
            *this = tmp;
            return tmp;
        }

        vec3<real> operator/=(const real b)
        {
            vec3<real> tmp(m_x / b,
                           m_y / b,
                           m_z / b);
            *this = tmp;
            return tmp;
        }

        bool operator<(const vec3<real> in) const
        {
            for(uint i=0; i<3; ++i)
            {
                if( this->operator[](i) < in[i] ) return true;
                if( this->operator[](i) > in[i] ) return false;
            }
            return false;
        }

        double min_entry() const
        {
            if (m_x < m_y && m_x < m_z) return m_x;
            if (m_y < m_z) return m_y;
            return m_z;
        }

        double max_entry() const
        {
            if (m_x > m_y && m_x > m_z) return m_x;
            if (m_y > m_z) return m_y;
            return m_z;
        }

        vec3<real> min(const vec3<real> &in) const
        {
            return vec3<real>(std::min(x(), in.x()),
                              std::min(y(), in.y()),
                              std::min(z(), in.z()));
        }

        vec3<real> max(const vec3<real> &in) const
        {
            return vec3<real>(std::max(x(), in.x()),
                              std::max(y(), in.y()),
                              std::max(z(), in.z()));
        }

        double angle_rad(const vec3<real> &in) const
        {
            vec3<real> u = *this; u.normalize();
            vec3<real> v = in;    v.normalize();
            // ensure dot stays srictly in between 0 and 1 (included)
            double dot   = std::min( 1.0, u.dot(v));
                   dot   = std::max(-1.0, dot);
            double angle = acos(dot);
            assert(!std::isnan(angle));
            return angle;
        }

        double angle_deg(const vec3<real> &in) const
        {
            return angle_rad(in)*180.0/M_PI;
        }

        double dist(const vec3<real> &in) const
        {
            return (*this - in).length();
        }

        double dist_squared(const vec3<real> &in) const
        {
            return (*this - in).length_squared();
        }
};

template<typename real>
CINO_INLINE
vec3<real> operator*(const double &b, const vec3<real> &a )
{
    return vec3<real>(b * a.x(),
                      b * a.y(),
                      b * a.z());
}

template<typename real>
CINO_INLINE
std::ostream& operator<<(std::ostream &in, const vec3<real> &val)
{
    in << "[" << val.x() << ", " << val.y() << ", " << val.z() << "]";
    return in;
}

typedef vec3<double> vec3d;
typedef vec3<float>  vec3f;
typedef vec3<int>    vec3i;


template<typename vec3>
CINO_INLINE
void transform(vec3 & p, const double m[3][3])
{
	vec3d tmp;
	tmp[0] = m[0][0] * p.x() + m[0][1] * p.y() + m[0][2] * p.z();
	tmp[1] = m[1][0] * p.x() + m[1][1] * p.y() + m[1][2] * p.z();
	tmp[2] = m[2][0] * p.x() + m[2][1] * p.y() + m[2][2] * p.z();
	p  = tmp;
}


template<typename vec3>
CINO_INLINE
void bake_rotation_matrix(const vec3   & axis,
                          const double   angle,
                                double   m[3][3])
{
    double u    = axis.x();
    double v    = axis.y();
    double w    = axis.z();
    double rcos = cos(angle);
    double rsin = sin(angle);
    m[0][0] =      rcos + u*u*(1-rcos);
    m[1][0] =  w * rsin + v*u*(1-rcos);
    m[2][0] = -v * rsin + w*u*(1-rcos);
    m[0][1] = -w * rsin + u*v*(1-rcos);
    m[1][1] =      rcos + v*v*(1-rcos);
    m[2][1] =  u * rsin + w*v*(1-rcos);
    m[0][2] =  v * rsin + u*w*(1-rcos);
    m[1][2] = -u * rsin + v*w*(1-rcos);
    m[2][2] =      rcos + w*w*(1-rcos);
}


template<typename vec3>
CINO_INLINE
void rotate(      vec3   & p,
            const vec3   & axis,
            const double   angle)
{
    double R[3][3];
    bake_rotation_matrix(axis, angle, R);
    transform(p, R);
}


template<typename vec3>
CINO_INLINE
void center_and_rotate(      vec3   & p,
                       const vec3   & centroid,
                       const double   m[3][3])
{
	p -= centroid;
	vec3d tmp;
	tmp[0] = m[0][0] * p.x() + m[0][1] * p.y() + m[0][2] * p.z();
	tmp[1] = m[1][0] * p.x() + m[1][1] * p.y() + m[1][2] * p.z();
	tmp[2] = m[2][0] * p.x() + m[2][1] * p.y() + m[2][2] * p.z();
	p  = tmp;
	p += centroid;
}


template<typename vec3>
CINO_INLINE
double distance_to_segment(const vec3 & P,
                           const vec3 & P0,
                           const vec3 & P1)
{
	vec3d v = P1 - P0;
	vec3d w = P  - P0;

	float cos_wv = w.dot(v);
	float cos_vv = v.dot(v);

	if (cos_wv <= 0.0) // before P0
	{
		return (P-P0).length();
	}

	if (cos_vv <= cos_wv) // after P1
	{
		return (P-P1).length();
	}

	float b  = cos_wv / cos_vv;
	vec3d Pb = P0 + v*b;
	return (P-Pb).length();
}

}

#endif // CINO_VEC3_H
