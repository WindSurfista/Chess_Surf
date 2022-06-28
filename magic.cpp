#include "main.hpp"
#include "bitboard.hpp"
#include "magic.hpp"
#include <iostream>
#include <fstream>
#include <string>

void MagicMove::init_bishop_masks()
{
    for (int index = 0; index < 64; index++)
    {
        U64 *bishop_mask = &bishop_masks[index];
        *bishop_mask = 0ULL;

        int rank = index / 8;
        int file = index % 8;

        for (int move_rank = rank + 1, move_file = file + 1; move_rank < 7 & move_file < 7; move_rank++, move_file++)
            set_bit(*bishop_mask, move_rank * 8 + move_file);
        for (int move_rank = rank + 1, move_file = file - 1; move_rank<7 & move_file> 0; move_rank++, move_file--)
            set_bit(*bishop_mask, move_rank * 8 + move_file);
        for (int move_rank = rank - 1, move_file = file + 1; move_rank > 0 & move_file < 7; move_rank--, move_file++)
            set_bit(*bishop_mask, move_rank * 8 + move_file);
        for (int move_rank = rank - 1, move_file = file - 1; move_rank > 0 & move_file > 0; move_rank--, move_file--)
            set_bit(*bishop_mask, move_rank * 8 + move_file);
    }
}

void MagicMove::init_rook_masks()
{
    for (int index = 0; index < 64; index++)
    {
        U64 *rook_mask = &rook_masks[index];
        *rook_mask = 0ULL;

        int rank = index / 8;
        int file = index % 8;

        for (int move_rank = rank + 1; move_rank < 7; ++move_rank)
            set_bit(*rook_mask, move_rank * 8 + file);
        for (int move_rank = rank - 1; move_rank > 0; --move_rank)
            set_bit(*rook_mask, move_rank * 8 + file);
        for (int move_file = file + 1; move_file < 7; ++move_file)
            set_bit(*rook_mask, rank * 8 + move_file);
        for (int move_file = file - 1; move_file > 0; --move_file)
            set_bit(*rook_mask, rank * 8 + move_file);
    }
}

void MagicMove::init_bishop_blocks(U64 **bishop_blocks)
{
    for (int index = 0; index < 64; index++)
    {
        bishop_blocks[index] = new U64[512]{0};
        for (int seed = 0; seed < 512; seed++)
        {
            U64 move_mask = bishop_masks[index];
            int size = hamming_weight(move_mask);

            for (int count = 0; count < move_mask; count++)
            {
                int bit_index = index_least_sig(move_mask);
                remove_bit(move_mask, bit_index);
                if (seed & (1 << count))
                    set_bit(bishop_blocks[index][seed], bit_index);
            }
        }
    }
}

void MagicMove::init_rook_blocks(U64 **rook_blocks)
{
    for (int index = 0; index < 64; index++)
    {
        rook_blocks[index] = new U64[4096]{0};
        for (int seed = 0; seed < 4096; seed++)
        {
            U64 move_mask = rook_masks[index];
            int size = hamming_weight(move_mask);

            for (int count = 0; count < move_mask; count++)
            {
                int bit_index = index_least_sig(move_mask);
                remove_bit(move_mask, bit_index);
                if (seed & (1 << count))
                    set_bit(rook_blocks[index][seed], bit_index);
            }
        }
    }
}

void MagicMove::init_bishop_moves(U64 **bishop_blocks, U64 **bishop_moves)
{
    for (int index = 0; index < 64; index++)
    {
        int rank = index / 8;
        int file = index % 8;
        bishop_moves[index] = new U64[512]{0};

        for (int block_num = 0; block_num < 512; block_num++)
        {
            U64 bishop_block = bishop_blocks[index][block_num];

            for (int move_rank = rank + 1, move_file = file + 1; move_rank < 8 & move_file < 8; move_rank++, move_file++)
            {
                int move_index = move_rank * 8 + move_file;
                set_bit(bishop_moves[index][block_num], move_index);
                if check_bit (bishop_block, move_index)
                    break;
            }

            for (int move_rank = rank + 1, move_file = file - 1; move_rank < 8 & move_file >= 0; move_rank++, move_file--)
            {
                int move_index = move_rank * 8 + move_file;
                set_bit(bishop_moves[index][block_num], move_index);
                if check_bit (bishop_block, move_index)
                    break;
            }

            for (int move_rank = rank - 1, move_file = file + 1; move_rank >= 0 & move_file < 8; move_rank--, move_file++)
            {
                int move_index = move_rank * 8 + move_file;
                set_bit(bishop_moves[index][block_num], move_index);
                if check_bit (bishop_block, move_index)
                    break;
            }

            for (int move_rank = rank - 1, move_file = file - 1; move_rank >= 0 & move_file >= 0; move_rank--, move_file--)
            {
                int move_index = move_rank * 8 + move_file;
                set_bit(bishop_moves[index][block_num], move_index);
                if check_bit (bishop_block, move_index)
                    break;
            }
        }
    }
}

