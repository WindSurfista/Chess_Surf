#include "move.hpp"

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
    std::cout << index_to_square[move_origin(move)] << index_to_square[move_target(move)] << "    " << unicode_pieces[move_piece(move)]  <<  "        " << unicode_pieces[move_promotion(move)] << "            "  << move_capture_flag(move) << "               " << move_double_flag(move) << "              " << move_castling_flag(move) << "                " << move_enpassant_flag(move) << std::endl;    
}