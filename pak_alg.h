/*
    The PAK Linear Algebra Library:

        The PAK libraries are a set of useful single header libraries written
        for C/C++.

        PAK takes heavy inspiration from the STB libraries found here:
            https://github.com/nothings/stb

        You must define PAK_ALG_IMPLEMENTATION before including this header file
        to define all of the functions, otherwise you'll just get the prototypes.

            #define PAK_ALG_IMPLEMENTATION
            #include "pak_alg.h"

        Similarly to STB, this library is highly tailored to the needs of it's
        author, so this library's usefulness may vary.

        PAK Algebra is a linear algebra library tailored towards writing 3D programs
        in OpenGL or any other rendering library.

        Please note that this library does not bother namespacing it's functions
        with a prefix, due to the fact that it is strange using two linear algebra
        libraries at the same time.

        You can also define PAK_ALG_STATIC in order to define all of the functions
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

#ifndef PAK_ALG_HEADER
#define PAK_ALG_HEADER

#ifdef PAK_ALG_IMPLEMENTATION
#   include <stdio.h>
#   include <math.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PAK_ALG_STATIC
#   define PAK_ALG_PREFIX static
#else
#   define PAK_ALG_PREFIX
#endif

typedef struct vec2 { float x, y;       } vec2;
typedef struct vec3 { float x, y, z;    } vec3;
typedef struct vec4 { float x, y, z, w; } vec4;
typedef struct mat2 { vec2 m[2];        } mat2;
typedef struct mat3 { vec3 m[3];        } mat3;
typedef struct mat4 { vec4 m[4];        } mat4;
typedef struct quat { float x, y, z, w; } quat;

typedef float mat2_f[2][2];
typedef float mat3_f[3][3];
typedef float mat4_f[4][4];

PAK_ALG_PREFIX void vec3_add(vec3 *d, vec3 *a, vec3 *b);
PAK_ALG_PREFIX void vec3_sub(vec3 *d, vec3 *a, vec3 *b);
PAK_ALG_PREFIX void vec3_scale(vec3 *d, vec3 *v, float s);
PAK_ALG_PREFIX void vec3_norm(vec3 *d, vec3 *v);
PAK_ALG_PREFIX void vec3_lerp(vec3 *d, vec3 *a, vec3 *b, float s);
PAK_ALG_PREFIX void vec3_cross(vec3 *d, vec3 *a, vec3 *b);
PAK_ALG_PREFIX float vec3_mag(vec3 *v);
PAK_ALG_PREFIX float vec3_dist(vec3 *a, vec3 *b);
PAK_ALG_PREFIX float vec3_dot(vec3 *a, vec3 *b);
PAK_ALG_PREFIX void vec3_print(vec3 *v);

/*

   Beginning of implementation

*/

#ifdef PAK_ALG_IMPLEMENTATION

#define VEC2_2OP(A, B, POST)  \
    A->x = B->x POST;         \
    A->y = B->y POST;

#define VEC2_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;       \
    A->y = B->y OP C->y POST;

#define VEC3_2OP(A, B, POST)  \
    A->x = B->x POST;         \
    A->y = B->y POST;         \
    A->z = B->z POST;

#define VEC3_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;       \
    A->y = B->y OP C->y POST;       \
    A->z = B->z OP C->z POST;

#define VEC4_2OP(A, B, POST)  \
    A->x = B->x POST;         \
    A->y = B->y POST;         \
    A->z = B->z POST;         \
    A->w = B->w POST;

#define VEC4_3OP(A, B, OP, C, POST) \
    A->x = B->x OP C->x POST;       \
    A->y = B->y OP C->y POST;       \
    A->z = B->z OP C->z POST;       \
    A->w = B->w OP C->w POST;

PAK_ALG_PREFIX void vec3_add(vec3 *d, vec3 *a, vec3 *b)   { VEC3_3OP(d,a,+,b,+0) }
PAK_ALG_PREFIX void vec3_sub(vec3 *d, vec3 *a, vec3 *b)   { VEC3_3OP(d,a,-,b,+0) }
PAK_ALG_PREFIX void vec3_scale(vec3 *d, vec3 *v, float s) { VEC3_2OP(d,    v,*s) }

// Determine the magnitude (length) of a vec3
PAK_ALG_PREFIX float vec3_mag(vec3 *v)
{
    // a^2 + b^2 = c^2
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

// Determine the distance between two vec3's
PAK_ALG_PREFIX float vec3_dist(vec3 *a, vec3 *b)
{
    // d = sqrt((x1 - x0)^2 + (y1 - y0)^2)
    float x, y, z;
    x = b->x - a->x;
    y = b->y - a->y;
    z = b->z - a->z;
    return sqrt(x*x + y*y + z*z);
}

// Determine the dot product of a vec3 (A * B)
PAK_ALG_PREFIX float vec3_dot(vec3 *a, vec3 *b)
{
    float x, y, z;
    x = a->x * b->x;
    y = a->y * b->y;
    z = a->z * b->z;
    return x + y + z;
}

// Determine the cross product of two vec3's (A x B)
PAK_ALG_PREFIX void vec3_cross(vec3 *d, vec3 *a, vec3 *b)
{
    d->x = a->y * b->z - a->z * b->y;
    d->y = a->z * b->x - a->x * b->z;
    d->z = a->x * b->y - a->y * b->x;
}

// Interpolate a vec3
PAK_ALG_PREFIX void vec3_lerp(vec3 *d, vec3 *a, vec3 *b, float s)
{
    d->x = a->x + s * (b->x - a->x);
    d->y = a->y + s * (b->y - a->y);
    d->z = a->z + s * (b->z - a->z);
}

// Determine the unit vector of a vec3
PAK_ALG_PREFIX void vec3_norm(vec3 *d, vec3 *v)
{
    float mag = vec3_mag(v);
    d->x = v->x/mag;
    d->y = v->y/mag;
    d->z = v->z/mag;
}

// Negate a vec3 (flip it in the opposite direction)
PAK_ALG_PREFIX void vec3_neg(vec3 *v)
{
    v->x *= -1;
    v->y *= -1;
    v->z *= -1;
}

// Prints out a vec3, snprintf would be better here, but that is not
// cross-platform notice that the newline is left out.
PAK_ALG_PREFIX void vec3_print(vec3 *v)
{
    printf("[%f %f %f]", v->x, v->y, v->z);
}

#if 0
PAK_ALG_PREFIX void mat4_identity(mat4 *p)
{
    mat4_f m = (mat4_f *) p;
    m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
    m[3][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
    m[4][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
#endif

#endif // PAK_ALG_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // PAK_ALG_HEADER
