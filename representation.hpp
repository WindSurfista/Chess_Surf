#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <memory.h>

#define U64 unsigned long long
#define check_bit(bitboard, index) ((bitboard >> index) & 1U)
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define clear_bit(bitboard, index) (bitboard &= ~(1ULL << index))

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

enum {P, N, B, R, Q, K, p, n, b, r, q, k}; // Capitals - White pieces, Lowercase - Black pieces

enum {wk = 1, wq = 2, bk = 4, bq = 8}; // Castling encoding: wk - white king side, bq - black queen side e.t.c.

struct Board
{
    bool side = white; // Which side is to play
    int enpassant = -1; // Square vulnerable to an enpassant
    int castle_rights = 0; // What castling is still available
    U64 piece_boards[12] = {}; // Bitboards for each piece type, marking where they are located
    U64 occupancies[4] = {}; // Tracks which squares are occupied by: Black, White, Both and Neither

    Board(const Board& chess_board)
    {
        side = chess_board.side;
        enpassant = chess_board.enpassant;
        castle_rights = chess_board.castle_rights;
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

// Possible optimisation: Experimental binary search alternative left in representation.cpp for future testing
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
