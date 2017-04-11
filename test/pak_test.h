#ifndef _pak_test_h
#define _pak_test_h

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define pak_test_debug(M_MSG, ...)\
	fprintf(stderr, "DEBUG %s: " M_MSG ".\n", __FILE__, ##__VA_ARGS__)

#define pak_test_print(M_MSG, ...)\
	fprintf(stderr, "PAK_TEST " M_MSG ".\n", ##__VA_ARGS__)

#define pak_test_assert(M_TEST, M_MSG, ...)						\
	if (!(M_TEST)) {											\
		pak_test_print("ERROR: " M_MSG " ", ##__VA_ARGS__);		\
		return M_MSG;											\
	}

#define pak_test_run(M_TEST_FUNC)									\
	do {															\
		char *pakt__msg = M_TEST_FUNC();							\
		if (pakt__msg) {											\
			pak_test_print("FAILED %s", #M_TEST_FUNC); 				\
			return pakt__msg;										\
		}															\
	} while (0);

#define pak_test_init()									\
	int pakt__tests_run = 0;							\
	clock_t pakt__clock_begin = clock();				\
	pak_test_print("STARTING TESTS...");

#define pak_test_begin(M_TEST)															\
	do {																				\
		pak_test_print("%d: Starting test %s.",											\
				pakt__tests_run, #M_TEST);												\
		char *pakt__msg = M_TEST();														\
		double pakt__elapsed = (double)(clock() - pakt__clock_begin)/CLOCKS_PER_SEC;	\
		if (!pakt__msg) {																\
			pak_test_print("%d: TEST SUCCESS: %s. ELAPSED %lf sec.",					\
					pakt__tests_run, #M_TEST, pakt__elapsed);							\
			pakt__tests_run++;															\
		} else {																		\
			pak_test_print("%d: TEST FAILED: %s.",										\
					pakt__tests_run, #M_TEST);											\
			return -1;																	\
		}																				\
	} while(0);

#define pak_test_exit()														\
	pak_test_print(": ALL %d TEST(S) HAVE SUCCEEDED.", pakt__tests_run);	\
	return 0;

#ifdef __cplusplus
}
#endif

#endif /* _pak_test_h */
