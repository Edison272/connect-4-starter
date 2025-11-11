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
    //std::cout << hold_x << "," << hold_y << " | " << _grid->getIndex(hold_x, hold_y) << std::endl;
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
// returns the bottom most empty y value of a column, given the x coordinate
//
int ConnectFour::getColumnTopFromX(int x)
{
    for (int y = _grid->getHeight()-1; y >= 0; y--) {
        BitHolder *curr_square = _grid->getSquare(x, y);
        if (curr_square->empty()) {
            return y;
        }
    }
    return -1;
}

//
// helper function to traverse board during winner check
// converts 4x4 index value to true grid index value
//
int ConnectFour::getUnscopedIndex(int x, int y, int index, int scope)
{
    return _grid->getIndex(x + index % scope, y + index / scope);
}

Player* ConnectFour::checkForWinner()
{   
    //iterate for each possible quad within the 6 x 7 boardspace
    for( int x = 0; x <= _grid->getWidth() - 4; x++ ) {
        for( int y = 0; y <= _grid->getHeight() - 4; y++ ) {
            // std::cout << (x) << "," << (y) << " to " << (x+(15%4)) << "," << (y+(15/4)) << std::endl;
            // within each quad, check for any matches
            for( int i=0; i<10; i++ ) {
                const int *quad = WinQuad[i];
                Player *player = ownerAt(getUnscopedIndex(x, y, quad[0], 4));
                if( player && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[1], 4)) && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[2], 4)) && 
                    player == ownerAt(getUnscopedIndex(x, y, quad[3], 4))
                ) {
                    std::cout << getUnscopedIndex(x, y, quad[0], 4) << getUnscopedIndex(x, y, quad[1], 4) << getUnscopedIndex(x, y, quad[2], 4) << getUnscopedIndex(x, y, quad[3], 4) << std::endl;
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
    // std::string open = "";
    // for (int i = 0; i < 42; i++) {
    //     if (ownerAt(i) == nullptr) {
    //         open += char(i);
    //     }
    // }

    // if (open.size() > 0) {
    //     int index = open[rand() % open.length()];
    //     int xcol = index % 7;
    //     int ycol = index / 7;
    //     BitHolder &holder = *_grid->getSquare(xcol, ycol);
    //     actionForEmptyHolder(holder);
    // }


    // MINIMAX AI
    int bestVal = -1000;
    BitHolder* bestMove = nullptr;
    std::string state = stateString();

    for (int i = 0; i < 7; i++) {
        // traverse all columns at their bottom most empty value and see what will happen
        int x = board_columns_sequence[i];
        int y = getColumnTopFromX(x);
        if ( y == -1) {  // skip this column if it is full
            std::cout << "next " << x << std::endl;
            continue;
        }
        int index = y * 7 + x;
        // Check if cell is empty
        if (state[index] == '0') {
            // Make the move
            state[index] = '2';
            int moveVal = -negamax(state, 6, HUMAN_PLAYER, -1000, 1000);
            // Undo the move
            state[index] = '0';
            // If the value of the current move is more than the best value, update best
            if (moveVal > bestVal) {
                bestMove = _grid->getSquare(x, y);
                bestVal = moveVal;
                std::cout << x << ", " << y << std::endl;
            }
        }
    }

    // // Make the best move
    if(bestMove) {
        if (actionForEmptyHolder(*bestMove)) {

        }
    }
}

bool ConnectFour::isAIBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

