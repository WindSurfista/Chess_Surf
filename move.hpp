#pragma once
#include "representation.hpp"
#include <stack>

// might be better to handle moves as a struct
#define create_move(origin, target, piece, promotion, capture_flag, double_flag, castling_flag, enpassant_flag) \
    (origin) | (target << 6) | (piece << 12) | (promotion << 16) | (capture_flag << 20) | (double_flag << 21) | (castling_flag << 22) | (enpassant_flag << 23)

#define move_origin(move) (move & 0x3f)
#define move_target(move) ((move & 0xfc0) >> 6)
#define move_piece(move) ((move & 0xf000) >> 12)
#define move_promotion(move) ((move & 0xf0000) >> 16)
#define move_capture_flag(move) ((move & 0x100000) >> 20)
#define move_double_flag(move) ((move & 0x200000) >> 21)
#define move_castling_flag(move) ((move & 0x400000) >> 22)
#define move_enpassant_flag(move) ((move & 0x800000) >> 23)

/*
1 0 0 0 0000 0000 000000 000000            enpassant_flag
0 1 0 0 0000 0000 000000 000000            castling_flag
0 0 1 0 0000 0000 000000 000000            double_flag
0 0 0 1 0000 0000 000000 000000            capture_flag
0 0 0 0 1111 0000 000000 000000            promotion_piece
0 0 0 0 0000 1111 000000 000000            piece
0 0 0 0 0000 0000 111111 000000            target
0 0 0 0 0000 0000 000000 111111            origin

1 1 1 1 0100 0110 111111 100100
*/

extern const char *index_to_square[64];

void print_move(int move);
