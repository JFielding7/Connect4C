//
// Created by joe on 4/12/24.
//

#ifndef CONNECT4C_DATABASE_H
#define CONNECT4C_DATABASE_H

#define COLUMNS 7
#define MAX_HEIGHT 6

int get_index(long state, int pieces, const int *pos_sums, int **nCr, int **combo_sums);

int** pascals_triangle(int n);

int combos(int pieces, int cols, int height, int ***combos_cache);

int** combo_sums(int n, int ***combos_cache);

int positions_count(int pieces, int **nCr, int ***combos_cache);

int* positions_sums(int n, int **nCr, int ***combos_cache);

#endif //CONNECT4C_DATABASE_H
