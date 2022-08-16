#include "attacks.hpp"
#include <iostream>

void ChessEngine::parse_FEN(char *FEN)
{
    // Reads board state
    std::string piece_string = "PNBRQKpnbrqk";
    int index = 0;
    while (index != 64)
    {
        if (std::isdigit(*FEN))
        {
            index += *FEN - '0'; // Converts from char to int
        }
        else if (*FEN != '/')
        {
            int piece = piece_string.find(*FEN);
            set_bit(chess_board.piece_boards[piece], index);
            set_bit(chess_board.occupancies[(piece > 5)], index);
            index++;
        }
        FEN++;
    }
    chess_board.occupancies[all] = chess_board.occupancies[white] | chess_board.occupancies[black];
    chess_board.occupancies[neither] = ~(chess_board.occupancies[all]);

    // Reads which side is playing currently
    FEN++;
    chess_board.side = (*FEN == 'w') ? white : black;

    // Reads castling rights
    FEN += 2;
    while (*FEN != ' ')
    {
        switch (*FEN)
        {
        case 'K':
            chess_board.castle += wk;
            break;

        case 'Q':
            chess_board.castle += wq;
            break;

        case 'k':
            chess_board.castle += bk;
            break;

        case 'q':
            chess_board.castle += bq;
            break;
        }
        FEN++;
    }

    // Reads enpassant square
    FEN++;
    if (*FEN != '-')
    {
        chess_board.enpassant = FEN[0] - 'a' + (8 + '0' - FEN[1]) * 8;
        FEN += 2;
    }
}

void ChessEngine::print_chessboard()
{
    std::string unicode_pieces[12] = {"♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int index = i * 8 + j;
            std::string output = ((i + j) % 2) ? "□" : "■";
            for (int piece = 0; piece < 12; piece++)
            {
                if (check_bit(chess_board.piece_boards[piece], index))
                {
                    output = unicode_pieces[piece];
                    break;
                }
            }
            std::cout << output << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Side to move: " << (chess_board.side ? "White" : "Black") << std::endl;
    std::cout << "Castling rights: ";
    if check_bit (chess_board.castle, 0)
        std::cout << 'K';
    if check_bit (chess_board.castle, 1)
        std::cout << 'Q';
    if check_bit (chess_board.castle, 2)
        std::cout << 'k';
    if check_bit (chess_board.castle, 3)
        std::cout << 'q';
    std::cout << std::endl;

    std::cout << "Enpassant Square: ";
    if (chess_board.enpassant != -1)
    {
        char files[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        std::cout << files[chess_board.enpassant % 8] << 8 - chess_board.enpassant % 8 << std::endl;
    }
    else
    {
        std::cout << "None" << std::endl;
    }
}

U64 ChessEngine::pawn_mask(int color, int index)
{
    U64 bitmask = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, index);

    if (color == white)
    {
        if (check_bit(no_a_file, index))
        {
            bitmask |= bitboard >> 9;
        }
        if (check_bit(no_h_file, index))
        {
            bitmask |= bitboard >> 7;
        }
        return bitmask;
    }
    else
    {
        if (check_bit(no_a_file, index))
        {
            bitmask |= bitboard << 7;
        }
        if (check_bit(no_h_file, index))
        {
            bitmask |= bitboard << 9;
        }
        return bitmask;
    }
}

U64 ChessEngine::knight_mask(int index)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, index);

    if (check_bit(no_a_file, index))
    {
        attacks |= bitboard << 15;
        attacks |= bitboard >> 17;
    }

    if (check_bit(no_ab_file, index))
    {
        attacks |= bitboard << 6;
        attacks |= bitboard >> 10;
    }

    if (check_bit(no_h_file, index))
    {
        attacks |= bitboard << 17;
        attacks |= bitboard >> 15;
    }

    if (check_bit(no_gh_file, index))
    {
        attacks |= bitboard << 10;
        attacks |= bitboard >> 6;
    }
    return attacks;
}

U64 ChessEngine::king_mask(int index)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, index);

    if (check_bit(no_h_file, index))
    {
        attacks |= bitboard << 9;
        attacks |= bitboard << 1;
        attacks |= bitboard >> 7;
    }

    if (check_bit(no_a_file, index))
    {
        attacks |= bitboard << 7;
        attacks |= bitboard >> 1;
        attacks |= bitboard >> 9;
    }
    attacks |= bitboard << 8;
    attacks |= bitboard >> 8;

    return attacks;
}

