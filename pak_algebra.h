/*
    The PAK Linear Algebra Library:

        The PAK libraries are a set of useful single header libraries written
        for C/C++.

        PAK takes heavy inspiration from the STB libraries found here:
            https://github.com/nothings/stb

        PAK Algebra is a linear algebra library tailored towards writing 3D programs
        in OpenGL or any other rendering library. It is written in C89.

        You must define PAK_ALGEBRA_IMPLEMENTATION before including this header file
        to define all of the functions, otherwise you'll just get the prototypes.

            #define PAK_ALGEBRA_IMPLEMENTATION
            #include "pak_alg.h"

        You can also define PAK_ALGEBRA_STATIC in order to define all of the functions
        as static, isolating the implementation.

    License:

                            The MIT License (MIT)

    Copyright (c) 2017 Phillip Kobylinski

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.namespace $`name`=$`namespace`
    THOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.THE SOFTWARE IS PROVIDED "AS IS"
*/

#ifndef PAK_ALGEBRA_HEADER
#define PAK_ALGEBRA_HEADER

#ifdef PAK_ALGEBRA_IMPLEMENTATION
#   include <math.h>
#   ifndef M_PI
#       define M_PI 3.1415926535
#   endif
#endif

#ifdef PAK_ALGEBRA_STATIC
#   define PAK_ALGEBRA_PREFIX static
#else
#   define PAK_ALGEBRA_PREFIX
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { float x, y;       } pak_vec2;
typedef struct { float x, y, z;    } pak_vec3;
typedef struct { float x, y, z, w; } pak_vec4;
typedef struct { float x, y, z, w; } pak_quat;

/* Matrixes are implemented as unions so that we can cast to an array */
typedef union {
    struct { pak_vec2 x, y; } m;
    float f22[2][2];
    pak_vec2 col[2];
} pak_mat2;

typedef union {
    struct { pak_vec3 x, y, z; } m;
    float f33[3][3];
    pak_vec3 col[3];
} pak_mat3;

typedef union {
    struct { pak_vec4 x, y, z, w; } m;
    float f44[4][4];
    pak_vec4 col[4];
} pak_mat4;

PAK_ALGEBRA_PREFIX pak_vec2 pak_vec2_new(float x, float y);
PAK_ALGEBRA_PREFIX pak_vec3 pak_vec3_new(float x, float y, float z);
PAK_ALGEBRA_PREFIX pak_vec4 pak_vec4_new(float x, float y, float z, float w);

/*
    Notice how the matrix creation function arguments are layed out vertically.
    This is so that creating a matrix actually looks like a matrix, for example

        pak_mat3_new(       |
            x1, x2, x3,     |
            y1, y2, y3,     |
            z1, z2, z3,     V
        );
*/

PAK_ALGEBRA_PREFIX pak_mat2 pak_mat2_new(float x1, float x2,
                                         float y1, float y2);
PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_new(float x1, float x2, float x3,
                                         float y1, float y2, float y3,
                                         float z1, float z2, float z3);
PAK_ALGEBRA_PREFIX pak_mat4 pak_mat4_new(float x1, float x2, float x3, float x4,
                                         float y1, float y2, float y3, float y4,
                                         float z1, float z2, float z3, float z4,
                                         float w1, float w2, float w3, float w4);

PAK_ALGEBRA_PREFIX void pak_vec2_add        (pak_vec2 *d, pak_vec2 *a, pak_vec2 *b);
PAK_ALGEBRA_PREFIX void pak_vec2_sub        (pak_vec2 *d, pak_vec2 *a, pak_vec2 *b);
PAK_ALGEBRA_PREFIX void pak_vec2_scale      (pak_vec2 *d, pak_vec2 *v, float s);
PAK_ALGEBRA_PREFIX void pak_vec2_add_eq     (pak_vec2 *d, pak_vec2 *v);
PAK_ALGEBRA_PREFIX void pak_vec2_sub_eq     (pak_vec2 *d, pak_vec2 *v);
PAK_ALGEBRA_PREFIX void pak_vec2_scale_eq   (pak_vec2 *d, float s);

