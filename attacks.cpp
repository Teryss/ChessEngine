#include "attacks.h"
#include <cstring>
#include <cstdio>
static const U64 not_h_file = 9187201950435737471ULL;
static const U64 not_a_file = 18374403900871474942ULL;
static const U64 not_ab_file = 18229723555195321596ULL;
static const U64 not_gh_file = 4557430888798830399ULL;

U64 find_magic_number(int square, int relevant_bits, int bishop, Attack &attack_masks, RandomNumber &num){
    U64 occupancies[4096];
    U64 attacks[4096];
    U64 used_attacks[4096];
    U64 attack_mask = bishop ? attack_masks.bishop_masks[square] : attack_masks.rook_masks[square];
    int occupancy_indicies = 1 << relevant_bits;
    for (int index = 0; index < occupancy_indicies; index++)
    {
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);
        attacks[index] = bishop ? mask_bishop_attacks_on_fly(square, occupancies[index]) :
                                    mask_rook_attacks_on_fly(square, occupancies[index]);
    }

    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        U64 magic_number = num.gen_magic_num();
        if (COUNT_BITS((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        int index, fail;
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            if (used_attacks[magic_index] == 0ULL)
                used_attacks[magic_index] = attacks[index];
            else if (used_attacks[magic_index] != attacks[index])
                fail = 1;
        }

        if (!fail)
            return magic_number;
    }
    printf("Magic number fails!\n");
    return 0ULL;
}

RandomNumber::RandomNumber(){
    state = 1804289383;
}

unsigned int RandomNumber::get_random_U32_num(){
    unsigned int num = state;
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;
    state = num;
    return num;
}

U64 RandomNumber::get_random_U64_num(){
    U64 n1, n2, n3, n4;
    n1 = (U64)(get_random_U32_num()) & 0xFFFF;
    n2 = (U64)(get_random_U32_num()) & 0xFFFF;
    n3 = (U64)(get_random_U32_num()) & 0xFFFF;
    n4 = (U64)(get_random_U32_num()) & 0xFFFF;
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 RandomNumber::gen_magic_num(){
    return RandomNumber::get_random_U64_num() & RandomNumber::get_random_U64_num() & RandomNumber::get_random_U64_num();
}

U64 Attack::mask_pawn_attacks(int side, int sqr){
    U64 bitboard = 0ULL, attacks = 0ULL;
    SET_BIT(bitboard, sqr);

    if (!side){
        if (bitboard & not_a_file)
            attacks |= (bitboard >> 9);
        if (bitboard & not_h_file)
            attacks |= (bitboard >> 7);
    }else{
        if (bitboard & not_a_file)
            attacks |= (bitboard << 7);
        if (bitboard & not_h_file)
            attacks |= (bitboard << 9);
    }

    return attacks;
}

U64 Attack::mask_knight_attacks(int sqr){
    U64 bitboard = 0ULL, attacks = 0ULL;
    SET_BIT(bitboard, sqr);
    if (bitboard & not_ab_file){
        attacks |= (bitboard >> 10);
        attacks |= (bitboard << 6);
    }
    if (bitboard & not_gh_file){
        attacks |= (bitboard >> 6);
        attacks |= (bitboard << 10);
    }
    if (bitboard & not_a_file){
        attacks |= (bitboard << 15);
        attacks |= (bitboard >> 17);
    }
    if (bitboard & not_h_file){
        attacks |= (bitboard >> 15);
        attacks |= (bitboard << 17);
    }
    return attacks;
}

U64 Attack::mask_king_attacks(int sqr){
    U64 bitboard = 0ULL, attacks = 0ULL;
    SET_BIT(bitboard, sqr);
    if (bitboard & not_a_file){
        attacks |= bitboard << 7;
        attacks |= bitboard >> 9;
        attacks |= bitboard >> 1;
    }
    if (bitboard & not_h_file){
        attacks |= bitboard >> 7;
        attacks |= bitboard << 1;
        attacks |= bitboard << 9;
    }
    if (bitboard << 8)
        attacks |= bitboard << 8;
    if (bitboard >> 8)
        attacks |= bitboard >> 8;
    return attacks;
}

U64 Attack::mask_bishop_attacks(int sqr){
    U64 attacks = 0ULL;
    int r, f;
    int tr = sqr / 8;
    int tf = sqr % 8;

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); }

    return attacks;
}

