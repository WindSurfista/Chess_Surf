#include <iostream>
#include <fstream>
#include <stack>
#include "bitboard.hpp"
#include "move.hpp"
#include "chess_engine.hpp"
#include "main.hpp"
#include "evaluate.hpp"

// chcp 65001
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1


int main()
{
    ChessEngine engine;
    engine.UCI_main_loop();

    char input[5000];
    
    strcpy(input, "position startpos");
    engine.UCI_get_position(input);
    print_chessboard(engine.chess_board);

    strcpy(input, "position startpos moves b1c3 g8f6");
    engine.UCI_get_position(input);
    print_chessboard(engine.chess_board);

    strcpy(input, "position startpos moves b1c3 g8f6 e2e3 e7e6");
    engine.UCI_get_position(input);
    print_chessboard(engine.chess_board);

    strcpy(input, "position startpos moves b1c3 g8f6 e2e3 e7e6 f1d3 f8d6");
    engine.UCI_get_position(input);
    print_chessboard(engine.chess_board);

    // std::stack<int> move_stack;
    // engine.generate_moves(&move_stack);
    // while(!move_stack.empty())
    // {
    //     int move = move_stack.top();
    //     move_stack.pop();
    //     Board backup(engine.chess_board);
    //     engine.make_move(move);
    //     print_move(move);
    //     std::cout << move << std::endl;
    //     print_chessboard(engine.chess_board);
    //     engine.chess_board = backup;
    //     std::getchar();
    // }
}


// position fen 7n/6K1/8/8/8/8/7k/6N1 w - - 0 1