/*
    The PAK Library Collection:

        The PAK libraries are a set of useful single header libraries written
        for C/C++.

        PAK takes heavy inspiration from the STB libraries found here:
            https://github.com/nothings/stb

        Similarly to STB, this library is highly tailored to the needs of it's
        author, so this library's usefulness may vary.

        You must define PAK_IMPLEMENTATION before including this header file to define
        all of the functions, otherwise you'll just get the prototypes.

            #define PAK_IMPLEMENTATION
            #include "pak.h"

        You can also define PAK_STATIC in order to define all of the functions
        as static, isolating the implementation.

        Here is a list of the libraries in this file:

            - PAK Math, utilities for doing things with numbers
            - PAK Lists, generic linked list library
            - PAK Arrays, generic dynamic array library
            - PAK I/O, file input and output library

        More are always on the way.

    Disabling Libraries:

        Libraries can be disabled with defines before including this file

            #define PAK_NO_MATH // Disable math library
            #define PAK_NO_LIST // Disable linked list library
            #define PAK_NO_ARR  // Disable dynamic array library
            #define PAK_NO_IO   // Disable I/O library

        Some of the libraries rely on each other, so if there is ever a conflict where
        a dependency is disabled, a compile-time error will be thrown via #error.

    Type Safety:

        It is also important to know how PAK achieves type-safety in order to
        use the library effectively. PAK uses C macros in order to improvise
        C++ style templates. Consider the following code:

            // Define a array type called "int_array" which holds int values
            PAK_INIT_ARR(int_array, int, NULL);

            int main()
            {
                int i;
                int_array arr = int_array_new(1024);

                for (i = 0; i < 2048; i++)
                    int_array_push(&arr, i);
            }

        This is a dirty solution, but it is the only way to accomplish generics in C.

        Also, in order to provide debuggablity, the initialization macros
        work by wrapping around generic void* functions. So if the library
        needs to be debugged, simply use the void* functions to do so.

    Error handling:

        PAK uses return codes to signal an error. Usually 0 means success and -1
        means failure, check the actual function definition if you are not sure.
        
        If you would like PAK to explicitly print out assertion fails, then define
        PAK_VERBOSE before including the file.

        The author of this library makes no guarantee that the library is secure,
        so please use at your own risk.

    Malloc Wrapper:

        All calls to malloc, realloc, free, etc are wrapped around macros, so
        they can easily be redefined by the user, just define PAK_NO_MALLOC,
        and redefine the following:
            
            pak_malloc   malloc
            pak_calloc   calloc
            pak_realloc  realloc
            pak_free     free

        WARNING! If you define PAK_NO_MALLOC, please be extra careful that you
        redefine ALL of the wrapper macros. This is because PAK uses typecasts
        in order to provide support for C++, so depending on your compiler, you
        may get references to undefined functions without warning!

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
#   define pak_assert(C) if (!(C)) { fprintf(stderr, "PAK Assertion fail (%s:%s:%d) %s.\n",\
        __FILE__, __FUNCTION__, __LINE__, #C); goto fail; }
#else
#   define pak_assert(C) if (!(C)) goto fail;
#endif

#define pak_debug(MSG)\
    fprintf(stderr, "PAK Debug (%s:%s:%d): " MSG "\n", __FILE__, __FUNCTION__, __LINE__);

#define pak_static_assert(EXPR, MSG) \
    ((void)sizeof(char[(EXPR && MSG) ? 1 : -1]))

/* Malloc wrapper */
#if defined(PAK_IMPLEMENTATION) && !defined(PAK_NO_MALLOC)
#   include <stdlib.h>
#   define pak_malloc(S)     malloc(S)
#   define pak_calloc(I, S)  calloc(I, S)
#   define pak_realloc(P, S) realloc(P, S)
#   define pak_free(P)       free(P)
#elif defined(PAK_IMPLEMENTATION)
#   ifndef pak_malloc
#       error "PAK Error: Please redefine pak_malloc."
#   endif
#   ifndef pak_calloc
#       error "PAK Error: Please redefine pak_calloc."
#   endif
#   ifndef pak_realloc
#       error "PAK Error: Please redefine pak_realloc."
#   endif
#   ifndef pak_free
#       error "PAK Error: Please redefine pak_free."
#   endif
#endif

#ifdef PAK_IMPLEMENTATION
#   include <stdio.h>
#   include <string.h> /* memcpy */
#   include <stdarg.h>
#   include <stdint.h>
#   include <math.h>
#   ifndef M_PI
#       define M_PI 3.1415926535
#   endif
#else
#   include <stddef.h> /* size_t */
#endif

/* Common types */

typedef enum {
    PAK_FALSE = 0,
    PAK_TRUE  = 1
} pak_bool;

typedef char           pak_i8;
typedef short          pak_i16;
typedef long           pak_i32;
typedef unsigned char  pak_ui8;
typedef unsigned short pak_ui16;
typedef unsigned long  pak_ui32;

/*
    PAK Math:

    A basic math utility library featuring safe min/max functions, pseudo-random
    number generation.

    Notes:

        The random number generator featured in this library is NOT cryptographically
        secure
            
        You can find out more about the random number generation algorithm
        at http://burtleburtle.net/bob/rand/smallprng.html
*/

/* Typesafe min/max functions in order to avoid the many issues with
   the classic "#define max(a, b)" */

#ifndef PAK_NO_MATH

#define PAK_INIT_COMPARE(NAME, TYPE, OP) \
    PAK_PREFIX TYPE NAME(TYPE a, TYPE b) { return a OP b ? a : b; }

#define PAK_INIT_COMPARE_PROTOTYPE(NAME, TYPE) \
    extern TYPE NAME(TYPE a, TYPE b);