ChessEngine::ChessEngine()
{
    for (int index = 0; index < 64; index++)
    {
        pawn_attacks[white][index] = pawn_mask(white, index);
        pawn_attacks[black][index] = pawn_mask(black, index);

        knight_attacks[index] = knight_mask(index);

        king_attacks[index] = king_mask(index);
    }
}

// Possible optimisation: Maintain an incrementally updated attacked squares bitboard in the chess engine class which is updated with each move. 
bool ChessEngine::check_if_attacked(bool side, int index) // Checks if a square is dangerous for the side
{
    if (pawn_attacks[side][index] & chess_board.piece_boards[p - 6 * side])
        return true;
    if (knight_attacks[index] & chess_board.piece_boards[n - 6 * side])
        return true;
    if (get_bishop(index, chess_board.occupancies[2]) & chess_board.piece_boards[b - 6 * side])
        return true;
    if (get_rook(index, chess_board.occupancies[2]) & chess_board.piece_boards[r - 6 * side])
        return true;
    if (get_queen(index, chess_board.occupancies[2]) & chess_board.piece_boards[q - 6 * side])
        return true;
    if (king_attacks[index] & chess_board.piece_boards[k - 6 * side])
        return true;
    return false;
}


// Todo: Currently class functions for testing and debugging change to inline function once able
void ChessEngine::generate_pawn_moves(Board generation_board)
{
    int origin;
    int target;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? p : P];
    U64 captures = 0ULL;
    
    if (generation_board.side == white)
    {
        while (bitboard)
        {
            origin = index_least_sig(bitboard);
            remove_bit(bitboard, origin);
            target = origin - 8;

            if (check_bit(chess_board.occupancies[neither], target)) // Non - capture moves
            {
                if (target <= 7)
                {
                    std::cout << "PROMOTION" << std::endl;
                }
                else
                {
                    std::cout << "Quiet move" << std::endl;
                }

                if (origin >= 48)
                {
                    target = origin - 16;
                    if (check_bit(chess_board.occupancies[neither], target))
                    {
                        std::cout << "Double push" << std::endl;
                    }
                }     
            }

            captures = pawn_attacks[white][origin] & chess_board.occupancies[black];
            while (captures) // Capture moves (No enpassant)
            {
                target = index_least_sig(captures);
                if (target <= 7)
                {
                    std::cout << "Promotion Capture" << std::endl;
                }
                else
                {
                    std::cout << "Capture" << std::endl;
                }
                remove_bit(captures, target);
            }
        
            if (chess_board.enpassant != -1) // Enpassant moves
            {
                if (check_bit(pawn_attacks[generation_board.side][origin], chess_board.enpassant))
                {
                    std::cout << "Enpassant" << std::endl;
                }
            }

        }
    }
    else
    {
        origin = 0;
        target = 0;
        bitboard = chess_board.piece_boards[p];

        while (bitboard)
        {
            origin = index_least_sig(bitboard);
            remove_bit(bitboard, origin);
            target = origin + 8;

            if (check_bit(chess_board.occupancies[neither], target))
            {
                if (target >= 56)
                {
                    std::cout << "PROMOTION" << std::endl;
                }
                else
                {
                    std::cout << "Quiet move" << std::endl;
                }

                if (origin <= 15)
                {
                    target = origin + 16;
                    if (check_bit(chess_board.occupancies[neither], target))
                    {
                        std::cout << "Double push" << std::endl;
                    }
                }
            }

            captures = pawn_attacks[black][origin] & chess_board.occupancies[white];
            while (captures)
            {
                target = index_least_sig(captures);
                if (target >= 56) // White's home row
                {
                    std::cout << "Promotion Capture" << std::endl;
                }
                else
                {
                    std::cout << "Capture" << std::endl;
                }
                remove_bit(captures, target);
            }

            if (chess_board.enpassant != -1) // Enpassant moves
            {
                if (check_bit(pawn_attacks[white][origin], chess_board.enpassant))
                {
                    std::cout << "Enpassant" << std::endl;
                }
            }
        }
    }
}

void ChessEngine::generate_knight_moves(Board generation_board)
{
    int origin;
    int target;
    U64 target_mask;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? n : N];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        remove_bit(bitboard, origin);
        target_mask = knight_attacks[origin] & ~generation_board.occupancies[generation_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            remove_bit(target_mask, target);
            if (check_bit(generation_board.occupancies[!generation_board.side], target))
            {
                std::cout << "Knight capture" << std::endl;
            }
            else
            {
                std::cout << "Knight quiet" << std::endl;
            }
        }
    }
}

