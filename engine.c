//
// Created by joe on 4/10/24.
//

#include "engine.h"
#include "database.h"
#define MAX_CACHE_DEPTH 42

int max(int arg0, int arg1) {
    return arg0 > arg1 ? arg0 : arg1;
}

int min(int arg0, int arg1) {
    return arg0 < arg1 ? arg0 : arg1;
}

long next_state(long state, int piece, int col, int height){
    if(piece == 1) state += 1L << (col * 6 + height);
    return state + (1L << (42 + col * 3));
}

long reflectState(long state) {
    long reflected = 0;
    for (int col = 0; col < 7; col++) {
        reflected += ((state >> col * 6 & 0b111111) << (6 - col) * 6) + ((state >> (MAX_TOTAL_MOVES + col * 3) & 0b111) << (MAX_TOTAL_MOVES + (6 - col) * 3));
    }
    return reflected;
}

int sortByThreats(int order, int threats) {
    for (int i = 4; i < 28; i += 4) {
        int j = i, currThreats = (threats >> (order >> i & 0b1111) * 4 & 0b1111);
        while (j > 0 && currThreats > (threats >> (order >> (j - 4) & 0b1111) * 4 & 0b1111)) {
            j -= 4;
        }
        order = (order & (1 << j) - 1) + ((order >> i & 0b1111) << j) + ((order >> j & (1 << (i - j)) - 1) << (j + 4)) + (order >> (i + 4) << (i + 4));
    }
    return order;
}

long getPieceLocations(long state, int piece) {
    long board = 0;
    if (piece == 1) {
        for (int i = 0; i < 7; i++) {
            board += (state >> (6 * i) & 0b111111) << (7 * i);
        }
    }
    else {
        for (int i = 0; i < 7; i++) {
            board += ((state >> (6 * i) & 0b111111) ^ ((1 << (state >> (42 + i * 3) & 0b111)) - 1)) << (7 * i);
        }
    }
    return board;
}

bool is_win(long pieceLocations) {
    for (int i = 1; i < 9; i += 1 / i * 4 + 1) {
        long connections = pieceLocations;
        for (int j = 0; j < 3; j++) connections = connections & (connections >> i);
        if (connections != 0) return true;
    }
    return false;
}

bool is_winning(long state, int piece) {
    return is_win(getPieceLocations(state, piece));
}

int count_threats(long state, long pieceLocations, int piece) {
    int threatCount = 0;
    for (int col = 0; col < 7; col++) {
        for (int row = (int) (state >> (42 + col * 3) & 0b111); row < 6; row++) {
            if (is_win(pieceLocations + (1L << (col * 7 + row)))) threatCount += 1 + ((row & 1) ^ piece);
        }
    }
    return threatCount;
}

