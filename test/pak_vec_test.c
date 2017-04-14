#include "pak_test.h"
#include "pak_vec_test.h"

#include <stdlib.h>
#include <pak.h>

PAK_INIT_VEC(IntVector, int, NULL);

void vec_gc(void *e)
{
    int **p = (int **) e; 
    free(*p);
}
PAK_INIT_VEC(GCVector, int*, vec_gc);

// Test out raw PAK vectors (the non-typesafe void* ones)
char *pak_vec_raw_test()
{
    static const int NUM_PUSHES = 5000;

    int *vec = pak_vec_new(int, 1024);
    pak_test_assert(vec, "Failed to create void* vector.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = pak_vec_push(&vec, i);
        pak_test_assert(rc == 0, "Failed to push value onto void* vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = pak_vec_pop(&vec);
        pak_test_assert(rc == 0, "Failed to pop value from void* vector.");
    }

    pak_vec_free(&vec);

    return NULL;
}

// Test out typesafe PAK vectors
char *pak_vec_typesafe_test()
{
    static const int NUM_PUSHES = 5000;

    IntVector vec = IntVector_new(1024);
    pak_test_assert(vec, "Failed to create typesafe vector.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = IntVector_push(&vec, i);
        pak_test_assert(rc == 0, "Failed to push value onto typesafe vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = IntVector_pop(&vec);
        pak_test_assert(rc == 0, "Failed to pop value from typesafe vector.");
    }

    IntVector_free(&vec);

    return NULL;
}

// Test vector garbage collection
char *pak_vec_gc_test()
{
    static const int NUM_PUSHES = 5000;

    GCVector vec = GCVector_new(1024);
    pak_test_assert(vec, "Failed to create GC vector.");

    int i;
    for (i = 0; i < NUM_PUSHES; i++) {
        int *p = malloc(sizeof(int));
        *p = i;

        int rc = GCVector_push(&vec, p);
        pak_test_assert(rc == 0, "Failed to push value onto GC vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int rc = GCVector_pop(&vec);
        pak_test_assert(rc == 0, "Failed to pop value from GC vector.");
    }

    for (i = 0; i < NUM_PUSHES; i++) {
        int *p = malloc(sizeof(int));
        *p = i;

        int rc = GCVector_push(&vec, p);
        pak_test_assert(rc == 0, "Failed to push value onto GC vector.");
    }

    // See how GCVector reacts to resizes
    GCVector_resize(&vec, 1);
    GCVector_free(&vec);

    return NULL;
}

char *pak_vec_test()
{
    pak_test_run(pak_vec_raw_test);
    pak_test_run(pak_vec_typesafe_test);
    pak_test_run(pak_vec_gc_test);

    return NULL;
}
