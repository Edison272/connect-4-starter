#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "Classes/ConnectFour.h"

namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                if (gameOver) {
                    ImGui::SeparatorText("End of Game");
                    ImGui::Text("Game Over!");
                    if (gameWinner == -1) {
                        ImGui::Text("===== Draw =====");
                    } else {
                        ImGui::Text("=== Winner: %d ===", gameWinner);
                    }
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                        game->_gameOptions.AIPlaying = false;
                    }
                }
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Connect Four")) {
                        game = new ConnectFour();
                        game->setUpBoard();
                    }
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());

                    // Toggle AI
                    ImGui::SeparatorText("AI Settings");
                    std::string toggle_ai_label = "Toggle AI | Currently " + std::string(game->_gameOptions.AIPlaying ? "On" : "Off");
                    if (ImGui::Button(toggle_ai_label.c_str())) {
                        game->_gameOptions.AIPlaying = !game->_gameOptions.AIPlaying;
                    }

                    if (game->_gameOptions.AIPlaying) {
                        std::string toggle_player1_ai_label = "Player 1 AI | Currently " + std::string(game->getPlayerAt(0)->isAIPlayer() ? "On" : "Off");
                        if (ImGui::Button(toggle_player1_ai_label.c_str())) {
                            game->getPlayerAt(0)->setAIPlayer(!game->getPlayerAt(0)->isAIPlayer());
                        }

                        std::string toggle_player2_ai_label = "Player 2 AI | Currently " + std::string(game->getPlayerAt(1)->isAIPlayer() ? "On" : "Off");
                        if (ImGui::Button(toggle_player2_ai_label.c_str())) {
                            game->getPlayerAt(1)->setAIPlayer(!game->getPlayerAt(1)->isAIPlayer());
                        }
                    }
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    // updateAI if true
                    if (!gameOver && game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();

                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
