#include "ConnectFour.h"


ConnectFour::ConnectFour()
{
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

//
// make an X or an O
//
Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "o.png" : "x.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard()
{
    // set up the 7 x 6 Connect 4 board
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");
    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    // do nothing if the bit is taken
    if (holder.bit()) {
        return false;
    }
    
    // get pointer for lowest non-taken square in a column
    BitHolder *lowest_holder = nullptr;
    int hold_x = static_cast<int>((holder.getPosition().x - 80/2) / 80);
    int hold_y = static_cast<int>((holder.getPosition().y - 80/2) / 80);
    std::cout << hold_x << "," << hold_y << " | " << _grid->getIndex(hold_x, hold_y) << std::endl;
    for (int y = _grid->getHeight()-1; y >= 0; y--) {
        BitHolder *curr_square = _grid->getSquare(hold_x, y);
        if (curr_square->empty()) {
            lowest_holder = curr_square;
            break;
        }
    }

    // place bit at pointer
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit && lowest_holder) {
        BitHolder &place_holder = *lowest_holder;
        bit->setPosition(place_holder.getPosition());
        place_holder.setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* ConnectFour::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 7, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

//
// helper function to traverse board during winner check
// converts 4x4 index value to true grid index value
//
int ConnectFour::getUnscopedIndex(int x, int y, int index)
{
    return _grid->getIndex(x + index % 4, y + index / 4);
}

Player* ConnectFour::checkForWinner()
{
    static const int WinQuad[10][4] =  { {0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15},  // rows
                                          {0,4,8,12}, {1,5,9,13}, {2,6,10,14}, {3,7,11,15},  // cols
                                          {0,5,10,15}, {3,6,9,12} };                         // diagonals
    
    //iterate for each possible quad within the 6 x 7 boardspace
    for( int x = 0; x <= _grid->getWidth() - 4; x++ ) {
        for( int y = 0; y <= _grid->getHeight() - 4; y++ ) {
            // std::cout << (x) << "," << (y) << " to " << (x+(15%4)) << "," << (y+(15/4)) << std::endl;
            // within each quad, check for any matches
            for( int i=0; i<10; i++ ) {
                const int *quad = WinQuad[i];
                Player *player = ownerAt(getUnscopedIndex(x, y, quad[0]));
                if( player && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[1])) && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[2])) && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[3]))
                ) {
                    std::cout << getUnscopedIndex(x, y, quad[0]) << getUnscopedIndex(x, y, quad[1]) << getUnscopedIndex(x, y, quad[2]) << getUnscopedIndex(x, y, quad[3]) << std::endl;
                    return player;
                }       
            }
        }
    }
    return nullptr;
}

bool ConnectFour::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state strings
//
std::string ConnectFour::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string ConnectFour::stateString()
{
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[_grid->getIndex(x, y)] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = _grid->getIndex(x, y);
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}


//
// this is the function that will be called by the AI
//
void ConnectFour::updateAI() 
{
    // RANDOM AI
    std::string open = "";
    for (int i = 0; i < 42; i++) {
        if (ownerAt(i) == nullptr) {
            open += char(i);
        }
    }

    int index = open[rand() % open.length()];
    int xcol = index % 7;
    int ycol = index / 7;
    BitHolder &holder = *_grid->getSquare(xcol, ycol);
    actionForEmptyHolder(holder);

    // int bestVal = -1000;
    // BitHolder* bestMove = nullptr;
    // std::string state = stateString();

    // // Traverse all cells, evaluate minimax function for all empty cells
    // _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
    //     int index = y * 3 + x;
    //     // Check if cell is empty
    //     if (state[index] == '0') {
    //         // Make the move
    //         state[index] = '2';
    //         int moveVal = -negamax(state, 0, HUMAN_PLAYER);
    //         // Undo the move
    //         state[index] = '0';
    //         // If the value of the current move is more than the best value, update best
    //         if (moveVal > bestVal) {
    //             bestMove = square;
    //             bestVal = moveVal;
    //         }
    //     }
    // });


    // // Make the best move
    // if(bestMove) {
    //     if (actionForEmptyHolder(*bestMove)) {
    //     }
    // }
}

bool ConnectFour::isAIBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

int ConnectFour::evaluateAIBoard(const std::string& state) {
    static const int kWinningTriples[8][3] =  { {0,1,2}, {3,4,5}, {6,7,8},  // rows
                                                {0,3,6}, {1,4,7}, {2,5,8},  // cols
                                                {0,4,8}, {2,4,6} };         // diagonals
    for( int i=0; i<8; i++ ) {
        const int *triple = kWinningTriples[i];
        char first = state[triple[0]];
        if( first != '0' && first == state[triple[1]] && first == state[triple[2]] ) {
            return 10;   // someone won, negamax will handle who
        }
    }
    return 0; // No winner
}

//
// player is the current player's number (AI or human)
//
int ConnectFour::negamax(std::string& state, int depth, int playerColor) 
{
    int score = evaluateAIBoard(state);

    // Check if AI wins, human wins, or draw
    if(score) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
        return -score; 
    }

    if(isAIBoardFull(state)) {
        return 0; // Draw
    }

    int bestVal = -1000; // Min value
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            // Check if cell is empty
            if (state[y * 3 + x] == '0') {
                // Make the move
                state[y * 3 + x] = playerColor == HUMAN_PLAYER ? '1' : '2'; // Set the cell to the current player's color
                bestVal = std::max(bestVal, -negamax(state, depth + 1, -playerColor));
                // Undo the move for backtracking
                state[y * 3 + x] = '0';
            }
        }
    }

    return bestVal;
}