#ifdef PAK_IMPLEMENTATION
    PAK_INIT_COMPARE(pak_imax, int,     >)
    PAK_INIT_COMPARE(pak_lmax, long,    >)
    PAK_INIT_COMPARE(pak_fmax, float,   >)
    PAK_INIT_COMPARE(pak_dmax, double,  >)
    PAK_INIT_COMPARE(pak_imin, int,     <)
    PAK_INIT_COMPARE(pak_lmin, long,    <)
    PAK_INIT_COMPARE(pak_fmin, float,   <)
    PAK_INIT_COMPARE(pak_dmin, double,  <)

    PAK_INIT_COMPARE(pak_uimax, unsigned int,  >)
    PAK_INIT_COMPARE(pak_ulmax, unsigned long, >)
    PAK_INIT_COMPARE(pak_uimin, unsigned int,  <)
    PAK_INIT_COMPARE(pak_ulmin, unsigned long, <)
#else
#ifndef PAK_STATIC
    PAK_INIT_COMPARE_PROTOTYPE(pak_imax, int)
    PAK_INIT_COMPARE_PROTOTYPE(pak_lmax, long)
    PAK_INIT_COMPARE_PROTOTYPE(pak_fmax, float)
    PAK_INIT_COMPARE_PROTOTYPE(pak_dmax, double)
    PAK_INIT_COMPARE_PROTOTYPE(pak_imin, int)
    PAK_INIT_COMPARE_PROTOTYPE(pak_lmin, long)
    PAK_INIT_COMPARE_PROTOTYPE(pak_fmin, float)
    PAK_INIT_COMPARE_PROTOTYPE(pak_dmin, double)

    PAK_INIT_COMPARE_PROTOTYPE(pak_uimax, unsigned int)
    PAK_INIT_COMPARE_PROTOTYPE(pak_ulmax, unsigned long)
    PAK_INIT_COMPARE_PROTOTYPE(pak_uimin, unsigned int)
    PAK_INIT_COMPARE_PROTOTYPE(pak_ulmin, unsigned long)
#endif
#endif

/* Random number generation */
PAK_PREFIX unsigned long pak_math_rand(unsigned long seed);

/* Radians and degree operations */
PAK_PREFIX float pak_math_deg_to_rad(float deg);
PAK_PREFIX float pak_math_rad_to_deg(float rad);

#ifdef PAK_IMPLEMENTATION

/* See above documentation for info on the pseudo-random number generator */
PAK_PREFIX unsigned long pak_math_rand(unsigned long seed)
{
    unsigned long ctx[4];
    unsigned long e;

    #define pak_rot(X, K) (((X)<<(K))|((X)>>(32-(K))))
    
    ctx[0] = 0xf1ea5eed;
    ctx[1] = seed;
    ctx[2] = seed;
    ctx[3] = seed;

    e = ctx[0] - pak_rot(ctx[1], 27);
    ctx[0] = ctx[1] ^ pak_rot(ctx[2], 17);
    ctx[1] = ctx[2] + ctx[3];
    ctx[2] = ctx[3] + e;
    ctx[3] = e + ctx[0];

    #undef pak_rot
    return ctx[3];
}

PAK_PREFIX float pak_math_deg_to_rad(float deg)
{
    return (deg * M_PI)/180.0f;
}

PAK_PREFIX float pak_math_rad_to_deg(float rad)
{
    return rad * (180.0f/M_PI);
}

#endif /* PAK_IMPLEMENTATION */
#endif /* PAK_NO_MATH */
/*
    End of PAK math
*/

/*
   The PAK List library

   A simple, generic doubly linked list implementation.

   Example:

        pak_list *list = pak_list_new(int);

        int i;
        for (i = 0; i < 1000; i++)
            pak_list_push(list, i);

        // Delete elements from end
        for (i = 0; i < 100; i++)
            pak_list_pop(list);

        // Delete elements from beginning
        for (i = 0; i < 100; i++)
            pak_list_shift(list);

        pak_list_free(&list);
*/

#ifndef PAK_NO_LIST

/* Memory managment function used when we delete elements from the list */
typedef void (*pak__list_gc)(void *);

typedef struct pak_node {
    void *data;
    struct pak_node *next;
    struct pak_node *prev;
} pak_node;

typedef struct {
    int count;
    size_t elem_sz;
    pak__list_gc gc;
    pak_node *first;
    pak_node *last;
} pak_list;

/* Looping utilities */
#define pak_list_foreach(L, N)       for (N = L->first; N != NULL && N->next != NULL; N = N->next)
#define pak_list_foreach_back(L, N)  for (N = L->last ; N != NULL && N->prev != NULL; N = N->prev)
#define pak_list_data(T, N)          *(T *) (N)->data

/* Function protypes with wrapper macros */

/* List creation and destruction */
PAK_PREFIX pak_list *pak__list_new(size_t elem_sz);
#define pak_list_new(T) pak__list_new(sizeof(T))

PAK_PREFIX pak_list *pak__list_new_gc(size_t elem_sz, pak__list_gc gc);
#define pak_list_new_gc(T, GC) pak__list_new_gc(sizeof(T), GC)

PAK_PREFIX void pak_list_free(pak_list **pp);

/* Pushing and popping */
PAK_PREFIX void pak_list_pop(pak_list *list);
PAK_PREFIX int pak__list_push(pak_list *list, size_t sz, void *e);
#define pak_list_push(L, E) pak__list_push(L, sizeof(E), &(E))

/* Unshifting and shifting */
PAK_PREFIX void pak_list_shift(pak_list *list);
PAK_PREFIX int pak__list_unshift(pak_list *list, size_t sz, void *e);
#define pak_list_unshift(L, E) pak__list_unshift(L, sizeof(E), &(E))

