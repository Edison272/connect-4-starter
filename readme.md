Using the provided code that can be downloaded from this github add a new class that inherits from game.cpp in the same way TicTacToe.cpp does and implement a working version of the game Connect 4. The game should be added as a fourth choice from the menu so the choices are Tic Tac Toe, Checkers, Othello and now Connect 4.

The game must be playable by both by 2 people and vs. an AI. Your implementation must check for the winning condition, and display that on the right hand side the same way the current setup does for tic-tac-toe. The stopGame() method is expected to clean up and delete any memory allocated.

Look at the new Grid.cpp class and see how it is used in Checkers.cpp for a good understanding about laying out a useable grid.

For extra credit, when a game piece is added to the board, make it animate into place instead of just appearing in it's final spot.

Graphic files are provided for the game pieces called yellow.png and red.png.

For the submission, create a new github based on the above code-base and provide a link to that along with a complete readme.md on how your implementation works.


NEGAMAX IMPLEMENTATION

The connect four negamax code relies on the negamax, evaluateAIBoard, and evaluateAIWindow functions to work properly

The connect 4 negamax function works similar to the tictactoe negamax, in that it analyzes all possible outcomes of a game before choosing an option. However, the difference between tic tac toe and connect 4 is that while it only takes a couple of seconds for a recursive function to reach the end of a tictactoe game, a connect 4 game is far to complex for such a possibility to be viable. Instead, the connect 4 negamax algorithm uses depth to cut the recursion short, and see whether the board position its left with is advantagious or not.

This means that unlike tictactoe where the negamax scores boardstates based on a win/loss or a draw, the AI must be much more specific.

In order to determine who's winning and who isn't, the AI uses the evaluateAIBoard function to judge how advantagious a board state is from the perspective of a given player (so it needs the current player's piece as a parameter). This function iterates through all connections between pieces (vertical, horizontal, TL to BR diagonals, and BL to TR diagonals), and uses the helper function evaluateAIWindow function to assign points.

The evaluation algorithm assigns a score based on how much they help or hurt the current player's objective. the algorithm will award points the more a current player has more pieces in a row, and will negate points the more an opposing has several pieces in a row. However, it must me noted that the algorithm will only award or negate points if these connections have potential to become 4-in-a-row connections. If they are just two pieces sandwiched between two opposing pieces, its useless.

- 2-in-a-row for the current player (with 2 blank spaces) will increases the score very slightly (10)
- 3-in-a-row for the current player (with 1 blank spaces) will increases the score moderately (100)
- 3-in-a-row for the opposing player (with 1 blank spaces) will decrease the score slightly more (-120)
    - the algorithm removes MORE score comparatively when an opposing player has 3 in a row, so that the AI will always try to deny enemy opportunities unless victory is absolutely assured
    - it's a bad idea to decrease player score for opponent 2-in-a-row connections, because the AI becomes too focused on defense to set up any kind of advantage
- 4-in-a-row for the opposing player will decrease the score by a massive amount (-90000)
- 4-in-a-row for the current player will increase the score by an even more massive amount (100000)
    - unlike for catching opposing player 3-in-a-row connects, a current piece 4 in a row is weighed significantly higher so that the AI will always go for the win


The evaluation AI uses this scoring metric to create a cumulative score that represents ALL advantages and disadvantages the current player faces on the board.
this score value is returned to the minimax function.

the negamax function's only terminal state is depth, If board state is full (draw), the depth is immediately set to 0. Once the depth is 0, the negamax function returns the most highest score from the perspective of each player, and the AI will make a move accordingly.



