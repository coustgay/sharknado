#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The color your AI is
 * on (BLACK or WHITE) is passed in as "color". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side color) {
    // Will be set to true in test_minimax.cpp.
    minimaxTest = false;

    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */

    board = new Board();
    side = color;
    std::string side_s;
    if (side == WHITE) {
        side_s = "White";
    } else {
        side_s = "Black";
    }
    fprintf(stderr, "sharknado is on side: %s\n", side_s.c_str());
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    // just interested in the timer, I don't think we have to worry about it this week
    // std::time_t end_turn = std::time(nullptr) + (time_t) (msLeft / 1000);
    /*
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponentsMove before calculating your own move
     */

    // update opponent's move into internal board state
    if (opponentsMove != nullptr){
        Side opp_side;
        if (side == BLACK){
            opp_side = WHITE;
        } else {
            opp_side = BLACK;
        }

        board->doMove(opponentsMove, opp_side);
        fprintf(stderr, "Completed %d's move at %d %d\n",
                opp_side, opponentsMove->getX(), opponentsMove->getY());
    }

    //--------------find moves------------------//

    std::vector<Move> valid_moves;
    Move move(0,0);
    //fprintf(stderr, "checking for valid moves: \n");

    // check the whole board (can make more efficient later)
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            move.setX(i); move.setY(j);
            if (board->checkMove(&move, side)) {
                valid_moves.push_back(move);
                //fprintf(stderr, "%d %d works!\n", i, j);
            }
        }
    }

    //--------------base case-------------//

    // in case there arent valid moves, this is faster than using their method
    // to check first b/c we don't have to iterate through the whole board again
    if (valid_moves.size() <= 0)
        return nullptr;

    //--------------choosing moves-------------//

    // make variables for testing moves. everything is a pointer except scores
    Board *next_board;
    Move next_move(0,0);
    Move best_move = valid_moves[0];
    int best_score = -64;
    int next_score;

    // find the naive best choice (basic heuristic) ~ update with minimax later
    for (unsigned int k = 0; k < valid_moves.size(); k++){

        // for each valid move, initialize our variables
        next_board = board->copy();
        next_move = valid_moves[k];
        next_board->doMove(&next_move, side);
        next_score = next_board->count(side);

        // sanity check
        //fprintf(stderr, "Investigating %d %d ... score: %d\n",
        //        next_move.getX(), next_move.getY(), next_score);

        // check if we should use the current choice instead of our previous
        if (next_score >= best_score){
            best_move = next_move;
            best_score = next_score;
        }
    }

    // decided on best move! put it in a pointer to be passed
    Move *final_move = new Move(best_move.getX(), best_move.getY());
    fprintf(stderr, "Completing %d's move: %d %d\n",
            side, best_move.getX(), best_move.getY());

    //before we return, update the board with our move
    board->doMove(final_move, side);
    past_moves.push_back(*final_move);
    return final_move;
}
