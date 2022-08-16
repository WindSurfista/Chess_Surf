#pragma once
#include "bitboard.hpp"
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
1 0 0 0 0000 0000 000000 000000                enpassant_flag
0 1 0 0 0000 0000 000000 000000                castling_flag
0 0 1 0 0000 0000 000000 000000                double_flag
0 0 0 1 0000 0000 000000 000000                capture_flag
0 0 0 0 1111 0000 000000 000000                promotion_piece
0 0 0 0 0000 1111 000000 000000                piece
0 0 0 0 0000 0000 111111 000000                target
0 0 0 0 0000 0000 000000 111111                origin

1 1 1 1 0100 0110 111111 100100
*/

const char *index_to_square[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

void print_move(int move)
{
    std::string unicode_pieces[12] = {"♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};
    std::cout << "Move    Piece    Promotion    Capture_Flag    Double_flag    Castling_Flag    Enpassant_Flag" << std::endl;
    std::cout << index_to_square[move_origin(move)] << index_to_square[move_target(move)] << "    " << unicode_pieces[move_piece(move)]  <<  "        " << unicode_pieces[move_promotion(move)] << "            " << move_capture_flag(move) << "               " << move_double_flag(move) << "              " << move_castling_flag(move) << "                " << move_enpassant_flag(move) << std::endl;    
}

