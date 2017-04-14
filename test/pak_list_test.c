#include "pak_test.h"
#include "pak_list_test.h"

#include <stdlib.h>
#include <pak.h>

PAK_INIT_LIST(IntList, int, NULL)

void list_gc(void *e)
{
    int *val = *(int **) e;
    free(val);
}
PAK_INIT_LIST(GCList, int*, list_gc)

char *pak_list_raw_test()
{
    static const int NUM_PUSHES = 100000;
    int i;

    pak_list *list = pak_list_new(int);
    pak_test_assert(list, "Failed to create PAK list");

    /*
       Pushing and popping
    */

    for (i = 0; i < NUM_PUSHES; i++)
        pak_list_push(list, i);

    for (i = 0; i < NUM_PUSHES; i++)
        pak_list_pop(list);

    pak_test_debug("Performed %d pushes and pops on PAK list.", NUM_PUSHES);

    /*
        Unshifting and shifting
    */

    for (i = 0; i < NUM_PUSHES; i++)
        pak_list_unshift(list, i);

    for (i = 0; i < NUM_PUSHES; i++)
        pak_list_shift(list);

    pak_test_debug("Performed %d unshifts and shifts on PAK list.", NUM_PUSHES);

    // Test to see if pak_list_free will clear out the list
    for (i = 0; i < NUM_PUSHES; i++)
        pak_list_push(list, i);

    pak_list_free(&list);
    pak_test_assert(!list, "List should be NULL after free.");

    return NULL;
}

char *pak_list_typesafe_test()
{
    static const int NUM_PUSHES = 100000;
    int i;

    IntList list = IntList_new();
    pak_test_assert(list, "Failed to create typesafe list.");

    /*
       Pushing and popping
    */

    for (i = 0; i < NUM_PUSHES; i++)
        IntList_push(list, i);

    // Test list traversal
    IntList_node n;
    pak_list_foreach(list, n) {
        int d = IntList_data(n);

        if (d == NUM_PUSHES/2)
            pak_test_debug("Hit list midpoint %d!", d);
    }

    for (i = 0; i < NUM_PUSHES; i++)
        IntList_pop(list);

    pak_test_debug("Performed %d pushes and pops on typesafe list.", NUM_PUSHES);

    /*
        Unshifting and shifting
    */

    for (i = 0; i < NUM_PUSHES; i++)
        IntList_unshift(list, i);

    for (i = 0; i < NUM_PUSHES; i++)
        IntList_shift(list);

    pak_test_debug("Performed %d unshifts and shifts on PAK list.", NUM_PUSHES);

    // Test to see if pak_list_free will clear out the list
    for (i = 0; i < NUM_PUSHES; i++)
        IntList_push(list, i);

    IntList_free(&list);
    pak_test_assert(!list, "List should be NULL after free.");

    return NULL;
}

char *pak_list_gc_test()
{
    static const int NUM_PUSHES = 1000;
    int i;

    GCList list = GCList_new();
    pak_test_assert(list, "Failed to create GC list.");

    for (i = 0; i < NUM_PUSHES; i++) {
        int *p = malloc(sizeof(*p));
        *p = i;

        GCList_push(list, p);
    }

    GCList_free(&list);

    return NULL;
}

char *pak_list_test()
{
    pak_test_run(pak_list_raw_test);
    pak_test_run(pak_list_typesafe_test);
    pak_test_run(pak_list_gc_test);

    return NULL;
}