// evaulate the boardstate and return a board based on the current situation.
// scoring punishes enemy advantagious positions, so curr_piece is parameter in order to distinguish who is the one playing
int ConnectFour::evaluateAIBoard(const std::string& state, char curr_piece) {   
    int score = 0;

    // check horizontal connects
    for (int y = 0; y < _grid->getHeight(); y++) {
        for (int x = 0; x < _grid->getWidth()-3; x++) {
            std::vector<char> window = {
                state[_grid->getIndex(x, y)],
                state[_grid->getIndex(x+1, y)],
                state[_grid->getIndex(x+2, y)],
                state[_grid->getIndex(x+3, y)]
            };
            score += evaluateAIWindow(window, curr_piece);
        }
    }

    // check horizontal connects
    for (int y = 0; y < _grid->getHeight()-3; y++) {
        for (int x = 0; x < _grid->getWidth(); x++) {
            std::vector<char> window = {
                state[_grid->getIndex(x, y)],
                state[_grid->getIndex(x, y+1)],
                state[_grid->getIndex(x, y+2)],
                state[_grid->getIndex(x, y+3)]
            };
            score += evaluateAIWindow(window, curr_piece);
        }
    }

    // check diagonal connects
    // BL to TR
    for (int y = 0; y < _grid->getHeight()-3; y++) {
        for (int x = 0; x < _grid->getWidth()-3; x++) {
            std::vector<char> window = {
                state[_grid->getIndex(x, y)],
                state[_grid->getIndex(x+1, y+1)],
                state[_grid->getIndex(x+2, y+2)],
                state[_grid->getIndex(x+3, y+3)]
            };
            score += evaluateAIWindow(window, curr_piece);
        }
    }
    // TL to BR
    for (int y = 3; y < _grid->getHeight(); y++) {
        for (int x = 3; x < _grid->getWidth(); x++) {
            std::vector<char> window = {
                state[_grid->getIndex(x, y)],
                state[_grid->getIndex(x-1, y-1)],
                state[_grid->getIndex(x-2, y-2)],
                state[_grid->getIndex(x-3, y-3)]
            };
            score += evaluateAIWindow(window, curr_piece);
        }
    }

    return score;
}

// helper function for evaluateAIBoard
int ConnectFour::evaluateAIWindow(std::vector<char> window, char curr_piece) {
    char opp_piece = curr_piece == '2' ? '1' : '2';
    int curr_count = std::count(window.begin(), window.end(), curr_piece);
    int opp_count = std::count(window.begin(), window.end(), opp_piece);
    int empty_count = std::count(window.begin(), window.end(), '0');

    if (curr_count == 4) return +100000;
    if (curr_count == 3 && empty_count == 1) return +100;
    if (curr_count == 2 && empty_count == 2) return +10;
    if (opp_count == 3 && empty_count == 1) return -120; // penalize enemy advantage
    if (opp_count == 4) return -90000;                   // prioritize winning over block enemy wins though
    return 0;
}


//
// player is the current player's number (AI or human)
//
int ConnectFour::negamax(std::string& state, int depth, int playerColor, int alpha, int beta) 
{   
    int score = evaluateAIBoard(state, playerColor == HUMAN_PLAYER ? '1' : '2');

    // check depth
    if (depth == 0) {
        return score;
    }

    int bestVal = -1000; // Min value
    for (int i = 0; i < 7; i++) {
        int x = board_columns_sequence[i];
        int y = getColumnTopFromX(x);
        if (y == -1) {  // skip this column if it is full
            continue;
        }
        int index = y * 7 + x;
        std::cout << "meep" << std::endl;
        // Check if cell is empty
        if (state[index] == '0') {
            // Make the move
            state[index] = playerColor == HUMAN_PLAYER ? '1' : '2'; // Set the cell to the current player's color
            int nextPlayer = (playerColor == HUMAN_PLAYER) ? AI_PLAYER : HUMAN_PLAYER;
            
            // swap alpha and beta and make them negative to switch perspectives on the board
            // find bestVal (the max)
            if (isAIBoardFull(state)) {
                depth = 0;
            }
            bestVal = std::max(bestVal, -negamax(state, depth - 1, nextPlayer, -beta, -alpha));
            
            // Undo the move for backtracking
            state[index] = '0'; 
            
            // retain the BEST score the of the curr player as alpha
            if (bestVal > alpha) {
                alpha = bestVal;
            }

            // "prune" this recursion, the opp has an advantage here
            if (alpha >= beta) {
                break;
            }

        }
    }

    return bestVal;
}