void MagicMove::init_rook_moves(U64 **rook_blocks, U64 **rook_moves)
{
    for (int index = 0; index < 64; index++)
    {
        int rank = index / 8;
        int file = index % 8;
        rook_moves[index] = new U64[4096]{0};

        for (int block_num = 0; block_num < 4096; block_num++)
        {
            U64 rook_block = rook_blocks[index][block_num];

            for (int move_rank = rank + 1; move_rank < 8; ++move_rank)
            {
                int move_index = move_rank * 8 + file;
                set_bit(rook_moves[index][block_num], move_index);
                if (check_bit(rook_block, move_index))
                    break;
            }
            for (int move_rank = rank - 1; move_rank >= 0; --move_rank)
            {
                int move_index = move_rank * 8 + file;
                set_bit(rook_moves[index][block_num], move_index);
                if (check_bit(rook_block, move_index))
                    break;
            }
            for (int move_file = file + 1; move_file < 8; ++move_file)
            {
                int move_index = rank * 8 + move_file;
                set_bit(rook_moves[index][block_num], move_index);
                if (check_bit(rook_block, move_index))
                    break;
            }
            for (int move_file = file - 1; move_file >= 0; --move_file)
            {
                int move_index = rank * 8 + move_file;
                set_bit(rook_moves[index][block_num], move_index);
                if (check_bit(rook_block, move_index))
                    break;
            }
        }
    }
}

void MagicMove::init_bishop_magic()
{
    U64 *bishop_blocks[64];
    init_bishop_blocks(bishop_blocks);
    U64 *bishop_moves[64];
    init_bishop_moves(bishop_blocks, bishop_moves);

    std::ifstream magic_file("magic_numbers/bishop_magics.txt");
    std::string magic_string;

    for (int index = 0; index < 64; index++)
    {
        std::getline(magic_file, magic_string, ',');
        bishop_magic[index] = stoull(magic_string);
    }

    for (int index = 0; index < 64; index++)
    {
        std::getline(magic_file, magic_string, ',');
        bishop_shift[index] = stoi(magic_string);
    }

    for (int index = 0; index < 64; index++)
    {
        U64 magic = bishop_magic[index];
        int shift = bishop_shift[index];
        bishop_htable[index] = new U64[1 << (64-shift)]{0};

        for (int i = 0; i < 512; i++)
        {
            int magic_hash = bishop_blocks[index][i] * magic >> shift;
            bishop_htable[index][magic_hash] = bishop_moves[index][i]; // Collisions do not result in loss of information as the exact same move board is saved in the same place, thanks to the magic numbers.
        }
    }
}

void MagicMove::init_rook_magic()
{
    U64 *rook_blocks[64];
    init_rook_blocks(rook_blocks);
    U64 *rook_moves[64];
    init_rook_moves(rook_blocks, rook_moves);

    std::ifstream magic_file("magic_numbers/rook_magics.txt");
    std::string magic_string;

    for (int index = 0; index < 64; index++)
    {
        std::getline(magic_file, magic_string, ',');
        rook_magic[index] = std::stoull(magic_string);
    }

    for (int index = 0; index < 64; index++)
    {
        std::getline(magic_file, magic_string, ',');
        rook_shift[index] = std::stoi(magic_string);
    }

    for (int index = 0; index < 64; index++)
    {
        U64 magic = rook_magic[index];
        int shift = rook_shift[index];
        rook_htable[index] = new U64[1 << (64 - shift)]{0};

        for (int i = 0; i < 4096; i++)
        {
            int magic_hash = rook_blocks[index][i] * magic >> shift;
            rook_htable[index][magic_hash] = rook_moves[index][i]; // Collisions do not result in loss of information as the exact same move board is saved in the same place, thanks to the magic numbers.
        }
    }
}

MagicMove::MagicMove()
{
    init_bishop_masks();
    init_rook_masks();
    init_bishop_magic();
    init_rook_magic();
}

U64 MagicMove::get_bishop(int index, U64 occupancy)
{
    return bishop_htable[index][((occupancy & bishop_masks[index]) * (bishop_magic[index])) >> bishop_shift[index]];
}

U64 MagicMove::get_rook(int index, U64 occupancy)
{
    return rook_htable[index][((occupancy & rook_masks[index]) * (rook_magic[index])) >> rook_shift[index]];
}

U64 MagicMove::get_queen(int index, U64 occupancy)
{
    return get_bishop(index, occupancy) | get_rook(index, occupancy);
}
