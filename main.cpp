#include <iostream>
#include <fstream>
#include "bitboard.hpp"
#include "move.hpp"
#include "attacks.hpp"
#include "main.hpp"

// chcp 65001
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1


int main()
{
    char FEN[] = "rnbqkbnr/ppp1pppp/8/3pP3/8/N7/PPPP1PPP/R1BQKBNR b KQkq d6 0 4";
    ChessEngine engine;
    engine.parse_FEN(FEN);
    engine.print_chessboard();

    engine.generate_moves(engine.chess_board);
}