/* Create typesafe wrapper functions around the void* ones */
#define PAK_INIT_LIST(NAME, TYPE, GC)                                                       \
    typedef pak_list* NAME;                                                                 \
    typedef pak_node* NAME##_node;                                                          \
                                                                                            \
    PAK_PREFIX NAME NAME##_new()                    { return pak_list_new_gc(TYPE, GC); }   \
    PAK_PREFIX void NAME##_free(NAME *l)            { pak_list_free(l); }                   \
    PAK_PREFIX int  NAME##_push(NAME l, TYPE e)     { return pak_list_push(l, e); }         \
    PAK_PREFIX void NAME##_pop(NAME l)              { pak_list_pop(l); }                    \
    PAK_PREFIX int  NAME##_unshift(NAME l, TYPE e)  { return pak_list_unshift(l, e); }      \
    PAK_PREFIX void NAME##_shift(NAME l)            { pak_list_shift(l); }                  \
    PAK_PREFIX TYPE NAME##_data(NAME##_node n)      { return pak_list_data(TYPE, n); }

/* For header files */
#define PAK_INIT_LIST_PROTOTYPES(NAME, TYPE)        \
    typedef pak_list *NAME;                         \
    typedef pak_node *NAME##_node;                  \
                                                    \
    extern NAME NAME##_new();                       \
    extern void NAME##_free(NAME *l);               \
    extern int  NAME##_push(NAME l, TYPE e);        \
    extern void NAME##_pop(NAME l);                 \
    extern int  NAME##_unshift(NAME *l, TYPE e);    \
    extern void NAME##_shift(NAME l);               \
    extern TYPE NAME##_data(NAME##_node *n);

/* Begin PAK list function definitions */
#ifdef PAK_IMPLEMENTATION

PAK_PREFIX pak_list *pak__list_new(size_t elem_sz)
{
    pak_list *list = (pak_list *)pak_malloc(sizeof(*list));
    pak_assert(list);

    list->count = 0;
    list->elem_sz = elem_sz;
    list->gc = NULL;
    list->first = NULL;
    list->last = NULL;

    return list;

fail:
    return NULL;
}

PAK_PREFIX pak_list *pak__list_new_gc(size_t elem_sz, pak__list_gc gc)
{
    pak_list *list = pak__list_new(elem_sz);
    pak_assert(list);

    list->gc = gc;

    return list;

fail:
    return NULL;
}

/* Clear out all of the elements and free the list */
PAK_PREFIX void pak_list_free(pak_list **pp)
{
    int i;
    pak_list *list = *pp;
    pak_assert(list); /* Double free? */

    for (i = 0; i < list->count; i++)
        pak_list_pop(list);

    pak_free(list);
    *pp = NULL;

fail:
    return;
}

/* Push value to back of the list */
PAK_PREFIX int pak__list_push(pak_list *list, size_t sz, void *p)
{
    pak_node *node = NULL;
    void *data = NULL;

    pak_assert(sz == list->elem_sz);

    node = (pak_node *)pak_malloc(sizeof(*node));
    pak_assert(node);

    node->next = NULL;
    node->prev = NULL;

    data = pak_malloc(sz);
    pak_assert(data);

    memcpy(data, p, sz);
    node->data = data;

    if (list->count == 0) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

    return 0;

fail:
    if (node)
        pak_free(node);

    if (data)
        pak_free(data);

    return -1;
}

/* Remove elements at the beginning of the list */
PAK_PREFIX void pak_list_pop(pak_list *list)
{
    pak_node *last = NULL;
    pak_node *prev = NULL;

    pak_assert(list->count > 0);
    pak_assert(list->last);

    last = list->last;
    prev = last->prev;

    if (list->gc)
        list->gc(last->data);

    pak_free(last->data);

    if (prev) {
        list->last = prev;
        prev->next = NULL;
    } else {
        list->first = NULL;
        list->last = NULL;
    }

    list->count--;

fail:
    return;
}

/* Push element to front of the list */
PAK_PREFIX int pak__list_unshift(pak_list *list, size_t sz, void *p)
{
    pak_node *node = NULL;
    void *data = NULL;

    pak_assert(sz == list->elem_sz);

    node = (pak_node *)pak_malloc(sizeof(*node));
    pak_assert(node);

    node->next = NULL;
    node->prev = NULL;

    data = pak_malloc(sz);
    pak_assert(data);

    memcpy(data, p, sz);
    node->data = data;

    if (list->count == 0) {
        list->first = node;
        list->last = node;
    } else {
        list->first->prev = node;
        node->next = list->first;
        list->first = node;
    }

    list->count++;

    return 0;

fail:
    if (node)
        pak_free(node);

    if (data)
        pak_free(data);

    return -1;
}

/* Remove element from beginning of the list */
PAK_PREFIX void pak_list_shift(pak_list *list)
{
    pak_node *first = NULL;
    pak_node *next = NULL;

    pak_assert(list->count > 0);
    pak_assert(list->first);

    first = list->first;
    next = first->next;

    if (list->gc)
        list->gc(first->data);

    pak_free(first->data);

    if (next) {
        list->first = next;
        next->prev = NULL;
    } else {
        list->first = NULL;
        list->last = NULL;
    }

    list->count--;

fail:
    return;
}

#endif /* PAK_IMPLEMENTATION */
#endif /* PAK_NO_LIST */

/*
    End of PAK List library
*/

