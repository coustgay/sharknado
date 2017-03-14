Sharknado: Madison Lee and Chris Couste

Set 10: Group Member Contributions:
Our group worked together on all changes theoretically, then committed them
through one computer to prevent inconsistency, used other computer for part 2
to make it seem balanced. Chris wrote up the bulk of minimax and the framework for
our entire Othello AI, and Madison added in some of the week 2 improvements. Both
of us helped each other work through all parts of the project.


AI improvements:
We first implemented alpha-beta pruning on a negamax search tree to speed up the
search. We then adjusted the scoring heuristic to one that was defined based on
the board state and weighted according to different characteristics of the board.
We applied iterative deepening to the choose_move function so that we could
search deeper given the alloted time, and also ordered the valid_moves vector
by best move in the 2-ply search to speed up the search of deeper plys. I think
these strategies will be effective because they help to speed up the computational
time, allowing us to search deeper into the tree and better control the flow of
the game. Increased efficiency in tandem with the iterative deepening should
give us a better result than last week. We tried looking at opening books and
transposition tables but decided we didn't have the time to implement them
properly. Also added in alternate endgame scoring mechanisms and killer moves
that will capture corners always.
