#include "pak_test.h"
#include "pak_dict_test.h"
#include <pak.h>

char *pak_dict_raw_test()
{
    static const int NUM_VALS = 4;
    static const char *keys[] = { "Apple", "Orange", "Peach", "Grapes"};
    static const int vals[] = { 1, 2, 3, 4 };

    pak_dict *dict = pak_dict_new(int, 100);
    pak_test_assert(dict, "Failed to create PAK dictionary.");

    int i;
    for (i = 0; i < NUM_VALS; i++)
        pak_dict_add(dict, keys[i], vals[i]);

    return NULL;
}

char *pak_dict_test()
{
    pak_test_run(pak_dict_raw_test);

    return NULL;
}
