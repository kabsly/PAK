/*
    The PAK Library Collection:

        The PAK libraries are a set of useful single header libraries written
        for C/C++.

        PAK takes heavy inspiration from the STB libraries found here:
            https://github.com/nothings/stb

        Similarly to STB, this library is highly tailored to the needs of it's
        author, so this library's usefulness may vary.

        Here is a list of the libraries found in PAK:

            - PAK Vectors, generic dynamic array library (redundant in C++)

        Libraries can be disabled, for example just define PAK_NO_VEC in order
        to disable PAK Vectors from being used.

        You must define PAK_IMPLEMENTATION before including this header file to define
        all of the functions, otherwise you'll just get the prototypes.

            #define PAK_IMPLEMENTATION
            #include "pak.h"

        It is also important to know how PAK achieves type-safety in order to
        use the library effectively. PAK uses C macros in order to improvise
        C++ style templates. Consider the following code:

            // Define a vector type called "IntVector" which holds int values
            PAK_INIT_VEC(IntVector, int, NULL);

            int main()
            {
                IntVector vec = IntVector_new(1024);

                int i;
                for (i = 0; i < 2048; i++)
                    IntVector_push(&vec, i);
            }

        This is a dirty solution, but it is the only way to accomplish generics in C.

        Also, in order to provide debuggablity, the initialization macros
        work by wrapping around generic void* functions. So if the library
        needs to be debugged, simply use the void* functions to do so.


        All calls to malloc, realloc, free, etc are wrapped around macros, so
        they can easily be redefined by the user, just define PAK_NO_MALLOC,
        and redefine the following:
            
            pak_malloc   malloc
            pak_calloc   calloc
            pak_realloc  realloc
            pak_free     free

        The following can also be redefined, but they are not used in PAK as of
        writing this:

            pak_reallocf reallocf
            pak_valloc   valloc

        WARNING! If you define PAK_NO_MALLOC, please be extra careful that you
        redefine ALL of the wrapper macros. This is because PAK uses typecasts
        in order to provide support for C++, so depending on your compiler, you
        may get references to undefined functions without warning!


        You can also define PAK_STATIC in order to define all of the functions
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

#ifndef PAK_HEADER
#define PAK_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PAK_STATIC
#   define PAK_PREFIX static
#else
#   define PAK_PREFIX
#endif

#ifdef PAK_VERBOSE
#   include <stdio.h> // fprintf
#   define pak_assert(C) if (!(C)) { fprintf(stderr, "PAK Assertion fail (%s:%d).\n",\
        __FILE__, __LINE__); goto fail; }
#   define pak_debug(M, ...) fprintf(stderr, "DEBUG (%s:%s:%d): " M "\n",\
        __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#   define pak_assert(C) if (!(C)) goto fail;
#   define pak_debug(M, ...) ((void) 0)
#endif

// Malloc wrapper
#if defined PAK_IMPLEMENTATION && !defined PAK_NO_MALLOC
#   include <stdlib.h>
#   define pak_malloc   malloc
#   define pak_calloc   calloc
#   define pak_realloc  realloc
#   define pak_reallocf reallocf
#   define pak_valloc   valloc
#   define pak_free     free
#endif

#ifdef PAK_IMPLEMENTATION
#   include <string.h> // memcpy
#else
#   include <stddef.h> // size_t
#endif

/*
    The PAK Vector library

    There are two types of vectors, "raw" ones that use void pointer, which is
    better suited for small projects, and "typesafe" versions which are made
    using the PAK_INIT_VEC macro behaving like a C++ template.

    Features:
        - Optional type-safety
        - Optional garbage collection
        - Ability to index like a normal C array (e.g. "array[0]")

    Notes:
        It is not recommended that you use this section of PAK in C++, due to the
        fact that it is redundant to the STL.

    Example:

        int *vec = pak_vec_new(int, 1024);
        assert(vec);

        int i;
        for (i = 0; i < 1000; i++)
            pak_vec_push(&vec, i);

        for (i = 0; i < 1000; i++)
            pak_vec_pop(&vec);
*/

#ifndef PAK_NO_VEC

// Function pointer to a free elements when doing resizes, pops, etc
// (I call it "garbage collection", because using "garbage collection" and "C"
// in the same sentence gets people's attention.)
typedef void (*pak__vec_gc)(void *);

