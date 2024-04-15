//
// Created by joe on 4/12/24.
//

#ifndef CONNECT4C_ENGINE_H
#define CONNECT4C_ENGINE_H

#include <stdbool.h>

#define DATABASE_DEPTH 15
#define WORST_EVAL (-18)
#define BEST_EVAL 18
#define DRAW 0
#define MAX_TOTAL_MOVES 42
#define MAX_PLAYER_MOVES 21
#define MAX_HEIGHT 6
#define SIZE 524289
#define MOVE_ORDER (3 + (2 << 4) + (4 << 8) + (5 << 12) + (1 << 16) + (6 << 20))
#define i8 signed char

int evaluatePosition(long state, int piece, int alpha, int beta, int movesMade, i8 *lowerCache0, i8 *upperCache0,
                     long *lowerCache1, long *upperCache1, int *lower_values, int *upper_values,
                     const int *pos_sums, int **nCr, int **combo_sums, long* pos_count);

#endif
