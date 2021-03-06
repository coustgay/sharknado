#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <vector>
#include <list>
#include "common.hpp"
#include "board.hpp"

using namespace std;

class Player {

public:
    Player(Side color);
    ~Player();

    Board *board;
    Side side;
    Move *doMove(Move *opponentsMove, int msLeft);

    // -------------- optimizing valid move finder --------- //
    std::vector<Move> past_moves;
    std::vector<Move> adjacents;
    std::vector<Move> valid_moves(Board *board, Side side, bool eff);

    // -------------- optimizing move chooser -------------- //
    Move *choose_move(Board *board, Side side, std::vector<Move> valid_moves, std::list<Move>& ordered_moves, int plys, time_t end_time, bool& timeout);
    int getScore(Board *board, Side side);
    int alphaBeta(Board *board, Side side, int& a, int& b, int plys, time_t end_time, bool& timeout);

    // returns a string describing the input side object
    const char * print_side(Side side){
        if (side == WHITE) {
            return "White";
        } else {
            return "Black";
        }
    }

    // returns the opposing side from the one provided
    // probably unnecessary if we just calculate it at the start
    Side opp(Side side) {return side == WHITE ? BLACK : WHITE;}

    // Flag to tell if the player is running within the test_minimax context
    bool minimaxTest;
};

#endif
