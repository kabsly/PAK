/*
    The PAK Library Collection:

        The PAK libraries are a set of useful single header libraries written
        for C/C++.

        PAK takes heavy inspiration from the STB libraries found here:
            https://github.com/nothings/stb

        Similarly to STB, this library is highly tailored to the needs of it's
        author, so this library's usefulness may vary.

        Here is a list of the libraries found in PAK:

            - PAK Compare, generic sorting macros
            - PAK Lists, generic linked list library
            - PAK Arrays, generic dynamic array library
            - PAK I/O, file input and output library

        Libraries can be disabled, for example just define PAK_NO_ARR in order
        to disable PAK arrays from being used.

        You must define PAK_IMPLEMENTATION before including this header file to define
        all of the functions, otherwise you'll just get the prototypes.

            #define PAK_IMPLEMENTATION
            #include "pak.h"

        It is also important to know how PAK achieves type-safety in order to
        use the library effectively. PAK uses C macros in order to improvise
        C++ style templates. Consider the following code:

            // Define a array type called "IntArray" which holds int values
            PAK_INIT_ARR(IntArray, int, NULL);

            int main()
            {
                IntArray vec = IntArray_new(1024);

                int i;
                for (i = 0; i < 2048; i++)
                    IntArray_push(&vec, i);
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
#   define pak_assert(C) if (!(C)) { fprintf(stderr, "PAK Assertion fail (%s:%d) %s.\n",\
        __FILE__, __LINE__, #C); goto fail; }
#else
#   define pak_assert(C) if (!(C)) goto fail;
#endif

// Malloc wrapper
#if defined PAK_IMPLEMENTATION && !defined PAK_NO_MALLOC
#   include <stdlib.h>
#   define pak_malloc   malloc
#   define pak_calloc   calloc
#   define pak_realloc  realloc
#   define pak_free     free
#endif

#ifdef PAK_IMPLEMENTATION
#   include <stdio.h>
#   include <string.h> // memcpy
#   include <stdarg.h>
#else
#   include <stddef.h> // size_t
#endif

/*
   The PAK Comparision Library

   Generic comparison functions all macroed up. Especially made for use in
   C standard sorting functions
*/

#define PAK_INIT_COMPARE(NAME, TYPE, OP)                   \
    int NAME(const void *a, const void *b)                 \
    {                                                      \
        return -(*(const TYPE *) a OP *(const TYPE *) b);  \
    }

#define PAK_INIT_COMPARE_PROTOTYPE(NAME)    \
    extern int NAME(const void *a, const void *b);

#ifdef PAK_IMPLEMENTATION
    PAK_INIT_COMPARE(pak_icmp_des, int,     +)
    PAK_INIT_COMPARE(pak_fcmp_des, float,   +)
    PAK_INIT_COMPARE(pak_dcmp_des, double,  +)
#else
    PAK_INIT_COMPARE_PROTOTYPE(pak_icmp_des)
    PAK_INIT_COMPARE_PROTOTYPE(pak_fcmp_des)
    PAK_INIT_COMPARE_PROTOTYPE(pak_dcmp_des)
#endif