int evaluatePosition(long state, int piece, int alpha, int beta, int movesMade, i8 *lowerCache0, i8 *upperCache0, // NOLINT(*-no-recursion)
                     long *lowerCache1, long *upperCache1, int *lower_values, int *upper_values,
                     const int *pos_sums, int **nCr, int **combo_sums, long *pos_count) {
    (*pos_count)++;
    if (movesMade == MAX_TOTAL_MOVES) return DRAW;
    alpha = max(alpha, ((movesMade + 1) >> 1) - MAX_PLAYER_MOVES);
    beta = min(beta, MAX_PLAYER_MOVES - (movesMade >> 1));
    int index;
    if (movesMade > DATABASE_DEPTH && movesMade < MAX_CACHE_DEPTH) {
        index = (int) (state % SIZE);
        if (lowerCache1[index] == state) alpha = max(alpha, lower_values[index]);
        if (upperCache1[index] == state) beta = min(beta, upper_values[index]);
    }
    else if (movesMade <= DATABASE_DEPTH) {
        index = get_index(state, movesMade, pos_sums, nCr, combo_sums);
        alpha = max(alpha, lowerCache0[index]);
        beta = min(beta, upperCache0[index]);
    }
    if (alpha >= beta) return alpha;
    int threats = 0, order = MOVE_ORDER, forcedMoves = 0;
    long forcedMove = -1;
    for (int i = 0; i < 7; i++) {
        int col = (MOVE_ORDER >> (i * 4)) & 0b1111;
        int height = (int) ((state >> (MAX_TOTAL_MOVES + col * 3)) & 0b111);
        if (height != MAX_HEIGHT) {
            long move = next_state(state, piece, col, height);
            long pieceLocations = getPieceLocations(move, piece);
            if (is_win(pieceLocations)) return MAX_PLAYER_MOVES - (movesMade >> 1);
            if (is_winning(next_state(state, piece ^ 1, col, height), piece ^ 1)) {
                forcedMoves++;
                forcedMove = move;
            }
            if (movesMade < DATABASE_DEPTH) alpha = max(alpha, -upperCache0[get_index(move, movesMade + 1, pos_sums, nCr, combo_sums)]);
            else if (movesMade < MAX_CACHE_DEPTH) {
                int moveIndex = (int) (move % SIZE);
                if (upperCache1[moveIndex] == move) alpha = max(alpha, -upper_values[moveIndex]);
            }
            if (alpha >= beta) return alpha;
            threats += count_threats(move, pieceLocations, piece) << col * 4;
        }
    }
    if (forcedMoves > 0) return forcedMoves > 1 ? ((movesMade + 1) >> 1) - MAX_PLAYER_MOVES :
        -evaluatePosition(forcedMove, piece ^ 1, -beta, -alpha, movesMade + 1,
                          lowerCache0, upperCache0, lowerCache1, upperCache1, lower_values, upper_values,
                          pos_sums, nCr, combo_sums, pos_count);
    order = sortByThreats(order, threats);
    int i = 0;
    for (int j = 0; j < 28; j += 4) {
        int col = order >> j & 0b1111;
        int height = (int) (state >> (42 + col * 3) & 0b111);
        if (height != MAX_HEIGHT) {
            long move = next_state(state, piece, col, height);
            int eval;
            if (i++ == 0) eval = -evaluatePosition(move, piece ^ 1, -beta, -alpha, movesMade + 1,
                                                   lowerCache0, upperCache0, lowerCache1, upperCache1, lower_values, upper_values,
                                                   pos_sums, nCr, combo_sums, pos_count);
            else {
                eval = -evaluatePosition(move, piece ^ 1, -alpha - 1, -alpha, movesMade + 1,
                                         lowerCache0, upperCache0, lowerCache1, upperCache1, lower_values, upper_values,
                                         pos_sums, nCr, combo_sums, pos_count);
                if (eval > alpha && eval < beta) eval = -evaluatePosition(move, piece ^ 1, -beta, -alpha, movesMade + 1,
                                                                          lowerCache0, upperCache0, lowerCache1, upperCache1, lower_values, upper_values,
                                                                          pos_sums, nCr, combo_sums, pos_count);
            }
            alpha = max(alpha, eval);
            if (alpha >= beta) {
                if (movesMade > DATABASE_DEPTH && movesMade < MAX_CACHE_DEPTH) {
                    lowerCache1[index] = state;
                    lower_values[index] = alpha;
                }
                else if (movesMade <= DATABASE_DEPTH) {
                    lowerCache0[index] = (i8) alpha;
                    int idx = get_index(reflectState(state), movesMade, pos_sums, nCr, combo_sums);
                    lowerCache0[idx] = (i8) max(alpha, lowerCache0[idx]);
                }
                return alpha;
            }
        }
    }
    if (movesMade > DATABASE_DEPTH && movesMade < MAX_CACHE_DEPTH) {
        upperCache1[index] = state;
        upper_values[index] = alpha;
    }
    else if (movesMade <= DATABASE_DEPTH) {
        upperCache0[index] = (i8) alpha;
        int idx = get_index(reflectState(state), movesMade, pos_sums, nCr, combo_sums);
        upperCache0[idx] = (i8) min(alpha, upperCache0[idx]);
    }
    return alpha;
}
