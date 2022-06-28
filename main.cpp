#include <iostream>
#include <fstream>
#include "bitboard.hpp"
#include "attacks.hpp"
#include "main.hpp"

// chcp 65001
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

int main()
{
    char FEN[] = "rnbqkbnr/PPPPPPPP/8/8/8/8/pppppppp/RNBQKBNR w KQkq - 0 1";
    ChessEngine engine;
    engine.parse_FEN(FEN);
    engine.print_chessboard();
    engine.generate_moves(white);
}