/*
   End of PAK comparision library
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

// Memory managment function used when we delete elements from the list
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

// Looping utilities
#define pak_list_foreach(L, N)       for (N = L->first; N != NULL && N->next != NULL; N = N->next)
#define pak_list_foreach_back(L, N)  for (N = L->last ; N != NULL && N->prev != NULL; N = N->prev)
#define pak_node_data(T, N)          *(T *) (N)->data

// Function protypes with wrapper macros

// List creation and destruction
PAK_PREFIX pak_list *pak__list_new(size_t elem_sz);
#define pak_list_new(T) pak__list_new(sizeof(T))

PAK_PREFIX pak_list *pak__list_new_gc(size_t elem_sz, pak__list_gc gc);
#define pak_list_new_gc(T, GC) pak__list_new_gc(sizeof(T), GC)

PAK_PREFIX void pak_list_free(pak_list **pp);

// Pushing and popping
PAK_PREFIX void pak_list_pop(pak_list *list);
PAK_PREFIX int pak__list_push(pak_list *list, size_t sz, void *e);
#define pak_list_push(L, E) pak__list_push(L, sizeof(E), &(E))

// Unshifting and shifting
PAK_PREFIX void pak_list_shift(pak_list *list);
PAK_PREFIX int pak__list_unshift(pak_list *list, size_t sz, void *e);
#define pak_list_unshift(L, E) pak__list_unshift(L, sizeof(E), &(E))

// Create typesafe wrapper functions around the void* ones
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

// For header files
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

// Begin PAK list function definitions
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

// Clear out all of the elements and free the list
PAK_PREFIX void pak_list_free(pak_list **pp)
{
    pak_list *list = *pp;
    pak_assert(list); // Double free?

    int i;
    for (i = 0; i < list->count; i++)
        pak_list_pop(list);

    pak_free(list);
    *pp = NULL;

fail:
    return;
}

// Push value to back of the list
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

// Remove elements at the beginning of the list
PAK_PREFIX void pak_list_pop(pak_list *list)
{
    pak_assert(list->count > 0);
    pak_assert(list->last);

    pak_node *last = list->last;
    pak_node *prev = last->prev;

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

// Push element to front of the list
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

// Remove element from beginning of the list
PAK_PREFIX void pak_list_shift(pak_list *list)
{
    pak_assert(list->count > 0);
    pak_assert(list->first);

    pak_node *first = list->first;
    pak_node *next = first->next;

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

#endif // PAK_IMPLEMENTATION
#endif // PAK_NO_LIST

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
        - Optional garbage collection
        - Ability to index like a normal C array (e.g. "array[0]")

    Notes:
        It is not recommended that you use this section of PAK in C++, due to the
        fact that it is redundant to the STL.

    Example:

        int *arr = pak_arr_new(int, 1024);
        assert(arr);

        int i;
        for (i = 0; i < 1000; i++)
            pak_arr_push(&arr, i);

        for (i = 0; i < 1000; i++)
            pak_arr_pop(&arr);
*/

#ifndef PAK_NO_ARR

// Function pointer to a free elements when doing resizes, pops, etc
// (I call it "garbage collection", because using "garbage collection" and "C"
// in the same sentence gets people's attention.)
typedef void (*pak__arr_gc)(void *);

// A unique signature kept inside of the array metadata
#ifndef PAK_ARR_SIGNATURE
#   define PAK_ARR_SIGNATURE 0x5F3C2A
#endif

// Header which contains the array metadata
typedef struct {
    int count;
    int max;
    int rate;
    size_t elem_sz;
    unsigned int sig;
    pak__arr_gc gc;
} pak__arr;

// The header is always the first element at the array, but we increment the
// pointer in order to hide it from the user
#define pak_arr_header(V)   (((pak__arr *) (V) - 1))

// Utility macros
#define pak_arr_count(V)        (pak_arr_header((V))->count)
#define pak_arr_max(V)          (pak_arr_header((V))->max)
#define pak_arr_elem_sz(V)      (pak_arr_header((V))->elem_sz)
#define pak_arr_isvalid(V)      (pak_arr_header((V))->sig == PAK_ARR_SIGNATURE)
#define pak_arr_foreach(I, V)   for (I = 0; I < pak_arr_count(V); I++)
#define pak_arr_last(V)         V[pak_arr_count(V) - 1]
#define pak_arr_sort(V, CMP)    qsort((void *) (V), pak_arr_count(V), pak_arr_elem_sz(V), CMP);

// Get values from the array , without knowing the type, mostly for internal
// use, but we'll make it available to the user
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

// Create typesafe wrapper functions for the array implementation,
// using a "ghetto" C++ style template.
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

// For header files
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

// Define commmon types for PAK arr
#ifdef PAK_IMPLEMENTATION
    PAK_INIT_ARR(pak_iarr, int,     NULL);
    PAK_INIT_ARR(pak_darr, double,  NULL);
    PAK_INIT_ARR(pak_farr, float,   NULL);
    PAK_INIT_ARR(pak_carr, char,    NULL);
#else
    PAK_INIT_ARR_PROTOTYPES(pak_iarr, int);
    PAK_INIT_ARR_PROTOTYPES(pak_darr, double);
    PAK_INIT_ARR_PROTOTYPES(pak_farr, float);
    PAK_INIT_ARR_PROTOTYPES(pak_carr, char);
#endif

#define pak_iarr_foreach pak_arr_foreach
#define pak_darr_foreach pak_arr_foreach
#define pak_farr_foreach pak_arr_foreach
#define pak_carr_foreach pak_arr_foreach

// Begin function definitions
#ifdef PAK_IMPLEMENTATION

