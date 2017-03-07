#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <vector>
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
    std::vector<Move> past_moves;
    std::vector<Move> adjacents;

    // returns a string describing the input side object
    const char * print_side(Side side){
        std::string color = "Black";
        if (side == WHITE) {color = "White";
        }
        return color.c_str();
    }

    // returns the opposing side from the one provided
    // probably unnecessary if we just calculate it at the start
    // Side opp(Side side) {return (Side) ((side + 1) % 2);}

    // Flag to tell if the player is running within the test_minimax context
    bool minimaxTest;
};

#endif
