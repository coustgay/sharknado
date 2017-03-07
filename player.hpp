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

    std::string print_side(Side side){
        if (side == WHITE) {
            return "White";
        } else {
            return "Black";
        }
    }

    Side opp(Side side) {return (Side) ((side + 1) % 2);}

    // Flag to tell if the player is running within the test_minimax context
    bool minimaxTest;
};

#endif
