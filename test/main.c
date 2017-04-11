#include "pak_test.h"
#include "pak_vec_test.h"

int main()
{
	pak_test_init();

    pak_test_begin(pak_vec_test);

	pak_test_exit();
}
