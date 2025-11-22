#pragma once
#include "Game.h"

//
// the classic game of tic tac toe
//

//
// the main game class
//
class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }
    int         turns_elapsed = 0;
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    int         getColumnTopFromX(int x);
    int         getAIColumnTopFromX(std::string& state, int x);
    int         getUnscopedIndex(int x, int y, int index, int scope);                // helper function to check for winner
    int         negamax(std::string& state, int depth, int playerColor, int alpha, int beta);
    bool        isAIBoardFull(const std::string& state);
    int         evaluateAIBoard(const std::string& state, char curr_piece);                // returns a score based on the most favorable position
    int         evaluateAIWindow(std::vector<char> window, char curr_piece);                     // helper function used by evaluateAIBoard to score sections of the board
    
    // const info
    const int board_columns_sequence[7] = {3, 2, 4, 1, 5, 0, 6};

    const int WinQuad[10][4] = { {0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15},  // rows
                                        {0,4,8,12}, {1,5,9,13}, {2,6,10,14}, {3,7,11,15},  // cols
                                        {0,5,10,15}, {3,6,9,12} };                         // diagonals
    const int TripletInQuad[24][3] = { {0,1,2}, {1,2,3}, {4,5,6}, {5,6,7}, {8,9,10}, {9,10,11}, {12,13,14}, {13,14,15}, // rows
                                       {0,4,8}, {4,8,12}, {1,5,9}, {5,9,13}, {2,6,10}, {6,10,14}, {3,7,11}, {7,11,15},  // cols
                                       {0,5,10}, {2,5,8}, {1,6,11}, {3,6,9}, {4,9,14}, {6,9,12}, {5,10,15}, {7,10,13}}; // diagonals
    Grid*       _grid;
};