// A unique signature kept inside of the vector metadata
#ifndef PAK_VEC_SIGNATURE
#   define PAK_VEC_SIGNATURE 0x5F3C2A
#endif

// Header which contains the vector metadata
typedef struct {
    int count;
    int max;
    int rate;
    size_t elem_sz;
    unsigned int sig;
    pak__vec_gc gc;
} pak__vec;

// The header is always the first element at the array, but we increment the
// pointer in order to hide it from the user
#define pak_vec_header(V)   (((pak__vec *) (V) - 1))

// Utility macros
#define pak_vec_count(V)        (pak_vec_header((V))->count)
#define pak_vec_max(V)          (pak_vec_header((V))->max)
#define pak_vec_elem_sz(V)      (pak_vec_header((V))->elem_sz)
#define pak_vec_isvalid(V)      (pak_vec_header((V))->sig == PAK_VEC_SIGNATURE)
#define pak_vec_foreach(I, V)   for (I = 0; I < pak_vec_count((V)); i++)
#define pak_vec_last(V)         V[pak_vec_count((V)) - 1]

// Get values from the vector, without knowing the type, mostly for internal
// use, but we'll make it available to the user
#define pak_vec_notype_get(V, I)    (void *)((char *) (V) + ((I) * pak_vec_elem_sz((V))))
#define pak_vec_notype_last(V)      pak_vec_notype_get((V), pak_vec_count((V)) - 1)

PAK_PREFIX void *pak__vec_new(size_t sz, int max);
#define pak_vec_new(T, M) (T *) pak__vec_new(sizeof(T), (M))

PAK_PREFIX void *pak__vec_new_gc(size_t sz, int max, pak__vec_gc gc);
#define pak_vec_new_gc(T, M, F) (T *) pak__vec_new_gc(sizeof(T), (M), (F))

PAK_PREFIX void pak__vec_free(void **pp);
#define pak_vec_free(PP) pak__vec_free((void **) (PP))

PAK_PREFIX int pak__vec_resize(void **pp, int max);
#define pak_vec_resize(PP, M) pak__vec_resize((void **) (PP), (M))

PAK_PREFIX int pak__vec_expand(void **pp);
#define pak_vec_expand(PP) pak__vec_expand((void **) (PP))

PAK_PREFIX int pak__vec_contract(void **pp);
#define pak_vec_contract(PP) pak__vec_contract((void **) (PP))

PAK_PREFIX int pak__vec_push(void **pp, void *e);
#define pak_vec_push(V, E) pak__vec_push((void **) (V), (void *) &(E))

PAK_PREFIX int pak__vec_pop(void **pp);
#define pak_vec_pop(PP) pak__vec_pop((void **) (PP))

// Create typesafe wrapper functions for the vector implementation,
// using a "ghetto" C++ style template.
#define PAK_INIT_VEC(NAME, TYPE, GC)                                                                \
    typedef TYPE* NAME;                                                                             \
                                                                                                    \
    PAK_PREFIX int NAME##_count(NAME vec)           { return pak_vec_count(vec); }                  \
    PAK_PREFIX int NAME##_max(NAME vec)             { return pak_vec_max(vec); }                    \
    PAK_PREFIX size_t NAME##_elem_sz(NAME vec)      { return pak_vec_elem_sz(vec); }                \
    PAK_PREFIX int NAME##_isvalid(NAME vec)         { return pak_vec_isvalid(vec); }                \
    PAK_PREFIX NAME NAME##_new(int max)             { return pak_vec_new_gc(TYPE, max, GC); }       \
    PAK_PREFIX void NAME##_free(NAME *pp)           { pak_vec_free(pp); }                           \
    PAK_PREFIX int NAME##_resize(NAME *pp, int max) { return pak_vec_resize(pp, max); }             \
    PAK_PREFIX int NAME##_expand(NAME *pp)          { return pak_vec_expand(pp); }                  \
    PAK_PREFIX int NAME##_contract(NAME *pp)        { return pak_vec_contract(pp); }                \
    PAK_PREFIX int NAME##_push(NAME *pp, TYPE val)  { return pak_vec_push(pp, val); }               \
    PAK_PREFIX int NAME##_pop(NAME *pp)             { return pak_vec_pop(pp); }

