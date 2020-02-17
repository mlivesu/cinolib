/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
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
#ifndef CINO_VEC2_H
#define CINO_VEC2_H

#include <cmath>
#include <iostream>
#include <assert.h>
#include <cinolib/pi.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

enum
{
    DROP_X = 0,
    DROP_Y = 1,
    DROP_Z = 2
};

template<typename real> class vec2
{
    private:

        union
        {
            struct
            {
                real m_x;
                real m_y;
            };
            real m_v[2];
        };

    public:

        explicit vec2()
        {
            set( 0.0, 0.0 );
        }

        explicit vec2(const real x, const real y=0.0)
        {
            set(x, y);
        }

        explicit vec2(const vec3<real> & p, const int mode = DROP_Z)
        {
            switch (mode)
            {
                case DROP_Z : set(p.x(), p.y()); break;
                case DROP_Y : set(p.x(), p.z()); break;
                case DROP_X : set(p.y(), p.z()); break;
                default : assert(false);
            }
        }

        inline const real *ptr() const
        {
            return m_v;
        }

        inline void set( const real x, const real y)
        {
            m_x = x;
            m_y = y;
        }

        inline real dot(const vec2<real> & in) const
        {
            return m_x * in.m_x +
                   m_y * in.m_y;
        }

        //inline vec2<real> cross(const vec2<real> & in) const
        //{
        //    return vec2<real>( m_x * in.m_y,
        //                      -m_y * in.m_x);
        //}

        inline real length_squared() const
        {
            return m_x * m_x +
                   m_y * m_y;
        }

        inline real length() const
        {
            return sqrt(m_x * m_x +
                        m_y * m_y);
        }

        inline real normalize()
        {
            real len = std::max(length(), 1e-5);
            m_x /= len;
            m_y /= len;
            return len;
        }

        inline real &x() { return m_x; }
        inline real &y() { return m_y; }

        inline const real &x() const { return m_x; }
        inline const real &y() const { return m_y; }

        inline real &operator[](const int id)
        {
            return m_v[id];
        }

        inline const real &operator[](const int id) const
        {
            return m_v[id];
        }

        inline vec2<real> operator-() const
        {
            return vec2<real>(-m_x, -m_y);
        }

        inline vec2<real> operator+(const vec2<real> b) const
        {
            return vec2<real>(m_x + b.m_x,
                              m_y + b.m_y);
        }

        inline vec2<real> operator-(const vec2<real> b) const
        {
            return vec2<real>(m_x - b.m_x,
                              m_y - b.m_y);
        }

        inline vec2<real> operator*(const real b) const
        {
            return vec2<real>(m_x * b,
                              m_y * b);
        }


        inline vec2<real> operator*(const vec2<real> b) const
        {
            return vec2<real>(m_x * b.x(),
                              m_y * b.y());
        }


        inline vec2<real> operator/(const real b) const
        {
            return vec2<real>(m_x / b,
                              m_y / b);
        }

        inline vec2<real> operator+=(const vec2<real> b)
        {
            vec2<real> tmp(m_x + b.m_x,
                           m_y + b.m_y);
            *this=tmp;
            return tmp;
        }

        inline vec2<real> operator-=(const vec2<real> b)
        {
            vec2<real> tmp(m_x - b.m_x,
                           m_y - b.m_y);
            *this = tmp;
            return tmp;
        }

        inline vec2<real> operator*=(const real b)
        {
            vec2<real> tmp(m_x * b,
                           m_y * b);
            *this = tmp;
            return tmp;
        }

        inline vec2<real> operator/=(const real b)
        {
            vec2<real> tmp(m_x / b,
                           m_y / b);
            *this = tmp;
            return tmp;
        }

        bool operator==(const vec2<real> & b) const
        {
            if(m_x==b.x() && m_y==b.y()) return true;
            return false;
        }

        inline bool operator<(const vec2<real> in) const
        {
            for(uint i=0; i<2; ++i)
            {
                if( this->operator[](i) < in[i] ) return true;
                if( this->operator[](i) > in[i] ) return false;
            }
            return false;
        }

        inline double min_entry() const
        {
            if (m_x < m_y) return m_x;
            return m_y;
        }

        inline double max_entry() const
        {
            if (m_x > m_y) return m_x;
            return m_y;
        }

        inline vec2<real> min(const vec2<real> &in) const
        {
            return vec2<real>(std::min(x(), in.x()),
                              std::min(y(), in.y()));
        }

        inline vec2<real> max(const vec2<real> &in) const
        {
            return vec2<real>(std::max(x(), in.x()),
                              std::max(y(), in.y()));
        }

        double angle_rad(const vec2<real> &in) const
        {
            vec2<real> u = *this; u.normalize();
            vec2<real> v = in;    v.normalize();
            double angle = acos(u.dot(v));
            assert(!std::isnan(angle));
            return angle;
        }

        double angle_deg(const vec2<real> &in) const
        {
            return angle_rad(in)*180.0/M_PI;
        }

        double dist(const vec2<real> &in) const
        {
            return (*this - in).length();
        }
};

template<typename real>
CINO_INLINE
vec2<real> operator*(const double &b, const vec2<real> &a )
{
    return vec2<real>(b * a.x(), b * a.y());
}

template<typename real>
CINO_INLINE
std::ostream& operator<<(std::ostream &in, const vec2<real> &val)
{
    in << "[" << val.x() << ", " << val.y() << "]";
    return in;
}

typedef vec2<double> vec2d;
typedef vec2<float>  vec2f;
typedef vec2<int>    vec2i;

}

#endif // CINO_VEC2_H
