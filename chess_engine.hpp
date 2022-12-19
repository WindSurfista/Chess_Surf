#pragma once
#include <stack>
#include "representation.hpp"
#include "move_gen.hpp"
#include "main.hpp"
#include <map>

void print_chessboard(Board chess_board);

class ChessEngine : public MoveGen
{
    private:
    MoveGen move_gen;
    int default_search_depth;
    int best_move;
    bool endgame_flag = false;
    int mg_table[12][64];
    int eg_table[12][64];

    bool check_if_attacked(int index);
    bool make_move(int move);
    int minimax(int depth, int alpha, int beta);
    
    void UCI_send_move(int move);
    int UCI_get_move(char* command_ptr);

    int static_eval();
public:
    ChessEngine(int search_depth);
    Board chess_board;
    void load_FEN(char *FEN);
    void UCI_get_position(char* command_ptr);
    void UCI_go(char* command_ptr);
    void UCI_main_loop();
    void find_moves(std::stack<int>* move_stack);
    void search();
};
