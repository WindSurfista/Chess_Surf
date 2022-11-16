#pragma once
#include <stack>
#include "bitboard.hpp"
#include "magic.hpp"
#include "main.hpp"
#include "evaluate.hpp"
#include <map>

void print_chessboard(Board chess_board);

class ChessEngine : public MagicMove
{
    private:
    U64 pawn_attacks[2][64];
    U64 knight_attacks[64];
    U64 king_attacks[64];

    U64 pawn_mask(int color, int index);
    U64 knight_mask(int index);
    U64 king_mask(int index);

    MagicMove slide_move_gen;

    int search_depth = 4;

    bool check_if_attacked(int index);
    bool make_move(int move);
    int minimax(int depth, int alpha, int beta);
    
    void UCI_send_move(int move);
    int UCI_get_move(char* command_ptr);

public:
    ChessEngine();
    Board chess_board;
    
    int ply;
    int best_move;

    void load_FEN(char *FEN);
    void UCI_get_position(char* command_ptr);
    void UCI_go(char* command_ptr);
    void UCI_main_loop();
    void generate_moves(std::stack<int>* move_stack);
    void search();
};
