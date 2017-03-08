#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The color your AI is
 * on (BLACK or WHITE) is passed in as "color". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side color) {
    // Will be set to true in test_minimax.cpp.
    minimaxTest = false;

    // initialize board and side
    board = new Board();
    side = color;
    
    // prepare for quick way of finding valid moves
    for (int q = -1; q < 2; q++) {
        for (int w = -1; w < 2; w++) {
            Move adjacent(0,0);
            adjacent.setX(q); adjacent.setY(w);
            adjacents.push_back(adjacent);
        }
    }
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

    // --------------- update opponent's move ----------------- //
    Side opp_side = (Side) ((side + 1) % 2);
    if (opponentsMove != nullptr){
        board->doMove(opponentsMove, opp_side);
        past_moves.push_back(*opponentsMove);
        fprintf(stderr, "---------------------------------\n%s's move: %d %d\n",
                print_side(opp_side), opponentsMove->getX(), opponentsMove->getY());
    } else {
        fprintf(stderr, "%s has no valid moves~\n", print_side(opp_side));
    }

    //--------------find moves------------------//

    std::vector<Move> valid_moves = this->valid_moves(board, side, false);
    fprintf(stderr, "%s:%lu legal moves.\n", print_side(side), valid_moves.size());

    
    //--------------choosing moves-------------//

    Move *best_move = this->choose_move(board, side, valid_moves, 2);

    //--------------finish using chosen move-------------//
    if (best_move != nullptr){
        fprintf(stderr, "%s's move: %d %d\n-----------------------------------\n",
                print_side(side), best_move->getX(), best_move->getY());
        past_moves.push_back(*best_move);
    }
    //before we return, update the board with our move
    board->doMove(best_move, side);
    return best_move;
}

/*
 * @brief provides a list of valid moves
 * 
 *  @in board state to evaluate, side of player to make move 
 *  eff == false uses basic method instead of fancy efficient method
 *
 */
std::vector<Move> Player::valid_moves(Board *board, Side side, bool eff) {    
    Move move(0,0); std::vector<Move> out;
    if (eff){
        // drafting alternative valid move checker; 
        // need to resolve going out of range at boundaries
        if (past_moves.size() < 1){
            for (int i = 0; i < 8; i++){
                for (int j = 0; j < 8; j++){
                    move.setX(i); move.setY(j);
                    out.push_back(move);
                }
            }
        } else {
            for (unsigned int i = 0; i < past_moves.size(); i++)
            {
                for (unsigned int j = 0; j < adjacents.size(); j++)
                {
                    move.setX(past_moves[i].getX() + adjacents[j].getX());
                    move.setY(past_moves[i].getY() + adjacents[j].getY());
                    if (board->checkMove(&move, side)) {
                        out.push_back(move);
                    }
                }
            }
        }
        // simple method
    } else {
        //fprintf(stderr, "checking for valid moves: \n");
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                move.setX(i); move.setY(j);
                if (board->checkMove(&move, side)) {
                    out.push_back(move);
                    //fprintf(stderr, "%d %d works!\n", i, j);
                }
            }
        }
    }
    //fprintf(stderr, "%s has %lu options\n", print_side(side), out.size());
    return out;
}

/*
 *  @brief picks a good move and returns a pointer to it
 * 
 *  @arguments:
 *  board state, side of player to make move, vector of valid moves
 *  iteration level (ply) (not implemented)
 *
 */
Move *Player::choose_move(Board *board, Side side, std::vector<Move> valid_moves, int plys) {

    // if the provided move vector is empty, we can't do anything
    if (valid_moves.size() < 1) {
        return nullptr;
    }

    Side opp_side = (Side) ((side + 1) % 2);
    Board *next_board;
    Move next_move(0,0);
    Move *opp_move;
    std::vector<Move> opp_moves;
    Move best_move = valid_moves[0];
    int best_score = -64;
    int next_score;

    // case where we have more layers to search - recurses deeper with plys-1
    if (plys > 1){
        for (unsigned int i = 0; i < valid_moves.size(); i++) {
            // update board copy with new move
            next_board = board->copy();
            next_move = valid_moves[i];
            next_board->doMove(&next_move, side);
            //fprintf(stderr, "if %s tries %d %d\n", 
            //        print_side(side), next_move.getX(), next_move.getY());

            // find opponent's likely counter move
            opp_moves = this->valid_moves(next_board, opp_side, false);
            opp_move = this->choose_move(next_board, opp_side, opp_moves, plys - 1);

            if (opp_move == nullptr){
                // if the opponent can't go, we do our next move (dont get confused)
                opp_moves = this->valid_moves(next_board, side, false);
                opp_move = this->choose_move(next_board, side, opp_moves, plys - 1);

                // now score the result of both of our moves
                next_board->doMove(opp_move, side);
                next_score = next_board->count(side) - next_board->count(opp_side);
            
            } else {
                // score our move based on outcome of opp's move
                next_board->doMove(opp_move, opp_side);
                next_score = next_board->count(side) - next_board->count(opp_side);
            }

            // decide if this option is better than any others
            if (next_score >= best_score) {
                best_move = next_move;
                best_score = next_score;
            }
        }
        Move *final_move = new Move(best_move.getX(), best_move.getY());
        return final_move;
    // case where this is the last layer to search - two heuristic options
    } else {

        // uses an in-place 2-ply minimax decision tree and naive heuristic for test
        if (minimaxTest)
        {
            for (unsigned int k = 0; k < valid_moves.size(); k++)
            {
                next_board = board->copy();
                next_move = valid_moves[k];
                next_board->doMove(&next_move, side);
                next_score = next_board->count(side) - next_board->count(opp_side);
    
                // generating valid moves for first layer in
                std::vector<Move> next_valid_moves = 
                    this->valid_moves(next_board, opp_side, false);
    
                // calculates for second layer
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
        // alternative to static 2-ply minimax / naive heuristic (better heuristic?)
        } else {
            for (unsigned int k = 0; k < valid_moves.size(); k++){
    
                // for each valid move, initialize our variables
                next_board = board->copy();
                next_move = valid_moves[k];
                //int x = next_move.getX(); int y = next_move.getY();
                next_board->doMove(&next_move, side);
                
                // naieve heuristic
                next_score = next_board->count(side) - next_board->count(opp_side);

                /*// tune score based on meta strategy checks
                if (x == y || x + y == 7){
                    if (x == 0 || x == 7){
                        next_score *= 10;
                    } else {
                        next_score *= 3;
                    }
                }*/
    
                // check if we should use the current choice instead of our previous
                if (next_score > best_score){
                    best_move = next_move;
                    best_score = next_score;
                }
            }
        }
        Move *final_move = new Move(best_move.getX(), best_move.getY());
        //fprintf(stderr, "%s would pick %d %d\n", 
        //        print_side(side), best_move.getX(), best_move.getY());
        return final_move;
    }
}

// ------- drafting timing calls -----------------//
    // just interested in the timer, I don't think we have to worry about it this week
    // std::time_t end_turn = std::time(nullptr) + (time_t) (msLeft / 1000);