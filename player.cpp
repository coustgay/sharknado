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
    Side opp_side = side == WHITE ? BLACK : WHITE;
    if (opponentsMove != nullptr){
        board->doMove(opponentsMove, opp_side);
        past_moves.push_back(*opponentsMove);
        fprintf(stderr, "-----------------------------------\n%s's move: %d %d\n",
                print_side(opp_side), opponentsMove->getX(), opponentsMove->getY());
    } else {
        fprintf(stderr, "%s has no valid moves~\n", print_side(opp_side));
    }

    // display current score
    fprintf(stderr, "prev score: %s: %d to %s: %d\n", print_side(side),
            board->count(side), print_side(opp_side), board->count(opp_side));

    //--------------find moves and choose one------------------//

    std::vector<Move> valid_moves = this->valid_moves(board, side, false);
    Move *best_move = this->choose_move(board, side, valid_moves, 5);

    // display new move, if it's not pass, add it to past moves
    if (best_move != nullptr){
        fprintf(stderr, "%s's move: %d %d\n",
                print_side(side), best_move->getX(), best_move->getY());
        past_moves.push_back(*best_move);
    } else {
        fprintf(stderr, "%s has to pass!\n",
                print_side(side));
    }

    //------------- update board with chosen move! ----------------//
    board->doMove(best_move, side);
    fprintf(stderr, "new  score:  %s: %d to %s: %d\n-----------------------------------\n",
            print_side(side), board->count(side), print_side(opp_side), board->count(opp_side));
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
Move *Player::choose_move(Board *board, Side side, std::vector<Move> valid_moves, int plys)
{
    // if the provided move vector is empty, we can't do anything
    if (valid_moves.size() < 1)
    {
        return nullptr;
    }

    Side opp_side = this->opp(side);
    Board *next_board;
    Move next_move(0,0);
    // Move *opp_move;
    // std::vector<Move> opp_moves;
    Move best_move = valid_moves[0];
    int best_score = 100;
    int next_score;
    int a = -100;
    int b = 100;

    for (unsigned int i = 0; i < valid_moves.size(); i++)
    {
        // update board copy with new move
        next_board = board->copy();
        next_move = valid_moves[i];
        next_board->doMove(&next_move, side);

        next_score = this->alphaBeta(next_board, opp_side, a, b, plys);

        // decide if this option is better than any others
        if (next_score <= best_score) {
            best_move = next_move;
            best_score = next_score;
        }
    }
    Move *final_move = new Move(best_move.getX(), best_move.getY());
    return final_move;
}

/*
 *  @brief returns the score of the maximizing player based on the current
*   state of the board
 *
 *  @arguments:
 *  board state, side of maximizing player
 *
 */