PAK_ALGEBRA_PREFIX void pak_vec3_add        (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b);
PAK_ALGEBRA_PREFIX void pak_vec3_sub        (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b);
PAK_ALGEBRA_PREFIX void pak_vec3_scale      (pak_vec3 *d, pak_vec3 *v, float s);
PAK_ALGEBRA_PREFIX void pak_vec3_add_eq     (pak_vec3 *d, pak_vec3 *v);
PAK_ALGEBRA_PREFIX void pak_vec3_sub_eq     (pak_vec3 *d, pak_vec3 *v);
PAK_ALGEBRA_PREFIX void pak_vec3_scale_eq   (pak_vec3 *d, float s);

PAK_ALGEBRA_PREFIX void pak_vec4_add        (pak_vec4 *d, pak_vec4 *a, pak_vec4 *b);
PAK_ALGEBRA_PREFIX void pak_vec4_sub        (pak_vec4 *d, pak_vec4 *a, pak_vec4 *b);
PAK_ALGEBRA_PREFIX void pak_vec4_scale      (pak_vec4 *d, pak_vec4 *v, float s);
PAK_ALGEBRA_PREFIX void pak_vec4_add_eq     (pak_vec4 *d, pak_vec4 *v);
PAK_ALGEBRA_PREFIX void pak_vec4_sub_eq     (pak_vec4 *d, pak_vec4 *v);
PAK_ALGEBRA_PREFIX void pak_vec4_scale_eq   (pak_vec4 *d, float s);

PAK_ALGEBRA_PREFIX float pak_vec3_mag       (pak_vec3 *v);
PAK_ALGEBRA_PREFIX float pak_vec3_dist      (pak_vec3 *a, pak_vec3 *b);
PAK_ALGEBRA_PREFIX float pak_vec3_dot       (pak_vec3 *a, pak_vec3 *b);
PAK_ALGEBRA_PREFIX void  pak_vec3_cross     (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b);
PAK_ALGEBRA_PREFIX void  pak_vec3_lerp      (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b, float s);
PAK_ALGEBRA_PREFIX void  pak_vec3_norm      (pak_vec3 *d, pak_vec3 *v);
PAK_ALGEBRA_PREFIX void  pak_vec3_norm_eq   (pak_vec3 *v);
PAK_ALGEBRA_PREFIX void  pak_vec3_neg       (pak_vec3 *v);

PAK_ALGEBRA_PREFIX void  pak_vec3_transform3(pak_vec3 *v, const pak_mat3 *m);
PAK_ALGEBRA_PREFIX void  pak_vec3_rotate_x  (pak_vec3 *v, float angle);
PAK_ALGEBRA_PREFIX void  pak_vec3_rotate_y  (pak_vec3 *v, float angle);
PAK_ALGEBRA_PREFIX void  pak_vec3_rotate_z  (pak_vec3 *v, float angle);

PAK_ALGEBRA_PREFIX void     pak_mat3_identity(pak_mat3 *m);
PAK_ALGEBRA_PREFIX void     pak_mat3_mul(pak_mat3 *d, pak_mat3 *a, pak_mat3 *b);
PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_x_new(float angle);
PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_y_new(float angle);
PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_z_new(float angle);

PAK_ALGEBRA_PREFIX void pak_mat4_identity(pak_mat4 *m);
PAK_ALGEBRA_PREFIX void pak_mat4_look_at(pak_mat4 *d, pak_vec3 *eye, pak_vec3 *center, pak_vec3 *up);
PAK_ALGEBRA_PREFIX void pak_mat4_perspective(pak_mat4 *d, float fov, float aspect, float near, float far);

#ifdef PAK_ALGEBRA_IMPLEMENTATION

