//
// Created by joe on 4/12/24.
//
#include <malloc.h>
#include "database.h"

int get_index(long state, int pieces, const int *pos_sums, int **nCr, int **combo_sums) {
    if (pieces == 0) return 0;
    int index = pos_sums[pieces - 1];
    int combos = nCr[pieces][pieces >> 1];
    int p2Pieces = pieces >> 1, p1Pieces = pieces - p2Pieces;
    for (int col = 0; col < 7 && pieces > 0; col++) {
        int height = (int) (state >> (42 + col * 3) & 0b111);
        index += (combo_sums[6 - col][pieces] - combo_sums[6 - col][pieces - height]) * combos;
        for (int row = 0; row < height && p1Pieces > 0 && p2Pieces > 0; row++) {
            if ((state >> (col * 6 + row) & 1) == 1) {
                index += nCr[pieces - row - 1][p1Pieces];
                p1Pieces--;
            }
            else p2Pieces--;
        }
        pieces -= height;
    }
    return index;
}

int** pascals_triangle(int n) {
    int **pascalsTriangle = malloc(sizeof (int*) * (n + 1));
    pascalsTriangle[0] = malloc(sizeof (int));
    pascalsTriangle[0][0] = 1;
    for (int i = 1; i <= n; i++) {
        int *prevRow = pascalsTriangle[i - 1];
        int *row = pascalsTriangle[i] = malloc(sizeof (int) * (i + 1));
        row[0] = 1;
        row[i] = 1;
        for (int j = 1; j < i; j++) {
            row[j] = prevRow[j - 1] + prevRow[j];
        }
    }
    return pascalsTriangle;
}

int combos(int pieces, int cols, int height, int ***combos_cache) { // NOLINT(*-no-recursion)
    if (pieces == 0) return 1;
    if (cols == 0 && pieces > 0) return 0;
    if (combos_cache[pieces][cols][height]) return combos_cache[pieces][cols][height];
    int total = combos(pieces, cols - 1, 0, combos_cache);
    if (height < MAX_HEIGHT) total += combos(pieces - 1, cols, height + 1, combos_cache);
    combos_cache[pieces][cols][height] = total;
    return total;
}

int** combo_sums(int n, int ***combos_cache) {
    int **sums = malloc((COLUMNS + 1) * (n + 1));
    for (int cols = 0; cols <= COLUMNS; cols++) {
        int *col_sums = sums[cols] = malloc(sizeof(int) * (n + 1));
        col_sums[0] = 1;
        for (int pieces = 1; pieces <= n; pieces++) {
            col_sums[pieces] = combos(pieces, cols, 0, combos_cache) + col_sums[pieces - 1];
        }
    }
    return sums;
}

int positions_count(int pieces, int **nCr, int ***combos_cache) {
    int sum = 0;
    for (int i = 0; i <= pieces; i++) {
        sum += combos(i, COLUMNS, 0, combos_cache) * nCr[i][i >> 1];
    }
    return sum;
}

int* positions_sums(int n, int **nCr, int ***combos_cache) {
    int *counts = malloc(sizeof (int) * (n + 1));
    for (int i = 0; i <= n; i++) {
        counts[i] = positions_count(i, nCr, combos_cache);
    }
    return counts;
}