U64 Attack::mask_rook_attacks(int sqr){
    U64 attacks = 0ULL;
    int r, f;
    int tr = sqr / 8;
    int tf = sqr % 8;

    for (r = tr + 1, f = tf; r < 7; r++) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr - 1, f = tf; r > 0; r--) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr, f = tf + 1; f < 7; f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); }
    for (r = tr, f = tf - 1; f > 0; f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); }

    return attacks;
}

U64 mask_rook_attacks_on_fly(int sqr, U64 blocker){
    U64 attacks = 0ULL;
    int r, f;
    int tr = sqr / 8;
    int tf = sqr % 8;

    for (r = tr + 1, f = tf; r < 8; r++) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr - 1, f = tf; r > -1; r--) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr, f = tf + 1; f < 8; f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr, f = tf - 1; f > -1; f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}

    return attacks;
}

U64 mask_bishop_attacks_on_fly(int sqr, U64 blocker){
    U64 attacks = 0ULL;
    int r, f;
    int tr = sqr / 8;
    int tf = sqr % 8;

    for (r = tr + 1, f = tf + 1; r < 8 && f < 8; r++, f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr + 1, f = tf - 1; r < 8 && f > -1; r++, f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr - 1, f = tf + 1; r > -1 && f < 8; r--, f++) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}
    for (r = tr - 1, f = tf - 1; r > -1 && f > -1; r--, f--) { attacks |= (1ULL << FR_TO_SQR(r,f)); if ((1ULL << FR_TO_SQR(r, f)) & blocker){ break;}}

    return attacks;
}

void Attack::init_slider_piece(int sqr, int bishop){
    U64 attack_mask = bishop ? bishop_masks[sqr] : rook_masks[sqr];
    int relevant_bits = COUNT_BITS(attack_mask);
    int occupancy_indicies = (1 << relevant_bits);

    for (int indx = 0; indx < occupancy_indicies; indx++){
        if (bishop){
            U64 occupancy = set_occupancy(indx, relevant_bits, attack_mask);
            int magic_index = (occupancy * bishop_magics[sqr]) >> (64 - bishop_relevant_occupancy_bits[sqr]);
            bishop_attacks[sqr][magic_index] = mask_bishop_attacks_on_fly(sqr, occupancy);
        }else{
            U64 occupancy = set_occupancy(indx, relevant_bits, attack_mask);
            int magic_index = (occupancy * rook_magics[sqr]) >> (64 - rook_relevant_occupancy_bits[sqr]);
            rook_attacks[sqr][magic_index] = mask_rook_attacks_on_fly(sqr, occupancy);
        }
    }
}

Attack::Attack(){
    for (int sqr = 0; sqr < 64; sqr++){
        pawn_attacks[WHITE][sqr] = mask_pawn_attacks(WHITE, sqr);
        pawn_attacks[BLACK][sqr] = mask_pawn_attacks(BLACK, sqr);
        knight_attacks[sqr] = mask_knight_attacks(sqr);
        king_attacks[sqr] = mask_king_attacks(sqr);
        bishop_masks[sqr] = mask_bishop_attacks(sqr);
        rook_masks[sqr] = mask_rook_attacks(sqr);

        Attack::init_slider_piece(sqr, BISHOP);
        Attack::init_slider_piece(sqr, ROOK);
    }
}

U64 set_occupancy(int indx, int bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;

    for (int count = 0; count < bits_in_mask; count++){
        int lsb_index = GET_INDX_LSB(attack_mask);
        POP_BIT(attack_mask, lsb_index);

        if (indx & (1 << count))
            occupancy |= (1ULL << lsb_index);
    }

    return occupancy;
}
