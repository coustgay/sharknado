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
    /**
    for (int q = -1; q < 2; q++) {
        for (int w = -1; w < 2; w++) {
            Move adjacent(0,0);
            adjacent.setX(q); adjacent.setY(w);
            adjacents.push_back(adjacent);
        }
    } */
    fprintf(stderr, "Sharknado is on color %s!\n", print_side(side));
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
    Side opp_side = (Side) ((side + 1) % 2);
    if (opponentsMove != nullptr){

        board->doMove(opponentsMove, opp_side);
        fprintf(stderr, "%s's move: %d %d\n",
                print_side(opp_side), opponentsMove->getX(), opponentsMove->getY());
    } else {
        fprintf(stderr, "%s has no valid moves~\n", print_side(opp_side));
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

    /** drafting alternative valid move checker; need to resolve going out of
        range at boundaries
    if (past_moves.size() < 1){
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                move.setX(i); move.setY(j);
                valid_moves.push_back(move);
            }
        }
    }

    else {
        for (unsigned int i = 0; i < past_moves.size(); i++)
        {
            for (unsigned int j = 0; j < adjacents.size(); j++)
            {
                move.setX(past_moves[i].getX() + adjacents[j].getX());
                move.setY(past_moves[i].getY() + adjacents[j].getY());
                if (board->checkMove(&move, side)) {
                    valid_moves.push_back(move);
                }
            }
        }
    }
    */


    //--------------base case-------------//

    // in case there arent valid moves, this is faster than using their method
    // to check first b/c we don't have to iterate through the whole board again
    if (valid_moves.size() <= 0)
        return nullptr;


    // make variables for testing moves. everything is a pointer except scores
    Board *next_board;
    Move next_move(0,0);
    Move best_move = valid_moves[0];
    int best_score = -64;
    int next_score;

    //--------------choosing moves-------------//
    // uses a 2-ply minimax decision tree and the naive heuristic for testing
    if (minimaxTest)
    {
        for (unsigned int k = 0; k < valid_moves.size(); k++)
        {
            next_board = board->copy();
            next_move = valid_moves[k];
            next_board->doMove(&next_move, side);
            next_score = next_board->count(side) - next_board->count(opp_side);

            // generating valid moves for first layer in
            std::vector<Move> next_valid_moves;
            Move move(0,0);
            for (int i = 0; i < 8; i++){
                for (int j = 0; j < 8; j++){
                    move.setX(i); move.setY(j);
                    if (next_board->checkMove(&move, opp_side)) {
                        next_valid_moves.push_back(move);
                    }
                }
            }
            for (unsigned int i = 0; i < next_valid_moves.size(); i++)
            {
                Board *next_next_board = next_board->copy();
                Move next_next_move(0,0);
                next_next_move = next_valid_moves[i];
                next_next_board->doMove(&next_next_move, opp_side);
                int next_next_score = next_board->count(opp_side) - next_board->count(side);
                next_score += next_next_score;
            }
            if (next_score > best_score)
            {
                best_move = next_move;
                best_score = next_score;
            }
        }
    }

    // alternative to 2-ply minimax / naive heuristic (better heuristic?)
    else
    {
        // find the naive best choice (basic heuristic) ~ update with minimax later
        for (unsigned int k = 0; k < valid_moves.size(); k++){

            // for each valid move, initialize our variables
            next_board = board->copy();
            next_move = valid_moves[k];
            next_board->doMove(&next_move, side);
            next_score = next_board->count(side) - next_board->count(opp_side);

            // sanity check
            //fprintf(stderr, "Investigating %d %d ... score: %d\n",
            //        next_move.getX(), next_move.getY(), next_score);

            // check if we should use the current choice instead of our previous
            if (next_score > best_score &&
                next_move.getX() != 1 && next_move.getX() != 6 &&
                next_move.getY() != 1 && next_move.getY() != 6){
                best_move = next_move;
                best_score = next_score;
            }
        }
    }


    //--------------finish using chosen move-------------//

    // decided on best move! put it in a pointer to be passed
    Move *final_move = new Move(best_move.getX(), best_move.getY());
    fprintf(stderr, "%s's move: %d %d\n",
            print_side(side), best_move.getX(), best_move.getY());

    //before we return, update the board with our move
    past_moves.push_back(*final_move);
    board->doMove(final_move, side);
    return final_move;
}