/*
    The PAK Array library

    There are two types of arrays, "raw" ones that use void pointer, which is
    better suited for small projects, and "typesafe" versions which are made
    using the PAK_INIT_ARR macro behaving like a C++ template.

    Features:
        - Optional type-safety
        - Optional automatic memory managment
        - Ability to index like a normal C array (e.g. "array[0]")

    Notes:
        It is not recommended that you use this section of PAK in C++, due to the
        fact that it is redundant to the STL.

        Also, be extermely careful when using the "raw" functions, as they are
        not at all typesafe. Consider using PAK_INIT_ARR if this becomes a problem.

    PAK_INIT_ARR Usage:

        PAK_INIT_ARR macro takes in three arguments, first being the name of your
        data structure, the second being the type, and the third being an optional
        memory managment function pointer, for example

            PAK_INIT_ARR(intptr_array, int*, some_free_func)

        Use NULL in place of the memory managment function pointer if the data
        does not need to be free'd (which is most of the use case)

            PAK_INIT_ARR(int_array, int, NULL) // Notice the third arg is NULL

        PAK_INIT_ARR will define the following (replace <name> with the first macro parameter):

            typedef <type *> <name>;

            int     <name>_count    (<name> arr);
            int     <name>_max      (<name> arr);
            size_t  <name>_elem_sz  (<name> arr);
            int     <name>_isvalid  (<name> arr);
            <name>  <name>_new      (int max);
            void    <name>_free     (<name> *pp);
            int     <name>_resize   (<name> *pp, int max);
            int     <name>_expand   (<name> *pp);
            int     <name>_contract (<name> *pp);
            int     <name>_push     (<name> *pp, <type> val);
            int     <name>_pop      (<name> *pp);

        Notice PAK Arrays do not have getter or setter functions.
        That is because you can index the array with "[]".

    Example:

        int *arr = pak_arr_new(int, 1024);
        assert(arr);

        int i;
        for (i = 0; i < 1024; i++)
            pak_arr_push(&arr, i);

        for (i = 0; i < 1024; i++)
            pak_arr_pop(&arr);
*/

#ifndef PAK_NO_ARR

/* Function pointer to a free elements when doing resizes, pops, etc */
typedef void (*pak__arr_gc)(void *);

/* A unique signature kept inside of the array metadata */
#ifndef PAK_ARR_SIGNATURE
#   define PAK_ARR_SIGNATURE 0x5F3C2A
#endif

/* Header which contains the array metadata */
typedef struct {
    int count;
    int max;
    int rate;
    size_t elem_sz;
    unsigned int sig;
    pak__arr_gc gc;
} pak__arr;

/* The header is always the first element at the array, but we increment the
   pointer in order to hide it from the user */
#define pak_arr_header(V)   (((pak__arr *) (V) - 1))

/* Utility macros */
#define pak_arr_count(V)        (pak_arr_header((V))->count)
#define pak_arr_max(V)          (pak_arr_header((V))->max)
#define pak_arr_elem_sz(V)      (pak_arr_header((V))->elem_sz)
#define pak_arr_isvalid(V)      (pak_arr_header((V))->sig == PAK_ARR_SIGNATURE)
#define pak_arr_foreach(I, V)   for (I = 0; I < pak_arr_count(V); I++)
#define pak_arr_last(V)         V[pak_arr_count(V) - 1]
#define pak_arr_sort(V, CMP)    qsort((void *) (V), pak_arr_count(V), pak_arr_elem_sz(V), CMP);

/* Get values from the array , without knowing the type, for internal use */
#define pak_arr_notype_get(V, I)    (void *)((char *) (V) + ((I) * pak_arr_elem_sz(V)))
#define pak_arr_notype_last(V)      pak_arr_notype_get((V), pak_arr_count(V) - 1)

PAK_PREFIX void *pak__arr_new(size_t sz, int max);
#define pak_arr_new(T, M) (T *) pak__arr_new(sizeof(T), (M))

PAK_PREFIX void *pak__arr_new_gc(size_t sz, int max, pak__arr_gc gc);
#define pak_arr_new_gc(T, M, F) (T *) pak__arr_new_gc(sizeof(T), (M), (F))

PAK_PREFIX void pak__arr_free(void **pp);
#define pak_arr_free(PP) pak__arr_free((void **) (PP))

PAK_PREFIX int pak__arr_resize(void **pp, int max);
#define pak_arr_resize(PP, M) pak__arr_resize((void **) (PP), (M))

PAK_PREFIX int pak__arr_expand(void **pp);
#define pak_arr_expand(PP) pak__arr_expand((void **) (PP))

PAK_PREFIX int pak__arr_contract(void **pp);
#define pak_arr_contract(PP) pak__arr_contract((void **) (PP))

PAK_PREFIX int pak__arr_push(void **pp, size_t sz, void *e);
#define pak_arr_push(V, E) pak__arr_push((void **) (V), sizeof(E), (void *) &(E))

PAK_PREFIX int pak__arr_pop(void **pp);
#define pak_arr_pop(PP) pak__arr_pop((void **) (PP))

/* Create typesafe wrapper functions for the array implementation */
#define PAK_INIT_ARR(NAME, TYPE, GC)                                                                \
    typedef TYPE* NAME;                                                                             \
                                                                                                    \
    PAK_PREFIX int NAME##_count(NAME arr)           { return pak_arr_count(arr); }                  \
    PAK_PREFIX int NAME##_max(NAME arr)             { return pak_arr_max(arr); }                    \
    PAK_PREFIX size_t NAME##_elem_sz(NAME arr)      { return pak_arr_elem_sz(arr); }                \
    PAK_PREFIX int NAME##_isvalid(NAME arr)         { return pak_arr_isvalid(arr); }                \
    PAK_PREFIX NAME NAME##_new(int max)             { return pak_arr_new_gc(TYPE, max, GC); }       \
    PAK_PREFIX void NAME##_free(NAME *pp)           { pak_arr_free(pp); }                           \
    PAK_PREFIX int NAME##_resize(NAME *pp, int max) { return pak_arr_resize(pp, max); }             \
    PAK_PREFIX int NAME##_expand(NAME *pp)          { return pak_arr_expand(pp); }                  \
    PAK_PREFIX int NAME##_contract(NAME *pp)        { return pak_arr_contract(pp); }                \
    PAK_PREFIX int NAME##_push(NAME *pp, TYPE val)  { return pak_arr_push(pp, val); }               \
    PAK_PREFIX int NAME##_pop(NAME *pp)             { return pak_arr_pop(pp); }

