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

            - PAK Lists, generic linked list library
            - PAK Arrays, generic dynamic array library
            - PAK I/O, file input and output library

        More are always on the way.

    Disabling Libraries:

        Libraries can be disabled with defines before including this file

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
#   define pak_assert(C) if (!(C)) { fprintf(stderr, "PAK Assertion fail (%s:%d) %s.\n",\
        __FILE__, __LINE__, #C); goto fail; }
#else
#   define pak_assert(C) if (!(C)) goto fail;
#endif

/* Malloc wrapper */
#if defined PAK_IMPLEMENTATION && !defined PAK_NO_MALLOC
#   include <stdlib.h>
#   define pak_malloc   malloc
#   define pak_calloc   calloc
#   define pak_realloc  realloc
#   define pak_free     free
#endif

#ifdef PAK_IMPLEMENTATION
#   include <stdio.h>
#   include <string.h> /* memcpy */
#   include <stdarg.h>
#else
#   include <stddef.h> /* size_t */
#endif

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
#define pak_node_data(T, N)          *(T *) (N)->data

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
    PAK_PREFIX TYPE NAME##_data(NAME##_node n)      { return pak_node_data(TYPE, n); }

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
    PAK_INIT_ARR_PROTOTYPES(pak_iarr, int)
    PAK_INIT_ARR_PROTOTYPES(pak_larr, long)
    PAK_INIT_ARR_PROTOTYPES(pak_darr, double)
    PAK_INIT_ARR_PROTOTYPES(pak_farr, float)
    PAK_INIT_ARR_PROTOTYPES(pak_carr, char)
    PAK_INIT_ARR_PROTOTYPES(pak_sarr, char*)
#endif

#define pak_iarr_foreach pak_arr_foreach
#define pak_larr_foreach pak_arr_foreach
#define pak_darr_foreach pak_arr_foreach
#define pak_farr_foreach pak_arr_foreach
#define pak_carr_foreach pak_arr_foreach
#define pak_sarr_foreach pak_arr_foreach

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
        arr = *pp;
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
    The PAK Dictionary (hashmap) Library

    A generic hashmap implementation, that uses seperate chaining and the FNV1A
    hash function by default. (WIP)
*/

#ifndef PAK_NO_DICT

#if defined PAK_NO_ARR
#   error "PAK Dictionaries require array to be used."
#endif

/* Hashing algorithms */
PAK_PREFIX unsigned long pak_dict_FNV1A(const char *data);

/* Begin PAK Dictionary implementation */
#ifdef PAK_IMPLEMENTATION

/* Fowler-Noll-Vo hash function */
PAK_PREFIX unsigned long pak_dict_FNV1A(const char *data)
{
    static const unsigned long FNV_PRIME  = 16777619;
    static const unsigned long FNV_OFFSET = 2166136261;

    unsigned long hash = FNV_OFFSET;

    unsigned long i;
    for (i = 0; i < strlen(data); i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }

    return hash;
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
