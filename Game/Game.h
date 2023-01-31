#pragma once
#include "Board.h"
#include "Hand.h"
#include "Logic.h"

#include <chrono>
#include <thread>

class Game {
public:
    Game():
        board(),
        hand(&board),
        logic(&board) {}
    Game(const unsigned int W, const unsigned int H) :
        board(W, H),
        hand(&board),
        logic(&board) {}
    
    int play(const int delay_ms = 1000, const bool is_bot_first = false,
             const bool is_bot_second = true,
             const int first_bot_level = 5,
             const int second_bot_level = 5) {
        auto start = chrono::steady_clock::now();
        delay = delay_ms;
        board.draw();
        
        int turn_num = -1;
        bool is_quit = false;
        while ( ++turn_num < Max_turns) {
            logic.find_turns(turn_num % 2);
            if (logic.turns.empty()) break;
            if (turn_num % 2) {
                logic.Max_depth = second_bot_level;
                if (!is_bot_second) {
                    if (player_turn(1)) {
                        is_quit = true;
                        return 0;
                    }
                }
                else bot_turn(1);
            }
            else {
                logic.Max_depth = first_bot_level;
                if (!is_bot_first) {
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
        // 6 6 23000->11300->3076   7 6 119479->56991->3838   10 9 29776
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
        thread th(SDL_Delay, delay);
        auto turns = logic.find_best_turns(color);
        th.join();
        bool is_first = true;
        for (auto turn: turns) {
            if (!is_first) {
                SDL_Delay(delay);
            }
            is_first = false;
            board.move_piece(turn);
        }
        auto end = chrono::steady_clock::now();
        cout << "Bot turn time: " << (int)chrono::duration <double, milli> (end - start).count() << " millisec\n";
        // 2348 2287 5 > 1000
    }
    
    bool player_turn(const bool color) {
        // return 1 if quit
        vector<pair<int, int>> cells;
        for (auto turn: logic.turns) {
            cells.emplace_back(turn.x, turn.y);
        }
        board.highlight_cells(cells);
        move_pos pos = {-1, -1, -1, -1};
        int x = -1, y = -1;
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
            vector<pair<int, int>> cells2;
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
            
            vector<pair<int, int>> cells;
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
    Board board;
    Hand hand;
    Logic logic;
    int delay = 1000;
    const int Max_turns = 130;
};