/* For header files */
#define PAK_INIT_ARR_PROTOTYPES(NAME, TYPE)         \
    typedef TYPE* NAME;                             \
                                                    \
    extern int NAME##_count(NAME arr);              \
    extern int NAME##_max(NAME arr);                \
    extern size_t NAME##_elem_sz(NAME arr);         \
    extern int NAME##_isvalid(NAME arr);            \
    extern NAME NAME##_new(int max);                \
    extern void NAME##_free(NAME *pp);              \
    extern int NAME##_resize(NAME *pp, int max);    \
    extern int NAME##_expand(NAME *pp);             \
    extern int NAME##_contract(NAME *pp);           \
    extern int NAME##_push(NAME *pp, TYPE val);     \
    extern int NAME##_pop(NAME *pp);

/* Define commmon types for PAK arr */
#ifdef PAK_IMPLEMENTATION
    PAK_INIT_ARR(pak_iarr, int,     NULL)
    PAK_INIT_ARR(pak_larr, long,    NULL)
    PAK_INIT_ARR(pak_darr, double,  NULL)
    PAK_INIT_ARR(pak_farr, float,   NULL)
    PAK_INIT_ARR(pak_carr, char,    NULL)
    PAK_INIT_ARR(pak_sarr, char*,   NULL)
#else
#ifndef PAK_STATIC
    PAK_INIT_ARR_PROTOTYPES(pak_iarr, int)
    PAK_INIT_ARR_PROTOTYPES(pak_larr, long)
    PAK_INIT_ARR_PROTOTYPES(pak_darr, double)
    PAK_INIT_ARR_PROTOTYPES(pak_farr, float)
    PAK_INIT_ARR_PROTOTYPES(pak_carr, char)
    PAK_INIT_ARR_PROTOTYPES(pak_sarr, char*)
#endif
#endif

/* Begin function definitions */
#ifdef PAK_IMPLEMENTATION

PAK_PREFIX void *pak__arr_new(size_t sz, int max)
{
    pak__arr *arr = NULL;
    pak__arr *head = NULL;

    pak_assert(max > 0);

    arr = (pak__arr *)pak_malloc(sizeof(*arr) + (sz * max));
    pak_assert(arr);

    arr++;
    head = pak_arr_header(arr);

    head->count = 0;
    head->max = max;
    head->rate = max;
    head->elem_sz = sz;
    head->sig = PAK_ARR_SIGNATURE;
    head->gc = NULL;

    return arr;

fail:
    return NULL;
}

PAK_PREFIX void *pak__arr_new_gc(size_t sz, int max, pak__arr_gc gc)
{
    void *arr = pak__arr_new(sz, max);
    pak_assert(arr);

    pak_arr_header(arr)->gc = gc;

    return arr;

fail:
    return NULL;
}

PAK_PREFIX void pak__arr_free(void **pp)
{
    void *arr = *pp;
    pak__arr *head = NULL;

    pak_assert(arr); /* Double free? */

    head = pak_arr_header(arr);
    pak_assert(head->sig == PAK_ARR_SIGNATURE);

    while(head->gc && --head->count > 0)
        head->gc(pak_arr_notype_last(arr));

    pak_free(head);
    *pp = NULL;

fail:
    return;
}

