#pragma once
#include "bitboard.hpp"
#include "magic.hpp"
#include "main.hpp"

enum {white, black, all, neither};

enum {P, N, B, R, Q, K, p, n, b, r, q, k}; // Capital - White pieces, Lower - Black pieces

enum {wk = 1, wq = 2, bk = 4, bq = 8}; // Castling encoding

struct Board
{
    bool side = white;
    int enpassant = -1;
    int castle = 0;
    U64 piece_boards[12] = {};
    U64 occupancies[4] = {};
};

class ChessEngine : public MagicMove
{
private:
    U64 pawn_attacks[2][64];
    U64 knight_attacks[64];
    U64 king_attacks[64];

    U64 pawn_mask(int color, int index);
    U64 knight_mask(int index);
    U64 king_mask(int index);

public:
    Board chess_board;
    MagicMove magic;
    ChessEngine();
    void parse_FEN(char *FEN);
    void print_chessboard();
    bool check_if_attacked(Board chess_board, bool side, int index);
    
    void generate_moves(Board generation_board);
};
