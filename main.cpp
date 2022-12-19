#include <iostream>
#include <fstream>
#include <stack>
#include "representation.hpp"
#include "move.hpp"
#include "chess_engine.hpp"
#include "main.hpp"

// chcp 65001
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

// const U64 no_a_file = 18374403900871474942ULL;
// const U64 no_h_file = 9187201950435737471ULL; 
// const U64 no_ab_file = 18229723555195321596ULL;
// const U64 no_gh_file = 4557430888798830399ULL;
// const U64 no_border = 35605478574587392ULL;


int main(int argc, char** argv)
{
    int thing[10];
    std::cout << thing[18];

    if (argc >= 3)
    {
        throw std::runtime_error(std::string("Error: Too many arguments passed to exe"));
    }

    int search_depth;
    if (argc == 2)
    {
        search_depth = atoi(argv[1]);
    }
    else
    {
        search_depth = 4;
    }

    ChessEngine engine(search_depth);
    engine.UCI_main_loop();

    char input[5000];
    
    strcpy(input, "position startpos moves b1c3 g8f6 e2e3 e7e6 f1d3 f8d6");
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
    // engine.find_moves(&move_stack);
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