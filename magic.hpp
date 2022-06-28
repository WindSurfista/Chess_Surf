#pragma once
#include "bitboard.hpp"

class MagicMove
{
    U64 bishop_masks[64];
    U64 rook_masks[64];

    U64 bishop_magic[64];
    U64 rook_magic[64];

    int bishop_shift[64];
    int rook_shift[64];

    U64 *bishop_htable[64];
    U64 *rook_htable[64];

    private:
    // Piece_mask: A bitboard of a piece's movement pattern on a blank board
    void init_bishop_masks();
    void init_rook_masks();

    // Piece_block: A bitboard representing occupied squares which might prevent a piece's movement to the edge of the board
    void init_bishop_blocks(U64 **bishop_blocks);
    void init_rook_blocks(U64 **rook_blocks);

    // Piece_move: A bitboard of a piece's available movement patterns given a certain board
    void init_bishop_moves(U64 **bishop_blocks, U64 **init_bishop_moves);
    void init_rook_moves(U64 **rook_blocks, U64 **rook_moves);

    // Piece_magic: Initializes the piece's magic hash table
    void init_rook_magic();
    void init_bishop_magic();

    public:
    MagicMove();
    U64 get_bishop(int index, U64 occupancy);
    U64 get_rook(int index, U64 occupancy);
    U64 get_queen(int index, U64 occupancy);
};