int Player::getScore(Board *board, Side side)
{
    // TODO: add in a way to account for minimizing adjacent squares using the adjacents vector idk
    int score, board_count, board_opp_count, diff_score;
    int corner_score = 0, moves_score = 0, edge_score = 0, near_corner_score = 0;
    Side opp_side = opp(side);
    board_count = board->count(side);
    board_opp_count = board->count(opp_side);
    diff_score = (board_count - board_opp_count) / (board_count + board_opp_count);
    diff_score *= 100;
    std::vector<Move> total_moves = valid_moves(board, side, false);
    std::vector<Move> total_opp_moves = valid_moves(board, opp_side, false);
    int total = total_moves.size() + total_opp_moves.size();
    if (total != 0)
    {
        moves_score = (total_moves.size() - total_opp_moves.size()) / total;
    }
    moves_score *= 100;

    //corner checking
    if (board->checkSquare(side, 0, 0)) corner_score++;
    if (board->checkSquare(side, 0, 7)) corner_score++;
    if (board->checkSquare(side, 7, 0)) corner_score++;
    if (board->checkSquare(side, 7, 7)) corner_score++;
    if (board->checkSquare(opp_side, 0, 0)) corner_score--;
    if (board->checkSquare(opp_side, 0, 7)) corner_score--;
    if (board->checkSquare(opp_side, 7, 0)) corner_score--;
    if (board->checkSquare(opp_side, 7, 7)) corner_score--;

    // edge checking
    for (int i = 2; i < 6; i++)
    {
        if (board->checkSquare(side, i, 0)) edge_score++;
        if (board->checkSquare(side, i, 7)) edge_score++;
        if (board->checkSquare(side, 0, i)) edge_score++;
        if (board->checkSquare(side, 7, i)) edge_score++;
        if (board->checkSquare(opp_side, i, 0)) edge_score--;
        if (board->checkSquare(opp_side, i, 7)) edge_score--;
        if (board->checkSquare(opp_side, 0, i)) edge_score--;
        if (board->checkSquare(opp_side, 7, i)) edge_score--;
    }

    // near corner checking
    if (board->checkSquare(side, 1, 1)) near_corner_score -= 4;
    if (board->checkSquare(side, 1, 6)) near_corner_score -= 4;
    if (board->checkSquare(side, 6, 1)) near_corner_score -= 4;
    if (board->checkSquare(side, 6, 6)) near_corner_score -= 4;
    if (board->checkSquare(opp_side, 1, 1)) near_corner_score += 4;
    if (board->checkSquare(opp_side, 1, 6)) near_corner_score += 4;
    if (board->checkSquare(opp_side, 6, 1)) near_corner_score += 4;
    if (board->checkSquare(opp_side, 6, 6)) near_corner_score += 4;
    if (board->checkSquare(side, 1, 0)) near_corner_score --;
    if (board->checkSquare(side, 6, 0)) near_corner_score --;
    if (board->checkSquare(side, 0, 1)) near_corner_score --;
    if (board->checkSquare(side, 7, 1)) near_corner_score --;
    if (board->checkSquare(side, 0, 6)) near_corner_score --;
    if (board->checkSquare(side, 7, 6)) near_corner_score --;
    if (board->checkSquare(side, 1, 7)) near_corner_score --;
    if (board->checkSquare(side, 6, 7)) near_corner_score --;
    if (board->checkSquare(opp_side, 1, 0)) near_corner_score ++;
    if (board->checkSquare(opp_side, 6, 0)) near_corner_score ++;
    if (board->checkSquare(opp_side, 0, 1)) near_corner_score ++;
    if (board->checkSquare(opp_side, 7, 1)) near_corner_score ++;
    if (board->checkSquare(opp_side, 0, 6)) near_corner_score ++;
    if (board->checkSquare(opp_side, 7, 6)) near_corner_score ++;
    if (board->checkSquare(opp_side, 1, 7)) near_corner_score ++;
    if (board->checkSquare(opp_side, 6, 7)) near_corner_score ++;

    edge_score = 100 * edge_score / 16;
    corner_score = 100 * corner_score / 4;
    near_corner_score = 100 * near_corner_score / 24;
    score = 0.05 * diff_score + 0.2 * moves_score + 0.4 * corner_score + 0.2 * edge_score + 0.15 * near_corner_score;
    return score;
}

int Player::alphaBeta(Board *board, Side side, int& a, int& b, int plys)
{
    std::vector<Move> valid_moves = this->valid_moves(board, side, false);
    int score;
    if (plys == 0 || valid_moves.size() == 0)
    {
        score = this->getScore(board, side);
        return score;
    }
    Side opp_side = (Side) ((side + 1) % 2);
    for (unsigned int i = 0; i < valid_moves.size(); i++)
    {
        Board *next_board = board->copy();
        Move next_move(0,0);
        next_move = valid_moves[i];
        next_board->doMove(&next_move, side);
        int y = -b;
        int z = -a;
        score = -(this->alphaBeta(next_board, opp_side, y, z, plys - 1));
        if (score > a)
        {
            a = score;
        }
        if (score >= b)
        {
            return b;
        }
    }
    return a;
}



// ------- drafting timing calls -----------------//
    // just interested in the timer, I don't think we have to worry about it this week
    // std::time_t end_turn = std::time(nullptr) + (time_t) (msLeft / 1000);