PAK_PREFIX int pak__arr_resize(void **pp, int max)
{
    pak__arr *arr = NULL;
    pak__arr *head = NULL;
    pak__arr *new_head = NULL;

    pak_assert(max > 0);

    arr = *(pak__arr **) pp;
    head = pak_arr_header(arr);

    pak_assert(head->sig == PAK_ARR_SIGNATURE);

    if (max < head->count) {
        while (head->gc && --head->count > max)
            head->gc(pak_arr_notype_last(arr));

        head->count = max;
    }

    head->max = max;

    new_head = (pak__arr *)pak_realloc(head, sizeof(*new_head) + (head->elem_sz * max));
    pak_assert(new_head);

    *pp = new_head + 1;

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__arr_expand(void **pp)
{
    pak__arr *head = pak_arr_header(*pp);
    return pak__arr_resize(pp, head->max + head->rate);
}

PAK_PREFIX int pak__arr_contract(void **pp)
{
    pak__arr *head = pak_arr_header(*pp);
    return pak__arr_resize(pp, head->max - head->rate);
}

PAK_PREFIX int pak__arr_push(void **pp, size_t sz, void *e)
{
    pak__arr *arr = *(pak__arr **) pp;
    pak__arr *head = pak_arr_header(arr);

    pak_assert(head->sig == PAK_ARR_SIGNATURE);
    pak_assert(sz == head->elem_sz)

    if (head->count >= head->max) {
        pak_assert(pak_arr_expand(pp) == 0);
        arr = *(pak__arr **) pp;
        head = pak_arr_header(arr);
    }

    head->count++;

    memcpy(pak_arr_notype_last(arr), e, sz);

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__arr_pop(void **pp)
{
    pak__arr *head = pak_arr_header(*pp);
    pak_assert(head->sig == PAK_ARR_SIGNATURE);

    if (head->count > 0) {
        if (head->gc)
            head->gc(pak_arr_notype_last(*pp));

        head->count--;

        if (head->count < head->max - head->rate) {
            pak__arr_contract(pp);
            head = pak_arr_header(*pp);
        }
    }

    return 0;
        
fail:
    return -1;
}

#endif /* PAK_IMPLEMENTATION */
#endif /* PAK_NO_ARR */

/*
   End of PAK Array Library
*/

/*
    The PAK Dictionary (hashmap) Library:

        PAK Dicts are a highly customizable, generic, hashmap library,
        which use seperate chaining to handle collisons.

        PAK Dicts achieve type-safety through C++ template like C macros.
        Here is an example which creates a hashmap that uses a "char*"
        as the key, "int" as the value, and the FNV1A hash function:

            #define PAK_IMPLEMENTATION
            #include "pak.h"
            #include <assert.h>

            PAK_INIT_DICT(
                int_dict,

                // Key          Value
                char*,          int,
                const char*,    const int,
                strdup,         (int),
                != NULL,        || 1,

                // Functions
                (char *), strlen, 0 == strcmp,
                pak_dict_FNV1A,
                
                // Key          Value
                free,           (void)
            )

            int main(void)
            {
                int_dict dict = int_dict_new(1024);

                int_dict_insert(dict, "ABC", 1);
                int_dict_insert(dict, "DEF", 2);
                int_dict_insert(dict, "GHI", 3);

                assert(int_dict_get(dict, "ABC")->val == 1);
                assert(int_dict_get(dict, "DEF")->val == 2);
                assert(int_dict_get(dict, "GHI")->val == 3);

                return 0;
            }
        
        Notice the (ridiculous) amount of parameters passed into the PAK_INIT_DICT macro.
        The parameter amount is due to the fact that PAK Dicts are designed to work with
        a variety of different data types (e.g. data types from a library).

        Let's analyze the parameters passed into PAK_INIT_DICT:

            PAK_INIT_DICT(
                // Here we declare the type name for the dict,
                // also serves as a function prefix.
                int_dict,

                // Here are the data types stored into a key/value pair, respectively.
                char*, int,

                // Here are the data types passed into insertion functions,
                // usually the same as key/value pair types, but can be different.
                const char*, const int,

                // Here are some functions (or typecast to ignore) that copy key/value
                // parameters into the pairs themselves (e.g. use "strdup" to copy the key).
                strdup, (int),

                // Here are assertions to make sure that the copy functions above
                // worked properly (e.g. "strdup" will return NULL if it failed).
                != NULL, || 1,

                // Here we have a function (or typecast to ignore) that is used
                // to pass the key into the hash function. Must be of type "char*".
                (char *),
                
                // Here is a function to determine the length of the key you are
                // using, which is then passed into the hash function.
                strlen,

                // Here is a comparison function to compare keys, we just use plain
                // old strcmp, since the keys are of type "char*".
                0 == strcmp,

                // Here is the hashing function, PAK comes with some already.
                // The function must be of type "unsigned long(const char*, unsigned int)".
                pak_dict_FNV1A,

                // Here are the free functions when getting rid of key/value pairs.
                // We used "strdup" to store the keys, so "free" is needed.
                // For the "int" values, we have no need free memory, so use "(void)" to ignore.
                free, (void)
            )

        Most of this is overkill for a simple "char*" to "int" dictionary, but again,
        now we can use this with external data types, such as ones in found in libraries.

        For example here we have a dictionary that uses bstrlib, an alternative string library
        (the library is found here "http://bstring.sourceforge.net/"):

            PAK_INIT_DICT(
                bstr_dict,

                // Key          Value
                bstring,        int,
                const char*,    const int,
                bfromcstr,      (int),
                != NULL,        || 1,

                // Functions
                bdata, blength, 0 == bstrcmp,
                pak_dict_FNV1A,

                // Key          Value
                bdestroy,       (void)
            )
*/

#ifndef PAK_NO_DICT

#define PAK_INIT_DICT(NAME,                                     \
                      KEY_TYPE, VAL_TYPE,                       \
                      KEY_PARAM_TYPE, VAL_PARAM_TYPE,           \
                      KEY_COPY, VAL_COPY,                       \
                      KEY_ASSERT, VAL_ASSERT,                   \
                      KEY_ACCESS, KEY_COUNT, KEY_CMP,           \
                      HASH,                                     \
                      KEY_FREE, VAL_FREE)                       \
                                                                \
    typedef struct NAME##_pair {                                \
        KEY_TYPE key;                                           \
        VAL_TYPE val;                                           \
        struct NAME##_pair *next;                               \
    } NAME##_pair;                                              \
                                                                \
    typedef struct {                                            \
        unsigned int max;                                       \
        unsigned int rate;                                      \
        unsigned int busy;                                      \
        NAME##_pair **buckets;                                  \
    } NAME##_;                                                  \
                                                                \
    typedef NAME##_* NAME;                                      \
                                                                \
    unsigned int NAME##_busy(NAME dict)  { return dict->busy; } \
    unsigned int NAME##_max(NAME dict)   { return dict->max;  } \
    unsigned int NAME##_rate(NAME dict)  { return dict->rate; } \
                                                                \
    NAME NAME##_new(unsigned int sz)                            \
    {                                                           \
        NAME dict = NULL;                                       \
                                                                \
        dict = (NAME)pak_malloc(sizeof(*dict));                 \
        pak_assert(dict);                                       \
                                                                \
        dict->buckets = (NAME##_pair **)pak_malloc(             \
                sizeof(*dict->buckets) * sz);                   \
        pak_assert(dict->buckets);                              \
                                                                \
        dict->busy = 0;                                         \
        dict->max = sz;                                         \
        dict->rate = sz;                                        \
                                                                \
        memset(dict->buckets, (int)NULL,                        \
                sizeof(*dict->buckets) * sz);                   \
                                                                \
        return dict;                                            \
                                                                \
    fail:                                                       \
        if (dict)                                               \
            pak_free(dict);                                     \
                                                                \
        return NULL;                                            \
    }                                                           \
                                                                \
    void NAME##_free(NAME *pp)                                  \
    {                                                           \
        NAME dict = *pp;                                        \
                                                                \
        pak_assert(dict); /* Double free? */                    \
                                                                \
        if (dict->busy != 0) {                                  \
            unsigned int i;                                     \
            for (i = 0; i < dict->max; i++) {                   \
                NAME##_pair *curr = dict->buckets[i];           \
                while (curr) {                                  \
                    NAME##_pair *tmp = curr->next;              \
                                                                \
                    KEY_FREE(curr->key);                        \
                    VAL_FREE(curr->val);                        \
                    pak_free(curr);                             \
                                                                \
                    dict->buckets[i] = tmp;                     \
                    curr = tmp;                                 \
                }                                               \
            }                                                   \
        }                                                       \
                                                                \
        pak_free(dict);                                         \
        *pp = NULL;                                             \
                                                                \
    fail:                                                       \
        return;                                                 \
    }                                                           \
                                                                \
    int NAME##_insert(NAME dict, KEY_PARAM_TYPE key, VAL_PARAM_TYPE val)\
    {                                                           \
        NAME##_pair *pair = NULL;                               \
        pak_bool key_alloced = PAK_FALSE;                       \
        pak_bool val_alloced = PAK_FALSE;                       \
        unsigned int loc;                                       \
                                                                \
        pair = (NAME##_pair *)pak_malloc(sizeof(*pair));        \
        pak_assert(pair);                                       \
                                                                \
        pair->key = KEY_COPY(key);                              \
        if (pair->key KEY_ASSERT)                               \
            key_alloced = PAK_TRUE;                             \
        else                                                    \
            goto fail;                                          \
                                                                \
        pair->val = VAL_COPY(val);                              \
        if (pair->val VAL_ASSERT)                               \
            val_alloced = PAK_TRUE;                             \
        else                                                    \
            goto fail;                                          \
                                                                \
        pair->next = NULL;                                      \
                                                                \
        loc = HASH(KEY_ACCESS(pair->key),                       \
                   KEY_COUNT(pair->key)) % dict->max;           \
                                                                \
        if (dict->buckets[loc] == NULL) {                       \
            dict->buckets[loc] = pair;                          \
            dict->busy++;                                       \
        } else { /* Collison! */                                \
            NAME##_pair *curr = dict->buckets[loc];             \
                                                                \
            for (;;) {                                          \
                pak_assert(!(KEY_CMP(curr->key, pair->key)));   \
                if (curr->next)                                 \
                    curr = curr->next;                          \
                else                                            \
                    break;                                      \
            }                                                   \
                                                                \
            curr->next = pair;                                  \
        }                                                       \
                                                                \
        return 0;                                               \
                                                                \
    fail:                                                       \
        if (pair) {                                             \
            if (key_alloced)                                    \
                KEY_FREE(pair->key);                            \
            if (val_alloced)                                    \
                VAL_FREE(pair->key);                            \
                                                                \
            pak_free(pair);                                     \
        }                                                       \
                                                                \
        return -1;                                              \
    }                                                           \
                                                                \
    void NAME##_remove(NAME dict, KEY_PARAM_TYPE key)           \
    {                                                           \
        unsigned int loc;                                       \
        KEY_TYPE cpy;                                           \
        NAME##_pair *curr = NULL;                               \
        NAME##_pair *prev = NULL;                               \
                                                                \
        cpy = KEY_COPY(key);                                    \
        if (!(cpy KEY_ASSERT))                                  \
            return;                                             \
                                                                \
        loc = HASH(KEY_ACCESS(cpy),                             \
                   KEY_COUNT(cpy)) % dict->max;                 \
                                                                \
        curr = dict->buckets[loc];                              \
                                                                \
        while (curr) {                                          \
            if (KEY_CMP(curr->key, cpy)) {                      \
                NAME##_pair *tmp = curr->next;                  \
                                                                \
                KEY_FREE(curr->key);                            \
                VAL_FREE(curr->val);                            \
                pak_free(curr);                                 \
                                                                \
                if (prev)                                       \
                    prev->next = tmp;                           \
                else {                                          \
                    dict->buckets[loc] = tmp;                   \
                    if (!tmp)                                   \
                        dict->busy--;                           \
                }                                               \
                                                                \
                break;                                          \
            }                                                   \
                                                                \
            prev = curr;                                        \
            curr = curr->next;                                  \
        }                                                       \
                                                                \
        KEY_FREE(cpy);                                          \
    }                                                           \
                                                                \
                                                                \
    NAME##_pair *NAME##_get(NAME dict, KEY_PARAM_TYPE key)      \
    {                                                           \
        unsigned int loc;                                       \
        KEY_TYPE cpy;                                           \
        NAME##_pair *curr = NULL;                               \
                                                                \
        cpy = KEY_COPY(key);                                    \
        if (!(cpy KEY_ASSERT))                                  \
            return NULL;                                        \
                                                                \
        loc = HASH(KEY_ACCESS(cpy),                             \
                   KEY_COUNT(cpy)) % dict->max;                 \
                                                                \
        curr = dict->buckets[loc];                              \
                                                                \
        while (curr) {                                          \
            if (KEY_CMP(curr->key, cpy)) {                      \
                KEY_FREE(cpy);                                  \
                return curr;                                    \
            }                                                   \
            curr = curr->next;                                  \
        }                                                       \
                                                                \
        KEY_FREE(cpy);                                          \
        return NULL;                                            \
    }                                                           \
                                                                \
    int NAME##_set(NAME dict, KEY_PARAM_TYPE key, VAL_PARAM_TYPE val)\
    {                                                           \
        NAME##_pair *pair = NULL;                               \
                                                                \
        pair = NAME##_get(dict, key);                           \
        pak_assert(pair);                                       \
                                                                \
        pair->val = VAL_COPY(val);                              \
        pak_assert(pair->val VAL_ASSERT);                       \
                                                                \
        return 0;                                               \
                                                                \
    fail:                                                       \
        return -1;                                              \
    }

#define PAK_INIT_DICT_PROTOTYPES(NAME, KEY_TYPE, VAL_TYPE,                        \
                                 KEY_PARAM_TYPE, VAL_PARAM_TYPE)                  \
                                                                                  \
    typedef struct NAME##_pair {                                                  \
        KEY_TYPE key;                                                             \
        VAL_TYPE val;                                                             \
        struct NAME##_pair *next;                                                 \
    } NAME##_pair;                                                                \
                                                                                  \
    typedef struct {                                                              \
        int max;                                                                  \
        int rate;                                                                 \
        int busy;                                                                 \
        NAME##_pair **buckets;                                                    \
    } NAME##_;                                                                    \
                                                                                  \
    typedef NAME##_* NAME;                                                        \
                                                                                  \
    extern unsigned int NAME##_busy(NAME dict);                                   \
    extern unsigned int NAME##_max(NAME dict);                                    \
    extern unsigned int NAME##_rate(NAME dict);                                   \
                                                                                  \
    extern NAME NAME##_new(unsigned int sz)                                       \
    extern void NAME##_free(NAME *pp);                                            \
    extern int NAME##_insert(NAME dict, KEY_PARAM_TYPE key, VAL_PARAM_TYPE val);  \
    extern void NAME##_remove(NAME dict, KEY_PARAM_TYPE key);                     \
    extern NAME##_pair *NAME##_get(NAME dict, KEY_PARAM_TYPE key);                \
    extern int NAME##_set(NAME dict, KEY_PARAM_TYPE key, VAL_PARAM_TYPE val);