void ChessEngine::generate_bishop_moves(Board generation_board)
{
    int origin;
    int target;
    U64 target_mask;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? b : B];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        remove_bit(bitboard, origin);
        target_mask = magic.get_bishop(origin, generation_board.occupancies[all]) & ~generation_board.occupancies[generation_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            remove_bit(target_mask, target);
            if (check_bit(generation_board.occupancies[!generation_board.side], target))
            {
                std::cout << "Bishop Capture" << std::endl;
            }
            else
            {
                std::cout << "Bishop Quiet" << std::endl;
            }
        }
    }
}

void ChessEngine::generate_rook_moves(Board generation_board)
{
    int origin;
    int target;
    U64 target_mask;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? r : R];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        remove_bit(bitboard, origin);
        target_mask = magic.get_rook(origin, generation_board.occupancies[all]) & ~generation_board.occupancies[generation_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            remove_bit(target_mask, target);
            if (check_bit(generation_board.occupancies[!generation_board.side], target))
            {
                std::cout << "Rook Capture" << std::endl;
            }
            else
            {
                std::cout << "Rook Quiet" << std::endl;
            }
        }
    }
}

void ChessEngine::generate_queen_moves(Board generation_board)
{
    int origin;
    int target;
    U64 target_mask;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? q : Q];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        remove_bit(bitboard, origin);
        target_mask = magic.get_queen(origin, generation_board.occupancies[all]) & ~generation_board.occupancies[generation_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            remove_bit(target_mask, target);
            if (check_bit(generation_board.occupancies[!generation_board.side], target))
            {
                std::cout << "Queen Capture" << std::endl;
            }
            else
            {
                std::cout << "Queen Quiet" << std::endl;
            }
        }
    }
}

void ChessEngine::generate_king_moves(Board generation_board)
{
    int origin;
    int target;
    U64 target_mask;
    U64 bitboard = generation_board.piece_boards[generation_board.side ? k : K];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        remove_bit(bitboard, origin);
        target_mask = king_attacks[origin] & ~generation_board.occupancies[generation_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            remove_bit(target_mask, target);
            if (check_bit(generation_board.occupancies[!generation_board.side], target))
            {
                std::cout << "King Capture" << std::endl;
            }
            else
            {
                std::cout << "King Quiet" << std::endl;
            }
        }
    }
}

void ChessEngine::generate_castling_moves(Board generation_board)
{
    if (generation_board.side == white)
    {
        if (generation_board.castle & wk)
        {
            if (check_bit(generation_board.occupancies[neither], f1) && check_bit(generation_board.occupancies[neither], g1))
            {
                if (!check_if_attacked(generation_board.side, e1) && !check_if_attacked(generation_board.side, f1))
                {
                    std::cout << "White side castle" << std::endl;
                }
            }
        }
        if (generation_board.castle & wq)
        {
            if (check_bit(generation_board.occupancies[neither], d1) && check_bit(generation_board.occupancies[neither], c1) && check_bit(generation_board.occupancies[neither], b1))
            {
                if (!check_if_attacked(generation_board.side, e1) && !check_if_attacked(generation_board.side, d1))
                {
                    std::cout << "White queen side castle" << std::endl;
                }
            }
        }
    }
    else
    {
        if (generation_board.castle & bk)
        {
            if (check_bit(generation_board.occupancies[neither], f8) && check_bit(generation_board.occupancies[neither], g8))
            {
                if (!check_if_attacked(generation_board.side, e8) && !check_if_attacked(generation_board.side, f8))
                {
                    std::cout << "Black king side castle" << std::endl;
                }
            }
        }
        if (generation_board.castle & bq)
        {
            if (check_bit(generation_board.occupancies[neither], d8) && check_bit(generation_board.occupancies[neither], c8) && check_bit(generation_board.occupancies[neither], b8))
            {
                if (!check_if_attacked(generation_board.side, e8) && !check_if_attacked(generation_board.side, d8))
                {
                    std::cout << "Black queen side castle" << std::endl;
                }
            }
        }
    }
}

// Possible optimisation: Check if the type of piece is on the board might speed up this function a bit?
void ChessEngine::generate_moves(Board generation_board)
{
    generate_pawn_moves(generation_board);
    generate_knight_moves(generation_board);
    generate_bishop_moves(generation_board);
    generate_rook_moves(generation_board);
    generate_queen_moves(generation_board);
    generate_king_moves(generation_board);
    generate_castling_moves(generation_board);
}
