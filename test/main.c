#include "pak_test.h"

#define PAK_IMPLEMENTATION
#include <pak.h>

#include "pak_list_test.h"
#include "pak_arr_test.h"

int main()
{
    pak_test_init();

    pak_test_begin(pak_arr_test);
    pak_test_begin(pak_list_test);

    pak_test_exit();
}
