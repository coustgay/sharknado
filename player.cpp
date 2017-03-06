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
     * process the opponent's opponents move before calculating your own move
     */

    // update opponent's move into internal board state
    if (side == BLACK){
        board->doMove(opponentsMove, WHITE);
    } else {board->doMove(opponentsMove, BLACK);}
    fprintf(stderr, "Completed opponent's move~\n");

    //--------------find moves------------------//

    std::vector<Move> valid_moves;
    Move *move = new Move(0,0);
    fprintf(stderr, "checking moves: \n");

    // check the whole board (can make more efficient later)
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            move->setX(i); move->setY(j);
            if (board->checkMove(move, side)) {
                // using moves instead of pointers in the vector because of errors
                Move *valid = new Move(i,j);
                valid_moves.push_back(*valid);
                fprintf(stderr, "%d %d works!\n", i, j);
            }
        }
    }

    //--------------base cases-------------//

    // in case there arent valid moves, this is faster than using their method
    // to check first b/c we don't have to iterate through the whole board again
    if (valid_moves.size() <= 0) {return nullptr;}
    // if there's only one choice, do it
    if (valid_moves.size() == 1) {return &valid_moves[0];}

    //--------------choosing moves-------------//

    // make variables for testing moves. everything is a pointer except scores
    Board *next_board = board->copy(); Move *next_move = &valid_moves[0];
    Move *best_move; int best_score = -64;  int next_score;

    // find the naive best choice (basic heuristic) ~ update with minimax later
    for (unsigned int k = 0; k < valid_moves.size(); k++){

        // for each valid move, initialize our variables
        next_move = &valid_moves[k];
        next_board->doMove(next_move, side);
        next_score = next_board->count(side);

        // sanity check
        fprintf(stderr, "Investigating %d %d ...\n", 
                next_move->getX(), next_move->getY());

        // check if we should use the current choice instead of our previous
        if (next_score >= best_score){
            delete best_move;
            best_move = next_move;
            best_score = next_score;
        } else {
            delete next_move;
        }
    }
    return best_move;
}
