//
// Created by joe on 4/10/24.
//
#include <malloc.h>
#include "engine.h"
#include "database.h"
#include <time.h>

int main() {
    int depth = DATABASE_DEPTH;
    int **nCr = pascals_triangle(depth);

    int ***combos_cache = malloc(sizeof(int**) * (depth + 1));
    for (int i = 0; i < (depth + 1); i++) {
        combos_cache[i] = malloc(sizeof (int*) * (COLUMNS + 1));
        for (int j = 0; j < (COLUMNS + 1); j++) {
            combos_cache[i][j] = malloc(sizeof (int) * COLUMNS);
            for (int k = 0; k < COLUMNS; k++) {
                combos_cache[i][j][k] = 0;
            }
        }
    }
    int *pos_sum = positions_sums(depth, nCr, combos_cache);
    int **combos_sums = combo_sums(depth, combos_cache);

    unsigned long cache0_size = pos_sum[depth];
    i8 *lowerCache0 = malloc(cache0_size);
    i8 *upperCache0 = malloc(cache0_size);
    for (unsigned long i = 0; i < cache0_size; i++) {
        lowerCache0[i] = WORST_EVAL;
        upperCache0[i] = BEST_EVAL;
    }

    long *lowerCache1 = malloc(sizeof (long) * SIZE);
    long *upperCache1 = malloc(sizeof (long) * SIZE);
    int *lower_values = malloc(sizeof (int) * SIZE);
    int *upper_values = malloc(sizeof (int) * SIZE);

//    long lowerCache1[SIZE];
//    long upperCache1[SIZE];
//    int lower_values[SIZE];
//    int upper_values[SIZE];

    clock_t begin = clock();
    long val = 0;
    long *pos_count = &val;
    printf("%d\n", evaluatePosition(0, 1, WORST_EVAL, BEST_EVAL, 0,
                                    lowerCache0, upperCache0, lowerCache1, upperCache1, lower_values, upper_values,
                                    pos_sum, nCr, combos_sums, pos_count));
    printf("Pos: %ld\n", *pos_count);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("%f\n", time_spent);

    free(lowerCache0);
    free(upperCache0);
    free(lowerCache1);
    free(upperCache1);
    free(lower_values);
    free(upper_values);
    free(pos_sum);
    free(combos_cache);
    free(nCr);
    return 0;
}