// For header files
#define PAK_INIT_VEC_PROTOTYPES(NAME, TYPE)         \
    typedef TYPE* NAME;                             \
                                                    \
    extern int NAME##_count(NAME vec);              \
    extern int NAME##_max(NAME vec);                \
    extern size_t NAME##_elem_sz(NAME vec);         \
    extern int NAME##_isvalid(NAME vec);            \
    extern NAME NAME##_new(int max);                \
    extern void NAME##_free(NAME *pp);              \
    extern int NAME##_resize(NAME *pp, int max);    \
    extern int NAME##_expand(NAME *pp);             \
    extern int NAME##_contract(NAME *pp);           \
    extern int NAME##_push(NAME *pp, TYPE val);     \
    extern int NAME##_pop(NAME *pp);

// Begin function definitions
#ifdef PAK_IMPLEMENTATION

PAK_PREFIX void *pak__vec_new(size_t sz, int max)
{
    pak_assert(max > 0);

    pak__vec *vec = (pak__vec *)pak_malloc(sizeof(*vec) + (sz * max));
    pak_assert(vec);

    vec++;

    pak__vec *h = pak_vec_header(vec);

    h->count = 0;
    h->max = max;
    h->rate = max;
    h->elem_sz = sz;
    h->sig = PAK_VEC_SIGNATURE;
    h->gc = NULL;

    // The "zu" sequence is not supported on windows
#ifndef _WIN32
    pak_debug("Created vector %p (Max: %d, Element size: %zu).", h, max, sz);
#endif

    return vec;

fail:
    return NULL;
}

PAK_PREFIX void *pak__vec_new_gc(size_t sz, int max, pak__vec_gc gc)
{
    void *vec = pak__vec_new(sz, max);
    pak_assert(vec);

    pak_vec_header(vec)->gc = gc;

    return vec;

fail:
    return NULL;
}

PAK_PREFIX void pak__vec_free(void **pp)
{
    void *vec = *pp;
    pak_assert(vec); // Double free?

    pak__vec *h = pak_vec_header(vec);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    while(h->gc && h->count > 0) {
        h->gc(pak_vec_notype_last(vec));
        h->count--;
    }

    pak_debug("Free'd PAK Vector %p.", h);

    free(h);
    *pp = NULL;

fail:
    return;
}

PAK_PREFIX int pak__vec_resize(void **pp, int max)
{
    pak_assert(max > 0);

    pak__vec *h = pak_vec_header(*pp);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    pak_debug("Resizing vector %p (%d -> %d).", h, h->max, max);

    if (max < h->count) {

        // Collect the garbage

        int i = 0;
        while (h->gc && --h->count > max) {
            h->gc(pak_vec_notype_last(*pp));
            i++;
        }

        pak_debug("Called garbage collector on vector %p, %d times.", h, i);

        h->count = max;
    }

    h->max = max;

    pak__vec *vec = (pak__vec *)pak_realloc(h, sizeof(*vec) + (h->elem_sz * max));
    pak_assert(vec);

    vec++;

    *pp = vec;

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__vec_expand(void **pp)
{
    pak__vec *h = pak_vec_header(*pp);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    return pak__vec_resize(pp, h->max + h->rate);

fail:
    return -1;
}

PAK_PREFIX int pak__vec_contract(void **pp)
{
    pak__vec *h = pak_vec_header(*pp);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    return pak__vec_resize(pp, h->max - h->rate);

fail:
    return -1;
}

PAK_PREFIX int pak__vec_push(void **pp, void *e)
{
    pak__vec *h = pak_vec_header(*pp);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    if (h->count >= h->max) {
        pak_assert(pak_vec_expand(pp) == 0);
        h = pak_vec_header(*pp);
    }

    h->count++;

    memcpy(pak_vec_notype_last(*pp), e, h->elem_sz);

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__vec_pop(void **pp)
{
    pak__vec *h = pak_vec_header(*pp);
    pak_assert(h->sig == PAK_VEC_SIGNATURE);

    if (h->count > 0) {
        if (h->gc)
            h->gc(pak_vec_notype_last(*pp));

        h->count--;

        if (h->count < h->max - h->rate) {
            pak__vec_contract(pp);
            h = pak_vec_header(*pp);
        }
    }

    return 0;
        
fail:
    return -1;
}

#endif // PAK_IMPLEMENTATION
#endif // PAK_NO_VEC

/*
   End of PAK Vector implementation.
*/

#ifdef __cplusplus
}
#endif

#endif // PAK_HEADER
