#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <memory.h>

#define U64 unsigned long long
#define check_bit(bitboard, index) ((bitboard >> index) & 1U)
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define remove_bit(bitboard, index) (bitboard &= ~(1ULL << index))

// Todo: Remove the temporary constant variables
const U64 no_a_file = 18374403900871474942ULL;
const U64 no_h_file = 9187201950435737471ULL; 
const U64 no_ab_file = 18229723555195321596ULL;
const U64 no_gh_file = 4557430888798830399ULL;
const U64 no_border = 35605478574587392ULL;

enum {
a8,b8,c8,d8,e8,f8,g8,h8,
a7,b7,c7,d7,e7,f7,g7,h7,
a6,b6,c6,d6,e6,f6,g6,h6,
a5,b5,c5,d5,e5,f5,g5,h5,
a4,b4,c4,d4,e4,f4,g4,h4,
a3,b3,c3,d3,e3,f3,g3,h3,
a2,b2,c2,d2,e2,f2,g2,h2,
a1,b1,c1,d1,e1,f1,g1,h1
};

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

    Board(const Board& chess_board)
    {
        side = chess_board.side;
        enpassant = chess_board.enpassant;
        castle = chess_board.castle;
        memcpy(piece_boards, chess_board.piece_boards, sizeof(chess_board.piece_boards));
        memcpy(occupancies, chess_board.occupancies, sizeof(chess_board.occupancies));
    }

    Board() = default;
};



void print_bitboard(U64);

inline int hamming_weight(U64 bitboard)
{
    int count = 0;
    while (bitboard)
    {
        count++;
        bitboard &= bitboard - 1;
    }
    return count;
}

// Possible optimisation: Experimental binary search alternative left in cpp file for future testing
inline static int index_least_sig(U64 bitboard)
{
    if (bitboard)
    {
        int count = 0;
        bitboard = (bitboard &- bitboard); // Isolate least significant bit
        while (bitboard)
        {
            bitboard >>= 1;
            ++count;
        }
        return --count;
    }
    return -1; // Empty bitboard error code
}

