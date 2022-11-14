// modern cpu instruction set for builtin functions to work faster
#pragma GCC target ("sse4")
#include <iostream>
// #include <string.h>
// #include <sstream>
#include "attacks.h"
#include "board.h"

void print_bitboard(U64 bitboard);
inline U64 Attack::get_bishop_attacks(int sqr, U64 occupancy){
    occupancy &= bishop_masks[sqr];
    occupancy *= bishop_magics[sqr];
    occupancy >>= (64 - bishop_relevant_occupancy_bits[sqr]);
    return bishop_attacks[sqr][occupancy];
}

inline U64 Attack::get_rook_attacks(int sqr, U64 occupancy){
    occupancy &= rook_masks[sqr];
    occupancy *= rook_magics[sqr];
    occupancy >>= (64 - rook_relevant_occupancy_bits[sqr]);
    return rook_attacks[sqr][occupancy];
}

inline U64 Attack::get_queen_attacks(int sqr, U64 occupancy){
    U64 rook_occupancy = occupancy, bishop_occupancy = occupancy;
    bishop_occupancy &= bishop_masks[sqr];
    bishop_occupancy *= bishop_magics[sqr];
    bishop_occupancy >>= (64 - bishop_relevant_occupancy_bits[sqr]);
    rook_occupancy &= rook_masks[sqr];
    rook_occupancy *= rook_magics[sqr];
    rook_occupancy >>= (64 - rook_relevant_occupancy_bits[sqr]);
    return (rook_attacks[sqr][rook_occupancy] | bishop_attacks[sqr][bishop_occupancy]);
}

static inline int is_sqr_attacked(int sqr, int side, Attack &at, Board &board){
    if ((side == WHITE) && (at.pawn[BLACK][sqr] & board.pieces[P])) return 1;
    if ((side == BLACK) && (at.pawn[WHITE][sqr] & board.pieces[p])) return 1;
    if (at.knight_attacks[sqr] & ((side == WHITE) ? board.pieces[N] : board.pieces[n])) return 1;
    if (at.knig_attacks[sqr] & ((side == WHITE) ? board.pieces[K] : board.pieces[k])) return 1;
    if (at.bishop_attacks[sqr][board.occupancy[BOTH]] & ((side == WHITE) ? board.pieces[B] : board.pieces[b])) return 1;
    if (at.rook_attacks[sqr][board.occupancy[BOTH]] & ((side == WHITE) ? board.pieces[R] : board.pieces[r])) return 1;
    return 0;
}

int main(){
    Attack at;
    Board board{tricky_position};
    board.print();
    U64 blocker = 0ULL;
    SET_BIT(blocker, d6);
    SET_BIT(blocker, e4);
    SET_BIT(blocker, b2);
    print_bitboard(at.get_queen_attacks(d4, blocker));
    return 0;
}

void print_bitboard(U64 bitboard){
    for (int r = 0; r < 8; r++){
        for (int f = 0; f < 8; f++){
            if (!f){
                std::cout << 8 - r << "   ";
            }
            std::cout << (GET_BIT(bitboard, FR_TO_SQR(r, f)) ? 1 : 0) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n    a b c d e f g h\n";
    std::cout << "Bitboard value: " << bitboard << std::endl;
}
