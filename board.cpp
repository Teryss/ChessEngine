#include <iostream>
#include "board.h"
#include "attacks.h"
#include <cstring>
// #include <ctype.h>

int char_to_piece_enum(char c){
    for (int i = 0; i < 12; i++){
        if (c == ascii_pieces[i]){
            return i;
        }
    }
    return 0;
}

Board::Board(){}
Board::Board(const char* fen){
    Board::parse_fen(fen);
}

void Board::print(){
    int found;
    for (int r = 0; r < 8; r++){
        for (int f = 0; f < 8; f++){
            found = -1;
            if (!f)
                std::cout << " " << 8 - r << "   ";
            for (int count = P; count < k + 1; count++){
                if (GET_BIT(pieces[count], FR_TO_SQR(r,f)))
                    found = count;
            }
            (found == -1) ? std::cout << " . " : std::cout << ' ' << ascii_pieces[found] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "\n      a  b  c  d  e  f  g  h\n";
    std::cout << "Side: " << (!side ? "White" : "Black") << "\nenpassant: " << c_sqrs[enpassant];
    std::cout << "\nCastling: " << ((castling & WK) ? 'K' : '-') << ((castling & WQ) ? 'Q' : '-')
                                << ((castling & BK) ? 'k' : '-') << ((castling & BQ) ? 'q' : '-') << std:: endl;
}

void Board::reset(){
    memset(pieces, 0ULL, sizeof(pieces));
    memset(occupancy, 0ULL, sizeof(occupancy));
    side = WHITE;
    castling = 0;
    enpassant = no_sq;
}

void Board::parse_fen(const char* fen){
    Board::reset();
    int index = 0;
    int num_of_spaces = 0;
    std::cout << "Parsing FEN: " << fen << std::endl;
    for (int i = 0; i < (strlen(fen) - 1); i++){
        if(fen[i] == ' '){
            num_of_spaces++;
        }else if (num_of_spaces == 0){
            if (fen[i] >= 'b' && fen[i] <= 'r' || fen[i] >= 'B' && fen[i] <= 'R'){
                int type = char_to_piece_enum(fen[i]);
                SET_BIT(pieces[type], index);
                index++;
            }
            else if(fen[i] > '0' && fen[i] < '9'){
                index += fen[i] - '0';
            }
        }
        else if (num_of_spaces == 1){
            side = (fen[i] == 'w') ? WHITE : BLACK;
        }
        else if (num_of_spaces == 2){
            switch(fen[i]){
                case 'K': castling |= WK; break;
                case 'Q': castling |= WQ; break;
                case 'k': castling |= BK; break;
                case 'q': castling |= BQ; break;
                case '-': break;
            }
        }
        else if(num_of_spaces == 3){
            if (fen[i] != '-'){
                enpassant = FR_TO_SQR((8 - ((int)fen[i+1] - '0')), ((int)fen[i] - 'a'));
                i++;
            }
            else{
                enpassant = no_sq;
            }
        }
    }

    for (int piece = P; piece < K + 1; piece++){
        occupancy[WHITE] |= pieces[piece];
        occupancy[BLACK] |= pieces[piece + 6];
        occupancy[BOTH] |= occupancy[WHITE];
        occupancy[BOTH] |= occupancy[BLACK];
    }

}
