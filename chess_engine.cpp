#include <iostream>
#include <string.h>
#include "chess_engine.hpp"
#include "move.hpp"

const int castling_rights[64] = { // Ask lomax if this should be moved inside the chess engine class
    7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

void ChessEngine::load_FEN(char *FEN)
{
    memset(chess_board.occupancies, 0, sizeof(chess_board.occupancies));
    memset(chess_board.piece_boards, 0, sizeof(chess_board.piece_boards));
    chess_board.castle_rights = 0;

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
            chess_board.castle_rights += wk;
            break;

        case 'Q':
            chess_board.castle_rights += wq;
            break;

        case 'k':
            chess_board.castle_rights += bk;
            break;

        case 'q':
            chess_board.castle_rights += bq;
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

void print_chessboard(Board chess_board)
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
    std::cout << "Side to move: " << (chess_board.side ? "Black" : "White") << std::endl;
    std::cout << "Castling rights: ";
    if check_bit (chess_board.castle_rights, 0)
        std::cout << 'K';
    if check_bit (chess_board.castle_rights, 1)
        std::cout << 'Q';
    if check_bit (chess_board.castle_rights, 2)
        std::cout << 'k';
    if check_bit (chess_board.castle_rights, 3)
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

ChessEngine::ChessEngine(int search_depth)
{
    default_search_depth = search_depth;

    int mg_material_value[12] = {82, 337, 365, 477, 1025, 9999, 82, 337, 365, 477, 1025, 9999};
    int eg_material_value[12] = {94, 281, 297, 512, 936, 9999, 94, 281, 297, 512, 936, 9999};

    int pawn_table[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    int knight_table[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    int bishop_table[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };

    int rook_table[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };

    int queen_table[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    int mg_king_table[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };

    int eg_king_table[64] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };

    int* mg_pos_tables[12] ={
        pawn_table,
        knight_table,
        bishop_table,
        rook_table,
        queen_table,
        mg_king_table,
        pawn_table,
        knight_table,
        bishop_table,
        rook_table,
        queen_table,
        mg_king_table
    };

    int* eg_pos_tables[12] ={
        pawn_table,
        knight_table,
        bishop_table,
        rook_table,
        queen_table,
        eg_king_table,
        pawn_table,
        knight_table,
        bishop_table,
        rook_table,
        queen_table,
        eg_king_table
    };

    for (int piece = 0; piece < 6; piece++)
    {
        for (int index = 0; index < 64; index++)
        {
            mg_table[piece][index] = mg_pos_tables[piece][index] + mg_material_value[piece];
            eg_table[piece][index] = eg_pos_tables[piece][index] + eg_material_value[piece];
        }
    }
    for (int piece = 6; piece < 12; piece++)
    {
        for (int index = 0; index < 64; index++)
        {
            mg_table[piece][index] = mg_pos_tables[piece][index^56] + mg_material_value[piece]; // Flips "^56" flips the index to the other side of the board
            eg_table[piece][index] = eg_pos_tables[piece][index^56] + eg_material_value[piece];
        }
    }
}

void ChessEngine::UCI_send_move(int move)
{
    char piece_to_char[] = {'p','n','b','r','q','k','p','n','b','r','q','k'};
    if (move_promotion(move))
        printf("bestmove %s%s%c\n", index_to_square[move_origin(move)],
                           index_to_square[move_target(move)],
                           piece_to_char[move_promotion(move)]);
    else
        printf("bestmove %s%s\n", index_to_square[move_origin(move)],
                           index_to_square[move_target(move)]);
}

int ChessEngine::UCI_get_move(char* command_ptr)
{
    std::map<char, int> char_to_piece = {
    {'p', 0},
    {'n', 1},
    {'b', 2},
    {'r', 3},
    {'q', 4}
    };

    int origin = (command_ptr[0] - 'a') + (8 - (command_ptr[1] - '0')) * 8;
    int target = (command_ptr[2] - 'a') + (8 - (command_ptr[3] - '0')) * 8;
    int promotion = ((command_ptr[4] == ' ') | (command_ptr[4] == '\0'))? 0 : (char_to_piece[command_ptr[4]] + 6*chess_board.side);

    std::stack<int> move_stack;
    find_moves(&move_stack);
    int move;
    while (!move_stack.empty())
    {
        move = move_stack.top();
        move_stack.pop();

        if ((move_origin(move) == origin) && (move_target(move) == target) && (move_promotion(move) == promotion))
        {
            return move;
        }
    }
    return 0;
}

void ChessEngine::UCI_get_position(char* command_ptr)
{
    command_ptr += 9;
    
    if (strncmp(command_ptr, "startpos", 8) == 0)
        load_FEN((char *) "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    else
    {
        command_ptr = strstr(command_ptr, "fen");   
        if (command_ptr == NULL)
            load_FEN((char *) "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        else
        {
            command_ptr += 4;
            load_FEN(command_ptr);
        }
    }
    
    command_ptr = strstr(command_ptr, "moves");
    
    if (command_ptr != NULL)
    {
        command_ptr += 6;

        int move;
        while(*command_ptr)
        {
            move = UCI_get_move(command_ptr);

            if (move == 0)
            {
                std::cout << "Position command move " << command_ptr << " not in generated moves" << std::endl;
                break;
            }
            
            make_move(move);
            
            while (*command_ptr && *command_ptr != ' ') command_ptr++;
            command_ptr++;
        }
    }
}

void ChessEngine::UCI_go(char* command_ptr)
{
    command_ptr += 3;

    search();

    // strstr(command_ptr, "depth");
    // if (command_ptr == NULL)
    // {
    //     search();
    // }
    // else
    // {
    //     search_depth = atoi(command_ptr+6);
    //     search();
    // }
}

void ChessEngine::UCI_main_loop()
{
    char command[5000];
    while (true)
    {
        memset(command,0,5000);
        std::cin.getline(command, sizeof(command));

        if (command[0] == '\n') continue;

        if (strncmp(command, "isready", 7) == 0)
        {
            std::cout << "readyok" << std::endl;
            continue;
        }
        else if (strncmp(command, "position", 8) == 0)
        {
            UCI_get_position(command);
            print_chessboard(chess_board);
            continue;
        }
        else if (strncmp(command, "ucinewgame", 10) == 0)
        {
            load_FEN((char *)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            continue;
        }
        else if (strncmp(command, "go", 2) == 0)
        {
            UCI_go(command);
            continue;
        }
        else if (strncmp(command, "quit", 4) == 0)
        {
            break;
        }
        else if (strncmp(command, "uci", 3) == 0)
        {
            std::cout << "id name Chess_Surf" << std::endl;
            std::cout << "id author WindSurfista" << std::endl;
            std::cout << "uciok" << std::endl;
        }
    }
}

// Possible optimisation: Maintain an incrementally updated attacked squares bitboard in the chess board struct is updated with each move.
bool ChessEngine::check_if_attacked(int index) // Checks if a square is dangerous for the current side
{
    if (move_gen.get_pawn(chess_board.side,index) & chess_board.piece_boards[p - 6 * chess_board.side])
        return true;
    if (move_gen.get_knight(index) & chess_board.piece_boards[n - 6 * chess_board.side])
        return true;
    if (move_gen.get_bishop(index, chess_board.occupancies[all]) & chess_board.piece_boards[b - 6 * chess_board.side])
        return true;
    if (move_gen.get_rook(index, chess_board.occupancies[all]) & chess_board.piece_boards[r - 6 * chess_board.side])
        return true;
    if (move_gen.get_queen(index, chess_board.occupancies[all]) & chess_board.piece_boards[q - 6 * chess_board.side])
        return true;
    if (move_gen.get_king(index) & chess_board.piece_boards[k - 6 * chess_board.side])
        return true;
    return false;
}

// Possible optimisation: Check if the type of piece is on the board might speed up this function a bit?
void ChessEngine::find_moves(std::stack<int>* move_stack)
{
    while (!move_stack->empty()) move_stack->pop();

    int origin;
    int target;
    U64 bitboard;
    U64 captures;
    U64 target_mask;

    // ###########
    // ## Pawns ##
    // ###########

    if (chess_board.side == white)
    {
        bitboard = chess_board.piece_boards[P];

        while (bitboard)
        {
            origin = index_least_sig(bitboard);
            clear_bit(bitboard, origin);
            target = origin - 8;

            if (check_bit(chess_board.occupancies[neither], target)) // Non - capture moves
            {
                if (target <= 7)
                {
                    move_stack->push(create_move(origin, target, P, Q, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, R, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, B, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, N, 0, 0, 0, 0));
                    // White pawn promotion
                }
                else
                {
                    move_stack->push(create_move(origin, target, P, 0, 0, 0, 0, 0));
                    // White pawn quiet
                }

                if (origin >= 48)
                {
                    target = origin - 16;
                    if (check_bit(chess_board.occupancies[neither], target))
                    {
                        move_stack->push(create_move(origin, target, P, 0, 0, 1, 0, 0));
                        // White pawn double
                    }
                }
            }

            captures = move_gen.get_pawn(white, origin) & chess_board.occupancies[black];
            while (captures) // Capture moves
            {
                target = index_least_sig(captures);
                if (target <= 7)
                {
                    move_stack->push(create_move(origin, target, P, Q, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, R, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, B, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, P, N, 1, 0, 0, 0));
                    // White pawn promotion capture
                }
                else
                {
                    move_stack->push(create_move(origin, target, P, 0, 1, 0, 0, 0));
                    // White pawn capture
                }
                clear_bit(captures, target);
            }

            if (chess_board.enpassant != -1) // Enpassant moves
            {
                if (check_bit(move_gen.get_pawn(white, origin), chess_board.enpassant))
                {
                    move_stack->push(create_move(origin, chess_board.enpassant, P, 0, 1, 0, 0, 1));
                    // White pawn enpassant
                }
            }
        }
    }
    else
    {
        bitboard = chess_board.piece_boards[p];

        while (bitboard)
        {
            origin = index_least_sig(bitboard);
            clear_bit(bitboard, origin);
            target = origin + 8;

            if (check_bit(chess_board.occupancies[neither], target))
            {
                if (target >= 56)
                {
                    move_stack->push(create_move(origin, target, p, q, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, r, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, b, 0, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, n, 0, 0, 0, 0));
                    // Black pawn promotion
                }
                else
                {
                    move_stack->push(create_move(origin, target, p, 0, 0, 0, 0, 0));
                    // Black pawn quiet
                }

                if (origin <= 15)
                {
                    target = origin + 16;
                    if (check_bit(chess_board.occupancies[neither], target))
                    {
                        move_stack->push(create_move(origin, target, p, 0, 0, 1, 0, 0));
                        // Black pawn double
                    }
                }
            }

            captures = move_gen.get_pawn(black, origin) & chess_board.occupancies[white];
            while (captures)
            {
                target = index_least_sig(captures);
                if (target >= 56) // White's home row
                {
                    move_stack->push(create_move(origin, target, p, q, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, r, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, b, 1, 0, 0, 0));
                    move_stack->push(create_move(origin, target, p, n, 1, 0, 0, 0));
                    // Black pawn promotion capture
                }
                else
                {
                    move_stack->push(create_move(origin, target, p, 0, 1, 0, 0, 0));
                    // Black pawn capture
                }
                clear_bit(captures, target);
            }

            if (chess_board.enpassant != -1) // Enpassant moves
            {
                if (check_bit(move_gen.get_pawn(black, origin), chess_board.enpassant))
                {
                    move_stack->push(create_move(origin, chess_board.enpassant, p, 0, 1, 0, 0, 1));
                    // Black pawn enpassant
                }
            }
        }
    }

    // #############
    // ## Knights ##
    // #############

    bitboard = chess_board.piece_boards[chess_board.side ? n : N];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        clear_bit(bitboard, origin);
        target_mask = move_gen.get_knight(origin) & ~chess_board.occupancies[chess_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            clear_bit(target_mask, target);
            if (check_bit(chess_board.occupancies[!chess_board.side], target))
            {
                move_stack->push(create_move(origin, target, (N + (6 * chess_board.side)), 0, 1, 0, 0, 0));
                // Knight capture
            }
            else
            {
                move_stack->push(create_move(origin, target, (N + (6 * chess_board.side)), 0, 0, 0, 0, 0));
                // Knight quiet
            }
        }
    }

    // #############
    // ## Bishops ##
    // #############

    bitboard = chess_board.piece_boards[chess_board.side ? b : B];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        clear_bit(bitboard, origin);
        target_mask = move_gen.get_bishop(origin, chess_board.occupancies[all]) & ~chess_board.occupancies[chess_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            clear_bit(target_mask, target);
            if (check_bit(chess_board.occupancies[!chess_board.side], target))
            {
                move_stack->push(create_move(origin, target, (B + 6 * chess_board.side), 0, 1, 0, 0, 0));
                // Bishop capture
            }
            else
            {
                move_stack->push(create_move(origin, target, (B + 6 * chess_board.side), 0, 0, 0, 0, 0));
                // Bishop quiet
            }
        }
    }

    // ###########
    // ## Rooks ##
    // ###########

    bitboard = chess_board.piece_boards[chess_board.side ? r : R];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        clear_bit(bitboard, origin);
        target_mask = move_gen.get_rook(origin, chess_board.occupancies[all]) & ~chess_board.occupancies[chess_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            clear_bit(target_mask, target);
            if (check_bit(chess_board.occupancies[!chess_board.side], target))
            {
                move_stack->push(create_move(origin, target, (R + 6 * chess_board.side), 0, 1, 0, 0, 0));
                // Rook capture
            }
            else
            {
                move_stack->push(create_move(origin, target, (R + 6 * chess_board.side), 0, 0, 0, 0, 0));
                // Rook quiet
            }
        }
    }

    // ############
    // ## Queens ##
    // ############

    bitboard = chess_board.piece_boards[chess_board.side ? q : Q];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        clear_bit(bitboard, origin);
        target_mask = move_gen.get_queen(origin, chess_board.occupancies[all]) & ~chess_board.occupancies[chess_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            clear_bit(target_mask, target);
            if (check_bit(chess_board.occupancies[!chess_board.side], target))
            {
                move_stack->push(create_move(origin, target, (Q + 6 * chess_board.side), 0, 1, 0, 0, 0));
                // Queen capture
            }
            else
            {
                move_stack->push(create_move(origin, target, (Q + 6 * chess_board.side), 0, 0, 0, 0, 0));
                // Queen quiet
            }
        }
    }

    // ###########
    // ## Kings ##
    // ###########

    bitboard = chess_board.piece_boards[chess_board.side ? k : K];

    while (bitboard)
    {
        origin = index_least_sig(bitboard);
        clear_bit(bitboard, origin);
        target_mask = move_gen.get_king(origin) & ~chess_board.occupancies[chess_board.side];

        while (target_mask)
        {
            target = index_least_sig(target_mask);
            clear_bit(target_mask, target);
            if (check_bit(chess_board.occupancies[!chess_board.side], target))
            {
                move_stack->push(create_move(origin, target, (K + 6 * chess_board.side), 0, 1, 0, 0, 0));
                // King capture
            }
            else
            {
                move_stack->push(create_move(origin, target, (K + 6 * chess_board.side), 0, 0, 0, 0, 0));
                // King quiet
            }
        }
    }

    // ##############
    // ## Castling ##
    // ##############

    if (chess_board.side == white)
    {
        if (chess_board.castle_rights & wk)
        {
            if (check_bit(chess_board.occupancies[neither], f1) && check_bit(chess_board.occupancies[neither], g1))
            {
                if (!check_if_attacked(e1) && !check_if_attacked(f1))
                {
                    move_stack->push(create_move(e1, g1, K, 0, 0, 0, 1, 0));
                    // White King side castle
                }
            }
        }
        if (chess_board.castle_rights & wq)
        {
            if (check_bit(chess_board.occupancies[neither], d1) && check_bit(chess_board.occupancies[neither], c1) && check_bit(chess_board.occupancies[neither], b1))
            {
                if (!check_if_attacked(e1) && !check_if_attacked(d1))
                {
                    move_stack->push(create_move(e1, c1, K, 0, 0, 0, 1, 0));
                    // White Queen side castle
                }
            }
        }
    }
    else
    {
        if (chess_board.castle_rights & bk)
        {
            if (check_bit(chess_board.occupancies[neither], f8) && check_bit(chess_board.occupancies[neither], g8))
            {
                if (!check_if_attacked(e8) && !check_if_attacked(f8))
                {
                    move_stack->push(create_move(e8, g8, k, 0, 0, 0, 1, 0));
                    // Black King side castle
                }
            }
        }
        if (chess_board.castle_rights & bq)
        {
            if (check_bit(chess_board.occupancies[neither], d8) && check_bit(chess_board.occupancies[neither], c8) && check_bit(chess_board.occupancies[neither], b8))
            {
                if (!check_if_attacked(e8) && !check_if_attacked(d8))
                {
                    move_stack->push(create_move(e8, c8, k, 0, 0, 0, 1, 0));
                    // Black Queen side castle
                }
            }
        }
    }
}

bool ChessEngine::make_move(int move)
{
    int piece = move_piece(move);
    bool capture_flag = move_capture_flag(move);
    bool castling_flag = move_castling_flag(move);
    bool double_flag = move_double_flag(move);
    bool enpassant_flag = move_enpassant_flag(move);
    int origin = move_origin(move);
    int target = move_target(move);
    int promotion_piece = move_promotion(move);

    set_bit(chess_board.piece_boards[piece], target); // Place piece on target square
    set_bit(chess_board.occupancies[chess_board.side], target);

    clear_bit(chess_board.occupancies[chess_board.side], origin); // Remove piece from origin square
    clear_bit(chess_board.piece_boards[piece], origin);

    chess_board.castle_rights &= castling_rights[target];
    chess_board.castle_rights &= castling_rights[origin];

    int start = chess_board.side ? 0:6; // Determines whether you should iterate through the pieces starting from the white or black pieces
    int capture;

    if (enpassant_flag)
    {
        target = (chess_board.side ? target-8 : target+8);
    }

    if (capture_flag)
    {
        for (int capture = start; capture < (start+6); capture++) // Find which piece is being captured
        {
            if (check_bit(chess_board.piece_boards[capture], target))
            {
                clear_bit(chess_board.piece_boards[capture], target); // Remove captured piece
                clear_bit(chess_board.occupancies[!chess_board.side], target);
                break;
            }
        }
    }

    if (promotion_piece)
    {
        clear_bit(chess_board.piece_boards[start], target); // The start variable is equal to the correct pawn colour so I can just repurpose it
        clear_bit(chess_board.occupancies[chess_board.side], target); // Removes the pawn that was just moved

        set_bit(chess_board.piece_boards[promotion_piece], target); // Places the promotion piece in place of the pawn
        set_bit(chess_board.occupancies[chess_board.side], target);
    }

    if (double_flag)
    {
        chess_board.enpassant = ((origin + target)/2); // Finds the space behind the pawn by averaging the origin and target square
    }
    else
    {
        chess_board.enpassant = -1;
    }

    if (castling_flag)
    {
        switch (target)
        {
        case g1: // White king side castle
            set_bit(chess_board.piece_boards[R], f1);
            set_bit(chess_board.occupancies[white], f1);
            
            clear_bit(chess_board.piece_boards[R], h1);
            clear_bit(chess_board.occupancies[white], h1);
            break;
        
        case c1:
            set_bit(chess_board.piece_boards[R], d1);
            set_bit(chess_board.occupancies[white], d1);
            
            clear_bit(chess_board.piece_boards[R], a1);
            clear_bit(chess_board.occupancies[white], a1);
            break;

        case g8:
            set_bit(chess_board.piece_boards[r], f8);
            set_bit(chess_board.occupancies[black], f8);
            
            clear_bit(chess_board.piece_boards[r], h8);
            clear_bit(chess_board.occupancies[black], h8);

            break;
        
        case c8:
            set_bit(chess_board.piece_boards[r], d8);
            set_bit(chess_board.occupancies[black], d8);
            
            clear_bit(chess_board.piece_boards[r], a8);
            clear_bit(chess_board.occupancies[black], a8);
        }
    }

    chess_board.occupancies[all] = chess_board.occupancies[black] | chess_board.occupancies[white];
    chess_board.occupancies[neither] = ~chess_board.occupancies[all];

    int least_sig = index_least_sig(chess_board.piece_boards[K + 6*chess_board.side]);
    if (least_sig == -1) return 0;

    if (check_if_attacked(least_sig))
    {
        chess_board.side ^= 1;
        return 0; // Oh no! The king is in check!!
    }
    else
    {
        chess_board.side ^= 1;
        return 1; // Seems ok ! Return legal 1
    }
}

int ChessEngine::static_eval()
{
    int value = 0;
    U64 bitboard;
    int index;

    for (int piece = 0; piece < 6; piece++)
    {
        bitboard = chess_board.piece_boards[piece];
        while (bitboard)
        {
            index = index_least_sig(bitboard);
            value += (endgame_flag ? eg_table[piece][index] : mg_table[piece][index]);
            clear_bit(bitboard, index);
        }
    }
    for (int piece = 6; piece < 12; piece++)
    {
        bitboard = chess_board.piece_boards[piece];
        while (bitboard)
        {
            index = index_least_sig(bitboard);
            value -= (endgame_flag ? eg_table[piece][index] : mg_table[piece][index]);
            clear_bit(bitboard, index);
        }
    }

    return value;
}

int ChessEngine::minimax(int depth, int alpha, int beta)
{   
    if (depth == 0)
        return static_eval();
    
    std::stack<int> move_stack;
    find_moves(&move_stack);
    int move;

    if (chess_board.side == white)
    {
        int max_eval = -99999;
        while (!move_stack.empty())
        {
            // Get next move and play it on the board
            move = move_stack.top();
            move_stack.pop();
            Board backup(chess_board);
            if (make_move(move) == 0)
            {
                // The move is illegal, undo the move and don't explore this branch further
                chess_board = backup;
                continue;
            }

            // Evaluate the move recursively
            int eval = minimax((depth-1), alpha, beta);
            if (eval > max_eval)
            {
                max_eval = eval;
                if (depth == default_search_depth) best_move = move;
            }
            std::max(alpha, eval);

            chess_board = backup;

            if (beta <= alpha) break; // There is no point in exploring this branch further, we have enough information to safely prune it
        }
        return max_eval;
    }
    else
    {
        int min_eval = 99999;
        while (!move_stack.empty())
        {
            move = move_stack.top();
            move_stack.pop();
            Board backup(chess_board);
            if (make_move(move) == 0)
            {
                chess_board = backup;
                continue;
            }

            int eval = minimax((depth-1), alpha, beta);
            if (eval < min_eval)
            {
                min_eval = eval;
                if (depth == default_search_depth) best_move = move;
            }
            std::min(beta, eval);

            chess_board = backup;

            if (beta <= alpha) break; // There is no point in exploring this branch further, we have enough information to safely prune it
        }
        return min_eval;
    }
}

void ChessEngine::search()
{
    minimax(default_search_depth, -99999, 99999);
    UCI_send_move(best_move);
}