PAK_ALGEBRA_PREFIX pak_vec2 pak_vec2_new(float x, float y)
{
    pak_vec2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_vec3 pak_vec3_new(float x, float y, float z)
{
    pak_vec3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_vec4 pak_vec4_new(float x, float y, float z, float w)
{
    pak_vec4 tmp;
    tmp.x = x; tmp.y = y;
    tmp.z = z; tmp.w = w;
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_mat2 pak_mat2_new(float x1, float x2,
                                         float y1, float y2)
{
    pak_mat2 tmp;
    tmp.m.x = pak_vec2_new(x1, y1);
    tmp.m.y = pak_vec2_new(x2, y2);
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_new(float x1, float x2, float x3,
                                         float y1, float y2, float y3,
                                         float z1, float z2, float z3)
{
    pak_mat3 tmp;
    tmp.m.x = pak_vec3_new(x1, y1, z1);
    tmp.m.y = pak_vec3_new(x2, y2, z2);
    tmp.m.z = pak_vec3_new(x3, y3, z3);
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_mat4 pak_mat4_new(float x1, float x2, float x3, float x4,
                                         float y1, float y2, float y3, float y4,
                                         float z1, float z2, float z3, float z4,
                                         float w1, float w2, float w3, float w4)
{
    pak_mat4 tmp;
    tmp.m.x = pak_vec4_new(x1, y1, z1, w1);
    tmp.m.y = pak_vec4_new(x2, y2, z2, w2);
    tmp.m.z = pak_vec4_new(x3, y3, z3, w3);
    tmp.m.w = pak_vec4_new(x4, y4, z4, w4);
    return tmp;
}

PAK_ALGEBRA_PREFIX pak_quat pak_quat_new(float x, float y, float z, float w)
{
    pak_quat tmp;
    tmp.x = x; tmp.y = y;
    tmp.z = z; tmp.w = w;
    return tmp;
}

#define PAK_VEC2_2OP(A, B, POST)  \
    A->x = B->x POST;             \
    A->y = B->y POST;

#define PAK_VEC2_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;           \
    A->y = B->y OP C->y POST;

#define PAK_VEC3_2OP(A, B, POST) \
    A->x = B->x POST;            \
    A->y = B->y POST;            \
    A->z = B->z POST;

#define PAK_VEC3_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;           \
    A->y = B->y OP C->y POST;           \
    A->z = B->z OP C->z POST;

#define PAK_VEC4_2OP(A, B, POST)  \
    A->x = B->x POST;             \
    A->y = B->y POST;             \
    A->z = B->z POST;             \
    A->w = B->w POST;

#define PAK_VEC4_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;           \
    A->y = B->y OP C->y POST;           \
    A->z = B->z OP C->z POST;           \
    A->w = B->w OP C->w POST;

PAK_ALGEBRA_PREFIX void pak_vec2_add        (pak_vec2 *d, pak_vec2 *a, pak_vec2 *b)   { PAK_VEC2_3OP(d,a,+,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec2_sub        (pak_vec2 *d, pak_vec2 *a, pak_vec2 *b)   { PAK_VEC2_3OP(d,a,-,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec2_scale      (pak_vec2 *d, pak_vec2 *v, float s)       { PAK_VEC2_2OP(d,    v,*s) }
PAK_ALGEBRA_PREFIX void pak_vec2_add_eq     (pak_vec2 *d, pak_vec2 *v)                { PAK_VEC2_3OP(d,d,+,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec2_sub_eq     (pak_vec2 *d, pak_vec2 *v)                { PAK_VEC2_3OP(d,d,-,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec2_scale_eq   (pak_vec2 *d, float s)                    { PAK_VEC2_2OP(d,    d,*s) }

PAK_ALGEBRA_PREFIX void pak_vec3_add        (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b)   { PAK_VEC3_3OP(d,a,+,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec3_sub        (pak_vec3 *d, pak_vec3 *a, pak_vec3 *b)   { PAK_VEC3_3OP(d,a,-,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec3_scale      (pak_vec3 *d, pak_vec3 *v, float s)       { PAK_VEC3_2OP(d,    v,*s) }
PAK_ALGEBRA_PREFIX void pak_vec3_add_eq     (pak_vec3 *d, pak_vec3 *v)                { PAK_VEC3_3OP(d,d,+,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec3_sub_eq     (pak_vec3 *d, pak_vec3 *v)                { PAK_VEC3_3OP(d,d,-,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec3_scale_eq   (pak_vec3 *d, float s)                    { PAK_VEC3_2OP(d,    d,*s) }

PAK_ALGEBRA_PREFIX void pak_vec4_add        (pak_vec4 *d, pak_vec4 *a, pak_vec4 *b)   { PAK_VEC4_3OP(d,a,+,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec4_sub        (pak_vec4 *d, pak_vec4 *a, pak_vec4 *b)   { PAK_VEC4_3OP(d,a,-,b,+0) }
PAK_ALGEBRA_PREFIX void pak_vec4_scale      (pak_vec4 *d, pak_vec4 *v, float s)       { PAK_VEC4_2OP(d,    v,*s) }
PAK_ALGEBRA_PREFIX void pak_vec4_add_eq     (pak_vec4 *d, pak_vec4 *v)                { PAK_VEC4_3OP(d,d,+,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec4_sub_eq     (pak_vec4 *d, pak_vec4 *v)                { PAK_VEC4_3OP(d,d,-,v,+0) }
PAK_ALGEBRA_PREFIX void pak_vec4_scale_eq   (pak_vec4 *d, float s)                    { PAK_VEC4_2OP(d,d,    *s) }

/*
    pak_vec3
*/

PAK_ALGEBRA_PREFIX float pak_vec3_mag(pak_vec3 *v)
{
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

PAK_ALGEBRA_PREFIX float pak_vec3_dist(pak_vec3 *a, pak_vec3 *b)
{
    float x, y, z;
    x = b->x - a->x;
    y = b->y - a->y;
    z = b->z - a->z;
    return sqrt(x*x + y*y + z*z);
}

PAK_ALGEBRA_PREFIX float pak_vec3_dot(pak_vec3 *a, pak_vec3 *b)
{
    float x, y, z;
    x = a->x * b->x;
    y = a->y * b->y;
    z = a->z * b->z;
    return x + y + z;
}

PAK_ALGEBRA_PREFIX void pak_vec3_cross(pak_vec3 *d, pak_vec3 *a, pak_vec3 *b)
{
    d->x = a->y * b->z - a->z * b->y;
    d->y = a->z * b->x - a->x * b->z;
    d->z = a->x * b->y - a->y * b->x;
}

PAK_ALGEBRA_PREFIX void pak_vec3_lerp(pak_vec3 *d, pak_vec3 *a, pak_vec3 *b, float s)
{
    d->x = a->x + s * (b->x - a->x);
    d->y = a->y + s * (b->y - a->y);
    d->z = a->z + s * (b->z - a->z);
}

PAK_ALGEBRA_PREFIX void pak_vec3_norm(pak_vec3 *d, pak_vec3 *v)
{
    float mag = pak_vec3_mag(v);
    d->x = v->x/mag;
    d->y = v->y/mag;
    d->z = v->z/mag;
}

PAK_ALGEBRA_PREFIX void pak_vec3_norm_eq(pak_vec3 *v)
{
    pak_vec3_norm(v, v);
}

PAK_ALGEBRA_PREFIX void pak_vec3_neg(pak_vec3 *v)
{
    v->x *= -1;
    v->y *= -1;
    v->z *= -1;
}

PAK_ALGEBRA_PREFIX void pak_vec3_transform3(pak_vec3 *v, const pak_mat3 *m)
{
    pak_vec3 i = m->m.x;
    pak_vec3 j = m->m.y;
    pak_vec3 k = m->m.z;

    pak_vec3_scale_eq(&i, v->x);
    pak_vec3_scale_eq(&j, v->y);
    pak_vec3_scale_eq(&k, v->z);

    pak_vec3_add_eq(&i, &j);
    pak_vec3_add_eq(&i, &k);

    *v = i;
}

PAK_ALGEBRA_PREFIX void pak_vec3_rotate_x(pak_vec3 *v, float angle)
{
    pak_mat3 m = pak_mat3_rotation_x_new(angle);
    pak_vec3_transform3(v, &m);
}

PAK_ALGEBRA_PREFIX void pak_vec3_rotate_y(pak_vec3 *v, float angle)
{
    pak_mat3 m = pak_mat3_rotation_y_new(angle);
    pak_vec3_transform3(v, &m);
}

PAK_ALGEBRA_PREFIX void pak_vec3_rotate_z(pak_vec3 *v, float angle)
{
    pak_mat3 m = pak_mat3_rotation_z_new(angle);
    pak_vec3_transform3(v, &m);
}

/*
   end pak_vec3
*/

/*
   pak_mat3
*/

PAK_ALGEBRA_PREFIX void pak_mat3_identity(pak_mat3 *m)
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            m->f33[i][j] = i - j ? 0 : 1;
}

PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_x_new(float angle)
{
    float s = sin(angle), c = cos(angle);
    return pak_mat3_new(
        1, 0, 0,
        0, c,-s,
        0, s, c
    );
}

PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_y_new(float angle)
{
    float s = sin(angle), c = cos(angle);
    return pak_mat3_new(
        c, 0, s,
        0, 1, 0,
       -s, 0, c
    );
}

PAK_ALGEBRA_PREFIX pak_mat3 pak_mat3_rotation_z_new(float angle)
{
    float s = sin(angle), c = cos(angle);
    return pak_mat3_new(
        c,-s, 0,
        s, c, 0,
        0, 0, 1
    );
}

PAK_ALGEBRA_PREFIX void pak_mat3_mul(pak_mat3 *d, pak_mat3 *a, pak_mat3 *b)
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            d->f33[i][j] = a->f33[0][i] * b->f33[j][0]
                         + a->f33[1][i] * b->f33[j][1]
                         + a->f33[2][i] * b->f33[j][2];
}

/*
   end pak_mat3
*/

/*
    pak_mat4
*/

PAK_ALGEBRA_PREFIX void pak_mat4_identity(pak_mat4 *m)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            m->f44[i][j] = i - j ? 0 : 1;
}

PAK_ALGEBRA_PREFIX void pak_mat4_look_at(pak_mat4 *d, pak_vec3 *eye, pak_vec3 *center, pak_vec3 *up)
{
    pak_vec3 f, u ,s, tmp;

    pak_vec3_sub(&tmp, center, eye);
    pak_vec3_norm(&f, &tmp);

    pak_vec3_norm(&u, up);

    pak_vec3_cross(&tmp, &f, &u);
    pak_vec3_norm(&s, &tmp);

    pak_mat4_identity(d);

    d->f44[0][0] = s.x;
    d->f44[1][0] = s.y;
    d->f44[2][0] = s.z;
    d->f44[0][1] = u.x;
    d->f44[1][1] = u.y;
    d->f44[2][1] = u.z;
    d->f44[0][2] =-f.x;
    d->f44[1][2] =-f.y;
    d->f44[2][2] =-f.z;
    d->f44[3][0] =-pak_vec3_dot(&s, eye);
    d->f44[3][1] =-pak_vec3_dot(&u, eye);
    d->f44[3][2] = pak_vec3_dot(&f, eye);
}

PAK_ALGEBRA_PREFIX void pak_mat4_perspective(pak_mat4 *d, float fov, float aspect, float near, float far)
{
    float d2r = M_PI/180.0f; /* Degrees to radians */

    float y_scale = 1.0f/tan(d2r * fov/2);
    float x_scale = y_scale/aspect;

    float c3r3 = (far + near)/(near + far);     /* 3rd column, 3rd row */
    float c4r3 = 2 * far * near - (far - near);

    *d = pak_mat4_new(
        x_scale, 0,       0,    0,
        0,       y_scale, 0,    0,
        0,       0,       c3r3, c4r3,
        0,       0,       -1,   0
    );
}

/*
    end pak_mat4
*/

#endif /* PAK_ALGEBRA_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* PAK_ALGEBRA_HEADER */
