#include <iostream>
#include "bitboard.hpp"

void print_bitboard(U64 bitboard)
{
    std::cout << std::endl;
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int index = (rank * 8) + file;

            if (file == 0)
            {
                std::cout << 8 - rank << "  ⎹";
            }
            std::cout << ' ' << (((1ULL << index) & bitboard) ? 1 : 0) << std::flush;
        }
        std::cout << std::endl;
    }
    std::cout << "    ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
    std::cout << "     A B C D E F G H " << std::endl;
    std::cout << std::endl << "Base 10: " << bitboard << std::endl;
}

/*
int index_least_sig(U64 bitboard)
{
    // Binary Search for least significant bit. Binary searching binary? A binary binary search? 
    if (bitboard & 1)
    {
        return 0; // Potential optimisation? Remember to test performance once engine functional.
    }
    else
    {
        int count;
        
        count = 1;
        if ((bitboard & 0xffffffff) == 0) // Checks half the bitboard for a 1
        {
            bitboard >>= 32;
            count += 32;
        }
        if ((bitboard & 0xffff) == 0) // Checks half of the remaining bitboard for a 1
        {  
            bitboard >>= 16;  
            count += 16;
        }
        if ((bitboard & 0xff) == 0) // E.t.c.
        {  
            bitboard >>= 8;  
            count += 8;
        }
        if ((bitboard & 0xf) == 0) 
        {  
            bitboard >>= 4;
            count += 4;
        }
        if ((bitboard & 0x3) == 0) 
        {  
            bitboard >>= 2;
            count += 2;
        }
        count -= bitboard & 1;
        return count;
    }
}
*/
