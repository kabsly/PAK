#include "pak_test.h"
#include "pak_alg_test.h"
#include <stddef.h> // NULL

#define PAK_ALG_IMPLEMENTATION
#include <pak_alg.h>

char *pak_alg_vec3_test()
{
    vec3 a = { 1, 8, 3 };
    vec3 b = { 5, 2, 9 };
    vec3 tmp;

    printf("Vec A: "); vec3_print(&a); puts(".");
    printf("Vec B: "); vec3_print(&b); puts(".");

    float mag_a = vec3_mag(&a);
    float mag_b = vec3_mag(&b);

    printf("A mag: %f.\n", mag_a);
    printf("B mag: %f.\n", mag_b);

    float dist = vec3_dist(&a, &b);
    printf("Dist (A -> B): %f.\n", dist);

    float dot = vec3_dot(&a, &b);
    printf("Dot (A * B): %f.\n", dot);

    vec3_cross(&tmp, &a, &b);
    printf("Cross (A x B): "); vec3_print(&tmp); puts(".");

    vec3_add(&tmp, &a, &b);
    printf("A + B: "); vec3_print(&tmp); puts(".");

    vec3_sub(&tmp, &a, &b);
    printf("A - B: "); vec3_print(&tmp); puts(".");

    vec3_norm(&tmp, &a);
    printf("A unit: "); vec3_print(&tmp); puts(".");

    vec3_lerp(&tmp, &tmp, &a, 1);
    printf("A unit (lerp): "); vec3_print(&tmp); puts(".");

    vec3_norm(&tmp, &b);
    printf("B unit: "); vec3_print(&tmp); puts(".");

    vec3_lerp(&tmp, &tmp, &b, 1);
    printf("B unit (lerp): "); vec3_print(&tmp); puts(".");

    return NULL;
}

char *pak_alg_mat3_test()
{
    vec3 v = { 2, 2, 1 };
    mat3 m = {{-1, 0, 0 },
              { 0, 1, 0 },
              { 0, 0, 0 }};

    mat3_transform3(m, &v);
    printf("vec3 after tranform: "); vec3_print(&v); puts(".");

    vec3_rotate_x(&v, 90);
    printf("vec3 after rotation (x, 90): "); vec3_print(&v); puts(".");

    vec3_rotate_y(&v, 90);
    printf("vec3 after rotation (y, 90): "); vec3_print(&v); puts(".");

    vec3_rotate_z(&v, 90);
    printf("vec3 after rotation (z, 90): "); vec3_print(&v); puts(".");
    
    return NULL;
}

char *pak_alg_test()
{
    // Add newlines to seperate linear algebra data from
    // other unit tests
    puts("");

    pak_test_run(pak_alg_vec3_test);
    pak_test_run(pak_alg_mat3_test);

    puts("");

    return NULL;
}