/* Hashing algorithms */
PAK_PREFIX pak_ui32 pak_dict_FNV1A(const pak_i8 *data, unsigned int len);
PAK_PREFIX pak_ui32 pak_dict_ADLER32(const pak_i8 *data, unsigned int len);
PAK_PREFIX pak_ui32 pak_dict_jenkins(const pak_i8 *data, unsigned int len);
PAK_PREFIX pak_ui32 pak_dict_murmur3(const pak_ui8 *data, unsigned int len, pak_ui32 key);

#ifdef PAK_IMPLEMENTATION

/* Fowler–Noll–Vo hash */
pak_ui32 pak_dict_FNV1A(const pak_i8 *data, unsigned int len)
{
    static const pak_ui32 FNV_PRIME  = 16777619;
    static const pak_ui32 FNV_OFFSET = 2166136261;

    pak_ui32 hash = FNV_OFFSET;
    pak_ui32 i;

    for (i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

/* ADLER32 checksum */
pak_ui32 pak_dict_ADLER32(const pak_i8 *data, unsigned int len)
{
    static const pak_ui32 MOD_ADLER = 65521;
    unsigned int i;
    pak_ui32 a = 1, b = 0;

    for (i = 0; i < len; i++) {
        a = (a + data[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}

/* Bob Jenkins "One at a Time" hash */
pak_ui32 pak_dict_jenkins(const pak_i8 *data, unsigned int len)
{
    int i = 0;
    pak_ui32 hash = 0;

    while (i != len) {
        hash += data[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}

/* Murmur3 hash */
pak_ui32 pak_dict_murmur3(const pak_ui8 *data, unsigned int len, pak_ui32 seed)
{
    pak_ui32 h = seed;

    if (len > 3) {
        const pak_ui32* key_x4 = (const pak_ui32*) data;
        unsigned int i = len >> 2;

        do {
            pak_ui32 k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h += (h << 2) + 0xe6546b64;
        } while (--i);

        data = (const pak_ui8*) key_x4;
    }
    
    if (len & 3) {
        unsigned int i = len & 3;
        pak_ui32 k = 0;
        data = &data[i - 1];
        do {
            k <<= 8;
            k |= *data--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }

    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

#endif /* PAK_IMPLEMENTATION */
#endif /* PAK_NO_DICT */

/*
   End of PAK Dictionary Library
*/

/*
   The PAK I/O Library

   This library contains various utility functions for managing file input and
   output. You can do things such as opening text files as strings with a single
   function call.
*/

#ifndef PAK_NO_IO

#ifdef PAK_NO_ARR
#   error "PAK I/O depends on PAK arrays"
#endif

PAK_PREFIX char *pak_io_read_file(const char *path);
PAK_PREFIX int pak_io_append_file(const char *path, const char *s, ...);

#ifdef PAK_IMPLEMENTATION

PAK_PREFIX pak_carr pak_io_read_file(const char *path)
{
    pak_carr s = NULL;
    FILE *f = NULL;
    size_t sz;

    f = fopen(path, "r");
    pak_assert(f);

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    rewind(f);

    /* +1 for NULL byte */
    s = pak_carr_new(sz + 1);
    pak_assert(s);

    fread(s, sizeof(char), sz, f);
    fclose(f);

    s[sz] = '\0';

    return s;

fail:
    if (f)
        fclose(f);

    if (s)
        pak_carr_free(&s);

    return NULL;
}

PAK_PREFIX int pak_io_append_file(const char *path, const char *s, ...)
{
    FILE *f = NULL;
    va_list a;

    f = fopen(path, "a");
    pak_assert(f);

    va_start(a, s);

    vfprintf(f, s, a);

    va_end(a);
    fputs("\n", f);
    fclose(f);

    return 0;

fail:
    if (f)
        fclose(f);

    return -1;
}

#endif /* PAK_IMPLEMENTATION */
#endif /* PAK_NO_IO */

/*
    End of PAK I/O Library
*/

#ifdef __cplusplus
}
#endif

#endif /* PAK_HEADER */
