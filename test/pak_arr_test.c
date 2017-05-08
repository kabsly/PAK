#include "pak_test.h"
#include "pak_arr_test.h"

#include <stdlib.h>
#include <pak.h>

PAK_INIT_ARR(IntArray, int, NULL);

void arr_gc(void *e)
{
    int **p = (int **) e; 
    free(*p);
}
PAK_INIT_ARR(GCArray, int*, arr_gc);

// Test out raw PAK vectors (the non-typesafe void* ones)
char *pak_arr_raw_test()
{
    static const int NUM_PUSHES = 5000;

    int *arr = pak_arr_new(int, 1024);
    pak_test_assert(arr, "Failed to create void* array.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = pak_arr_push(&arr, i);
        pak_test_assert(rc == 0, "Failed to push value onto void* array.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = pak_arr_pop(&arr);
        pak_test_assert(rc == 0, "Failed to pop value from void* array.");
    }

    pak_arr_free(&arr);

    return NULL;
}

// Test out typesafe PAK vectors
char *pak_arr_typesafe_test()
{
    static const int NUM_PUSHES = 5000;

    IntArray arr = IntArray_new(1024);
    pak_test_assert(arr, "Failed to create typesafe vector.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = IntArray_push(&arr, i);
        pak_test_assert(rc == 0, "Failed to push value onto typesafe vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = IntArray_pop(&arr);
        pak_test_assert(rc == 0, "Failed to pop value from typesafe vector.");
    }

    IntArray_free(&arr);

    return NULL;
}

// Test vector garbage collection
char *pak_arr_gc_test()
{
    static const int NUM_PUSHES = 5000;

    GCArray arr = GCArray_new(1024);
    pak_test_assert(arr, "Failed to create GC vector.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int *p = malloc(sizeof(int));
        *p = i;

        int rc = GCArray_push(&arr, p);
        pak_test_assert(rc == 0, "Failed to push value onto GC vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = GCArray_pop(&arr);
        pak_test_assert(rc == 0, "Failed to pop value from GC vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int *p = malloc(sizeof(int));
        *p = i;

        int rc = GCArray_push(&arr, p);
        pak_test_assert(rc == 0, "Failed to push value onto GC vector.");
    }

    // See how GCArray reacts to resizes
    GCArray_resize(&arr, 1);
    GCArray_free(&arr);

    return NULL;
}

char *pak_arr_test()
{
    pak_test_run(pak_arr_raw_test);
    pak_test_run(pak_arr_typesafe_test);
    pak_test_run(pak_arr_gc_test);

    return NULL;
}
