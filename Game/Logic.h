#pragma once
#include "../Models/Models.h"
#include "Board.h"

#include <vector>

const int INF = 1e9;

class Logic {
public:
    Logic(Board* board): board(board) {}
    
    void find_turns(const bool color) {
        find_turns(color, board->get_board());
    }
    
    void find_turns(const int x, const int y) {
        find_turns(x, y, board->get_board());
    }
    
    vector<move_pos> find_best_turns(const bool color) {
        next_best_state.clear();
        next_move.clear();
        
        find_first_best_turn(board->get_board(), color, -1, -1, 0);
        
        int cur_state = 0;
        vector<move_pos> res;
        do {
            res.push_back(next_move[cur_state]);
            cur_state = next_best_state[cur_state];
        } while(cur_state != -1 && next_move[cur_state].x != -1);
        return res;
    }
    
private:
    vector<vector<int>> make_turn(vector<vector<int>> mtx, move_pos turn) {
        if (turn.xb != -1) mtx[turn.xb][turn.yb] = 0;
        if ((mtx[turn.x][turn.y] == 1 && turn.x2 == 0) || (mtx[turn.x][turn.y] == 2 && turn.x2 == 7)) mtx[turn.x][turn.y] += 2;
        mtx[turn.x2][turn.y2] = mtx[turn.x][turn.y];
        mtx[turn.x][turn.y] = 0;
        return mtx;
    }
    
    double calc_score(const vector<vector<int>>& mtx, const bool color) {
        if (color) return calc_score_black(mtx, color);
        else return calc_score_white(mtx, color);
    }
    
