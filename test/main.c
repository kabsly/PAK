#include "pak_test.h"

#include "pak_list_test.h"
#include "pak_vec_test.h"
#include "pak_alg_test.h"

int main()
{
    pak_test_init();

    pak_test_begin(pak_vec_test);
    pak_test_begin(pak_alg_test);
    pak_test_begin(pak_list_test);

    pak_test_exit();
}
