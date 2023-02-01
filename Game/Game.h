#pragma once
#include "Board.h"
#include "Config.h"
#include "Hand.h"
#include "Logic.h"

#include <chrono>
#include <thread>

class Game {
public:
    Game() : board(config("WindowSize", "Width"), config("WindowSize", "Hight")), hand(&board), logic(&board, &config) {}
    
    int play() {
        auto start = chrono::steady_clock::now();
        board.draw();
        
        int turn_num = -1;
        bool is_quit = false;
        while ( ++turn_num < Max_turns) {
            logic.find_turns(turn_num % 2);
            if (logic.turns.empty()) break;
            if (turn_num % 2) {
                logic.Max_depth = config("Bot", "BlackBotLevel");
                if (!config("Bot", "IsBlackBot")) {
                    if (player_turn(1)) {
                        is_quit = true;
                        return 0;
                    }
                }
                else bot_turn(1);
            }
            else {
                logic.Max_depth = config("Bot", "WhiteBotLevel");
                if (!config("Bot", "IsWhiteBot")) {
                    if (player_turn(0)) {
                        is_quit = true;
                        return 0;
                    }
                }
                else bot_turn(0);
            }
        }
        auto end = chrono::steady_clock::now();
        cout << "Game time: " << (int)chrono::duration <double, milli> (end - start).count() << " millisec\n";
        if (is_quit) return 0;
        int res = 2;
        if (turn_num == Max_turns) {
            res = 0;
        }
        else if (turn_num % 2) {
            res = 1;
        }
        board.show_final(res);
        hand.wait();
        return res;
    }

private:
    void bot_turn(const bool color) {
        auto start = chrono::steady_clock::now();
        auto delay_ms = config("Bot", "BotDelayMS");
        thread th(SDL_Delay, delay_ms);
        auto turns = logic.find_best_turns(color);
        th.join();
        bool is_first = true;
        for (auto turn: turns) {
            if (!is_first) {
                SDL_Delay(delay_ms);
            }
            is_first = false;
            board.move_piece(turn);
        }
        auto end = chrono::steady_clock::now();
        cout << "Bot turn time: " << (int)chrono::duration <double, milli> (end - start).count() << " millisec\n";    }
    
    bool player_turn(const bool color) {
        // return 1 if quit
        vector<pair<POS_T, POS_T>> cells;
        for (auto turn: logic.turns) {
            cells.emplace_back(turn.x, turn.y);
        }
        board.highlight_cells(cells);
        move_pos pos = {-1, -1, -1, -1};
        POS_T x = -1, y = -1;
        while (true) {
            auto cell = hand.get_cell();
            if (cell.first == -1) return 1;
            
            bool is_correct = false;
            for (auto turn: logic.turns) {
                if (turn.x == cell.first && turn.y == cell.second) {
                    is_correct = true;
                    break;
                }
                if (turn == move_pos{x, y, cell.first, cell.second}) {
                    pos = turn;
                    break;
                }
            }
            if (pos.x != -1) break;
            if (!is_correct) {
                if (x != -1) {
                    board.clear_active();
                    board.clear_highlight();
                    board.highlight_cells(cells);
                }
                x = -1;
                y = -1;
                continue;
            }
            x = cell.first;
            y = cell.second;
            board.clear_highlight();
            board.set_active(x, y);
            vector<pair<POS_T, POS_T>> cells2;
            for (auto turn: logic.turns) {
                if (turn.x == x && turn.y == y) {
                    cells2.emplace_back(turn.x2, turn.y2);
                }
            }
            board.highlight_cells(cells2);
        }
        board.clear_highlight();
        board.clear_active();
        board.move_piece(pos);
        if (pos.xb == -1) return 0;
        // continue beating
        while (true) {
            logic.find_turns(pos.x2, pos.y2);
            if (!logic.have_beats) break;
            
            vector<pair<POS_T, POS_T>> cells;
            for (auto turn: logic.turns) {
                cells.emplace_back(turn.x2, turn.y2);
            }
            board.highlight_cells(cells);
            board.set_active(pos.x2, pos.y2);
            
            while (true) {
                auto cell = hand.get_cell();
                if (cell.first == -1) return 1;
                
                bool is_correct = false;
                for (auto turn: logic.turns) {
                    if (turn.x2 == cell.first && turn.y2 == cell.second) {
                        is_correct = true;
                        pos = turn;
                        break;
                    }
                }
                if (!is_correct)
                   continue;
                
                board.clear_highlight();
                board.clear_active();
                board.move_piece(pos);
                break;
            }
                
        }
        
        return 0;
    }
    
private:
    Config config;
    Board board;
    Hand hand;
    Logic logic;
    const int Max_turns = 130;
};