PAK_PREFIX void *pak__arr_new(size_t sz, int max)
{
    pak_assert(max > 0);

    pak__arr *arr = (pak__arr *)pak_malloc(sizeof(*arr) + (sz * max));
    pak_assert(arr);

    arr++;

    pak__arr *h = pak_arr_header(arr);

    h->count = 0;
    h->max = max;
    h->rate = max;
    h->elem_sz = sz;
    h->sig = PAK_ARR_SIGNATURE;
    h->gc = NULL;

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
    pak_assert(arr); // Double free?

    pak__arr *h = pak_arr_header(arr);
    pak_assert(h->sig == PAK_ARR_SIGNATURE);

    while(h->gc && --h->count > 0)
        h->gc(pak_arr_notype_last(arr));

    pak_free(h);
    *pp = NULL;

fail:
    return;
}

PAK_PREFIX int pak__arr_resize(void **pp, int max)
{
    pak_assert(max > 0);

    pak__arr *h = pak_arr_header(*pp);
    pak_assert(h->sig == PAK_ARR_SIGNATURE);

    if (max < h->count) {

        // Collect the garbage

        int i = 0;
        while (h->gc && --h->count > max) {
            h->gc(pak_arr_notype_last(*pp));
            i++;
        }

        h->count = max;
    }

    h->max = max;

    pak__arr *arr = (pak__arr *)pak_realloc(h, sizeof(*arr) + (h->elem_sz * max));
    pak_assert(arr);

    arr++;

    *pp = arr;

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__arr_expand(void **pp)
{
    pak__arr *h = pak_arr_header(*pp);
    pak_assert(h->sig == PAK_ARR_SIGNATURE);

    return pak__arr_resize(pp, h->max + h->rate);

fail:
    return -1;
}

PAK_PREFIX int pak__arr_contract(void **pp)
{
    pak__arr *h = pak_arr_header(*pp);
    pak_assert(h->sig == PAK_ARR_SIGNATURE);

    return pak__arr_resize(pp, h->max - h->rate);

fail:
    return -1;
}

PAK_PREFIX int pak__arr_push(void **pp, size_t sz, void *e)
{
    pak__arr *arr = *(pak__arr **) pp;
    pak__arr *h = pak_arr_header(arr);

    pak_assert(h->sig == PAK_ARR_SIGNATURE);
    pak_assert(sz == h->elem_sz)

    if (h->count >= h->max) {
        pak_assert(pak_arr_expand(pp) == 0);
        h = pak_arr_header(arr);
    }

    h->count++;

    memcpy(pak_arr_notype_last(arr), e, sz);

    return 0;

fail:
    return -1;
}

PAK_PREFIX int pak__arr_pop(void **pp)
{
    pak__arr *h = pak_arr_header(*pp);
    pak_assert(h->sig == PAK_ARR_SIGNATURE);

    if (h->count > 0) {
        if (h->gc)
            h->gc(pak_arr_notype_last(*pp));

        h->count--;

        if (h->count < h->max - h->rate) {
            pak__arr_contract(pp);
            h = pak_arr_header(*pp);
        }
    }

    return 0;
        
fail:
    return -1;
}

#endif // PAK_IMPLEMENTATION
#endif // PAK_NO_ARR

/*
   End of PAK Array Library
*/

/*
    The PAK Dictionary (hashmap) Library

    A generic hashmap implementation, that uses seperate chaining and the FNV1A
    hash function by default.
*/

#ifndef PAK_NO_DICT

#if defined PAK_NO_ARR
#   error "PAK Dictionaries require array to be used."
#endif

// Function pointers for hashing and memory management
typedef unsigned long (*pak__dict_hash) (const char *);
typedef void          (*pak__dict_gc)   (void *);

// Key Value Pairs
typedef struct {
    char *key;
    void *val;
    struct pak_dict_node *next;
} pak_dict_node;

// Define a PAK Array which will serve as our buckets
#ifdef PAK_IMPLEMENTATION
    PAK_INIT_ARR(pak__buckets, pak_dict_node*, NULL)
#else // Avoid dual typedefs
    PAK_INIT_ARR_PROTOTYPES(pak__buckets, pak_dict_node*)
#endif

typedef struct {
    size_t elem_sz;
    pak__buckets buckets;
    pak__dict_gc gc;
    pak__dict_hash hash;
} pak_dict;

// Hashing algorithms
PAK_PREFIX unsigned long pak_dict_FNV1A(const char *data);

// Function declarations, with wrapper macros

PAK_PREFIX pak_dict *pak__dict_new(int max, size_t elem_sz);
#define pak_dict_new(T, M) pak__dict_new(M, sizeof(T))

PAK_PREFIX int pak__dict_add(pak_dict *dict, char *key, size_t sz, void *val);
#define pak_dict_add(D, K, V) pak__dict_add(D, K, sizeof(V), (void *) &(V))

// Begin PAK Dictionary implementation
#ifdef PAK_IMPLEMENTATION

// Fowler-Noll-Vo hash function
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

PAK_PREFIX pak_dict *pak__dict_new(int max, size_t elem_sz)
{
    pak_dict *dict = NULL;
    pak__buckets buck = NULL;

    dict = (pak_dict *)pak_malloc(sizeof(*dict));
    pak_assert(dict);

    buck = pak__buckets_new(max);
    pak_assert(buck);

    dict->elem_sz = elem_sz;
    dict->buckets = buck;
    dict->gc = NULL;
    // Use FNV1A hash by default
    dict->hash = pak_dict_FNV1A;

    return dict;

fail:
    if (dict)
        pak_free(dict);

    return NULL;
}

PAK_PREFIX pak_dict_node *pak__dict_node_new(char *key, void *val)
{
    pak_dict_node *node = pak_malloc(sizeof(*node));
    pak_assert(node);

    node->key = strdup(key);
    pak_assert(node->key);

    node->val = val;
    node->next = NULL;

    return node;

fail:
    if (node)
        pak_free(node);

    return NULL;
}

PAK_PREFIX int pak__dict_add(pak_dict *dict, char *key, size_t sz, void *p)
{
    void *val = NULL;

    pak_assert(sz == dict->elem_sz);

    val = pak_malloc(sz);
    pak_assert(val);

    memcpy(val, p, sz);

    int loc = dict->hash(key) % pak__buckets_max(dict->buckets);
    pak_dict_node *next = dict->buckets[loc];

    return 0;

fail:
    if (val)
        pak_free(val);

    return -1;
}

#endif // PAK_IMPLEMENTATION
#endif // PAK_NO_DICT

/*
   End of PAK Dictionary Library
*/

/*
   The PAK I/O Library

   This library contains various utility functions for managing file input and
   output. You can do things such as opening text files as strings with a single
   function call, or listing out a directory of files, etc.
*/

#ifndef PAK_NO_IO

PAK_PREFIX char *pak_io_read_file(const char *path);
PAK_PREFIX int pak_io_append_file(const char *path, const char *s, ...);
PAK_PREFIX int *pak_io_search_str(const char *to_search, const char *pattern);
PAK_PREFIX int *pak_io_search_file(const char *path, const char *pattern);

#ifdef PAK_IMPLEMENTATION

PAK_PREFIX char *pak_io_read_file(const char *path)
{
    FILE *f = NULL;
    char *s = NULL;
    size_t sz;

    f = fopen(path, "r");
    pak_assert(f);

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    rewind(f);

    // +1 for NULL byte
    s = (char *)pak_malloc(sizeof(*s) * (sz + 1));
    pak_assert(s);

    fread(s, sizeof(char), sz, f);
    fclose(f);

    s[sz] = '\0';

    return s;

fail:
    if (f)
        fclose(f);

    if (s)
        pak_free(s);

    return NULL;
}

PAK_PREFIX int pak_io_append_file(const char *path, const char *s, ...)
{
    FILE *f = fopen(path, "a");
    pak_assert(f);

    va_list a;
    va_start(a, s);

    vfprintf(f, s, a);

    va_end(a);
    fputs("\n", f);
    fclose(f);

    return 0;

fail:
    return -1;
}

PAK_PREFIX int *pak_io_search_str(const char *to_search, const char *pattern)
{
    int slen = strlen(to_search);
    int plen = strlen(pattern);
    int i, j;

    pak_iarr finds = pak_iarr_new(1);
    pak_assert(finds);

    for (i = 0; i < slen; i++)
        if (pattern[0] == to_search[i])
            for (j = 0; j < plen && j + i < slen; j++) {
                if (pattern[j] != to_search[i + j])
                    break;
                else if (j == plen - 1)
                    pak_iarr_push(&finds, i);
            }

    return finds;

fail:
    if (finds)
        pak_iarr_free(&finds);

    return NULL;
}

PAK_PREFIX int *pak_io_search_file(const char *path, const char *pattern)
{
    char *file = pak_io_read_file(path);
    pak_assert(file);

    int *finds = pak_io_search_str(file, pattern);
    pak_assert(finds);

    return finds;

fail:
    if (file)
        pak_free(file);

    return NULL;
}

/*
   @TODO(kabsly):

    Features to add:
        - Read entire directories of files, indexed with a hashmap, with the
          file name as the key.
        - Add file searching functions
*/

#endif // PAK_IMPLEMENTATION
#endif // PAK_NO_IO

/*
    End of PAK I/O Library
*/

#ifdef __cplusplus
}
#endif

#endif // PAK_HEADER