    double calc_score_white(const vector<vector<int>>& mtx, const bool color) {
        // color - who is max player
        double w = 0, wq = 0, b = 0, bq = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                w += (mtx[i][j] == 1);
                wq += (mtx[i][j] == 3);
                b += (mtx[i][j] == 2);
                bq += (mtx[i][j] == 4);
            }
        }
        if (!color) {
            swap(b, w);
            swap(bq, wq);
        }
        if (w + wq == 0) return INF;
        if (b + bq == 0) return 0;
        return (b + bq * 4) / (w + wq * 4);
    }
    
    double calc_score_black(const vector<vector<int>>& mtx, const bool color) {
        // color - who is max player
        double w = 0, wq = 0, b = 0, bq = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                w += (mtx[i][j] == 1);
                // w += 0.1 * (mtx[i][j] == 1) * (7 - i);
                wq += (mtx[i][j] == 3);
                b += (mtx[i][j] == 2);
                // b += 0.1 * (mtx[i][j] == 2) * (i);
                bq += (mtx[i][j] == 4);
            }
        }
        if (!color) {
            swap(b, w);
            swap(bq, wq);
        }
        if (w + wq == 0) return INF;
        if (b + bq == 0) return 0;
        // return (b + bq * 7) / (w + wq * 7);
        return (b + bq * 4) / (w + wq * 4);
    }

    double find_first_best_turn(vector<vector<int>> mtx, const bool color, const int x, const int y, size_t state, double alpha = -1) {
        next_best_state.push_back(-1);
        next_move.emplace_back(-1, -1, -1, -1);
        double best_score = -1;
        if (state != 0) find_turns(x, y, mtx);
        auto turns_now = turns;
        bool have_beats_now = have_beats;
        
        if (!have_beats_now && state != 0) {
            return find_best_turns_rec(mtx , 1 - color, 0, alpha);
        }
        
        vector<move_pos> best_moves;
        vector<int> best_states;
        
        for (auto turn: turns_now) {
            size_t next_state = next_move.size();
            double score;
            if (have_beats_now) {
                score = find_first_best_turn(make_turn(mtx, turn), color, turn.x2, turn.y2, next_state, best_score);
            } else {
                score = find_best_turns_rec(make_turn(mtx, turn), 1 - color, 0, best_score);
            }
            if (score > best_score) {
                best_score = score;
                next_best_state[state] = (have_beats_now ? next_state : -1);
                next_move[state] = turn;
            }
            if (score == best_score) {
                score = best_score;
            }
        }
        return best_score;
    }
    
    double find_best_turns_rec(vector<vector<int>> mtx,
                           const bool color,
                           const size_t depth,
                           double alpha = -1,
                           double beta = INF + 1,
                           const int x = -1,
                           const int y = -1) {
        if (depth == Max_depth) {
            return calc_score(mtx, (depth % 2 == color));
        }
        if (x != -1)
            find_turns(x, y, mtx);
        else find_turns(color, mtx);
        auto turns_now = turns;
        bool have_beats_now = have_beats;
        
        if (!have_beats_now && x != -1) {
            return find_best_turns_rec(mtx, 1 - color, depth + 1, alpha, beta);
        }
        
        if (turns.empty()) return (depth % 2 ? 0 : INF);
        
        double min_score = INF + 1;
        double max_score = -1;
        for (auto turn: turns_now) {
            double score = 0.0;
            if (!have_beats_now && x == -1) {
                score = find_best_turns_rec(make_turn(mtx, turn), 1 - color, depth + 1, alpha, beta);
            } else {
                score = find_best_turns_rec(make_turn(mtx, turn), color, depth, alpha, beta, turn.x2, turn.y2);
            }
            min_score = min(min_score, score);
            max_score = max(max_score, score);
            // alpha-beta pruning
            if (depth % 2) alpha = max(alpha, max_score);
            else beta = min(beta, min_score);
            if (alpha >= beta) break;
        }
        return (depth % 2 ? max_score : min_score);
    }
    
    void find_turns(const bool color, const vector<vector<int>>& mtx) {
        vector<move_pos> res_turns;
        bool have_beats_before = false;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (mtx[i][j] && mtx[i][j] % 2 != color) {
                    find_turns(i, j, mtx);
                    if (have_beats && !have_beats_before) {
                        have_beats_before = true;
                        res_turns.clear();
                    }
                    if ((have_beats_before && have_beats) || !have_beats_before) {
                        res_turns.insert( res_turns.end(), turns.begin(), turns.end() );
                    }
                }
            }
        }
        turns = res_turns;
        have_beats = have_beats_before;
    }
    
    void find_turns(const int x, const int y, const vector<vector<int>>& mtx) {
        turns.clear();
        have_beats = false;
        int type = mtx[x][y];
        // check beats
        switch (type) {
            case 1:
            case 2:
                // check pieces
                for (int i = x - 2; i <= x + 2; i += 4) {
                    for (int j = y - 2; j <= y + 2; j += 4) {
                        if ( i < 0 || i > 7 || j < 0 || j > 7)
                            continue;
                        int xb = (x + i) / 2, yb = (y + j) / 2;
                        if ( mtx[i][j] ||
                            !mtx[xb][yb] ||
                            mtx[xb][yb] % 2 == type % 2)
                            continue;
                        turns.emplace_back(x, y, i, j, xb, yb);
                    }
                }
                break;
            default:
                // check queens
                for (int i = -1; i <= 1; i += 2) {
                    for (int j = -1; j <= 1; j += 2) {
                        int xb = -1, yb = -1;
                        for (int i2 = x + i, j2 = y + j;
                             i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1;
                             i2 += i, j2 += j) {
                            if (mtx[i2][j2]) {
                                if (mtx[i2][j2] % 2 == type % 2 ||
                                    (mtx[i2][j2] % 2 != type % 2 && xb != -1)) {
                                    break;
                                }
                                xb = i2;
                                yb = j2;
                            }
                            if (xb != -1 && xb != i2) {
                                turns.emplace_back(x, y, i2, j2, xb, yb);
                            }
                        }
                    }
                }
                break;
        }
        // check other turns
        if (!turns.empty()) {
            have_beats = true;
            return;
        }
        switch (type) {
            case 1:
            case 2:
                // check pieces
                {
                    int i = ((type % 2) ? x - 1 : x + 1);
                    for (int j = y - 1; j <= y + 1; j += 2) {
                        if ( i < 0 || i > 7 || j < 0 || j > 7 || mtx[i][j])
                            continue;
                        turns.emplace_back(x, y, i, j);
                    }
                    break;
                }
            default:
                // check queens
                for (int i = -1; i <= 1; i += 2) {
                    for (int j = -1; j <= 1; j += 2) {
                        for (int i2 = x + i, j2 = y + j;
                             i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1;
                             i2 += i, j2 += j) {
                            if (mtx[i2][j2]) break;
                            turns.emplace_back(x, y, i2, j2);
                        }
                    }
                }
                break;
        }
    }
    
public:
    vector<move_pos> turns;
    bool have_beats;
    int Max_depth;
private:
    vector<move_pos> next_move;
    vector<int> next_best_state;
    Board* board;
};
