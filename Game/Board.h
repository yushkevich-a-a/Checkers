#pragma once
#include "../Models/Models.h"

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

class Board {
public:
    Board() = default;
    Board(const unsigned int W, const unsigned int H) : W(W), H(H) {}

    int draw() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        if (W == 0 || H == 0) {
            SDL_DisplayMode dm;
            if (SDL_GetDesktopDisplayMode(0, &dm))
            {
                std::cout << "SDL_GetDesktopDisplayMode Error: " << SDL_GetError() << std::endl;
                return 1;
            }
            W = dm.w;
            H = dm.h;
        }
        win = SDL_CreateWindow("Checkers", 0, 0, W, H, SDL_WINDOW_RESIZABLE);
        if (win == nullptr) {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (ren == nullptr) {
            std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        board = IMG_LoadTexture(ren, board_path.c_str());
        if (board == nullptr) {
            std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        w_piece = IMG_LoadTexture(ren, piece_white_path.c_str());
        if (w_piece == nullptr) {
            std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        b_piece = IMG_LoadTexture(ren, piece_black_path.c_str());
        if (b_piece == nullptr) {
            std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        w_queen = IMG_LoadTexture(ren, queen_white_path.c_str());
        if (w_queen == nullptr) {
            std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        b_queen = IMG_LoadTexture(ren, queen_black_path.c_str());
        if (b_queen == nullptr) {
            std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        SDL_GetRendererOutputSize(ren, &W, &H);
        make_start_mtx();
        rerender();
        return 0;
    }
    
    void move_piece(move_pos turn) {
        if (turn.xb != -1) {
            mtx[turn.xb][turn.yb] = 0;
        }
        move_piece(turn.x, turn.y, turn.x2, turn.y2);
    }

    void move_piece(const POS_T i, const POS_T j, const POS_T i2, const POS_T j2) {
        if (mtx[i2][j2]) {
            throw runtime_error("final position is not empty, can't move");
        }
        if (!mtx[i][j]) {
            throw runtime_error("begin position is empty, can't move");
        }
        if ((mtx[i][j] == 1 && i2 == 0) || (mtx[i][j] == 2 && i2 == 7))
            mtx[i][j] += 2;
        mtx[i2][j2] = mtx[i][j];
        drop_piece(i, j);
    }
    
    void drop_piece(const POS_T i, const POS_T j) {
        mtx[i][j] = 0;
        rerender();
    }
    void turn_into_queen(const POS_T i, const POS_T j) {
        if (mtx[i][j] == 0 || mtx[i][j] > 2 ||
            (mtx[i][j] == 1 && i != 0) ||
            (mtx[i][j] == 2 && i != 7)) {
            throw runtime_error("can't turn into queen in this position");
        }
        mtx[i][j] += 2;
        rerender();
    }
    vector<vector<POS_T>> get_board() const {
        return mtx;
    }
    
    void highlight_cells(vector<pair<POS_T, POS_T>> cells) {
        for (auto pos: cells) {
            POS_T x = pos.first, y = pos.second;
            is_highlighted_[x][y] = 1;
        }
        rerender();
    }
    
    void clear_highlight() {
        for (POS_T i = 0; i < 8; ++i) {
            is_highlighted_[i].assign(8, 0);
        }
        rerender();
    }
    
    void set_active(const POS_T x, const POS_T y) {
        active_x = x;
        active_y = y;
        rerender();
    }
    
    void clear_active() {
        active_x = -1;
        active_y = -1;
        rerender();
    }
    
    bool is_highlighted(const POS_T x, const POS_T y) {
        return is_highlighted_[x][y];
    }
    
    void show_final(const int res) {
        game_results = res;
        rerender();
    }
    
    void reset_window_size() {
        SDL_GetRendererOutputSize(ren, &W, &H);
        rerender();
    }

    void quit() {
        SDL_DestroyTexture(board);
        SDL_DestroyTexture(w_piece);
        SDL_DestroyTexture(b_piece);
        SDL_DestroyTexture(w_queen);
        SDL_DestroyTexture(b_queen);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }
    
    ~Board() {
        if (win) quit();
    }

private:
    void make_start_mtx() {
        for (POS_T i = 0; i < 8; ++i) {
            for (POS_T j = 0; j < 8; ++j) {
                if (i < 3 && (i + j) % 2 == 1) mtx[i][j] = 2;
                if (i > 4 && (i + j) % 2 == 1) mtx[i][j] = 1;
            }
        }
    }
    
    void rerender() {
        // draw board
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, board, NULL, NULL);
        
        // draw pieces
        for (POS_T i = 0; i < 8; ++i) {
            for (POS_T j = 0; j < 8; ++j) {
                if (!mtx[i][j]) continue;
                int wpos = W * (j + 1) / 10 + W / 120;
                int hpos = H * (i + 1) / 10 + H / 120;
                SDL_Rect rect{ wpos, hpos, W / 12, H / 12 };

                SDL_Texture* piece_texture;
                if (mtx[i][j] == 1) piece_texture = w_piece;
                else if (mtx[i][j] == 2) piece_texture = b_piece;
                else if (mtx[i][j] == 3) piece_texture = w_queen;
                else piece_texture = b_queen;

                SDL_RenderCopy(ren, piece_texture, NULL, &rect);
            }
        }
        
        // draw hilight
        SDL_SetRenderDrawColor( ren, 0, 255, 0, 0 );
        const double scale = 2.5;
        SDL_RenderSetScale( ren, scale, scale );
        for (POS_T i = 0; i < 8; ++i) {
            for (POS_T j = 0; j < 8; ++j) {
                if (!is_highlighted_[i][j]) continue;
                SDL_Rect cell{int(W * (j + 1) / 10 / scale), int(H * (i + 1) / 10 / scale), int(W / 10 / scale), int(H / 10 / scale)};
                SDL_RenderDrawRect(ren, &cell);
            }
        }
        
        // draw active
        if (active_x != -1) {
            SDL_SetRenderDrawColor( ren, 255, 0, 0, 0 );
            SDL_Rect active_cell{int(W * (active_y + 1) / 10 / scale), int(H * (active_x + 1) / 10 / scale), int(W / 10 / scale), int(H / 10 / scale)};
            SDL_RenderDrawRect(ren, &active_cell);
        }
        SDL_RenderSetScale( ren, 1, 1 );
        
        //draw result
        if (game_results != -1) {
            string result_path = draw_path;
            if (game_results == 1) result_path = white_path;
            else if (game_results == 2) result_path = black_path;
            SDL_Texture* result_texture = IMG_LoadTexture(ren, result_path.c_str());
            if (result_texture == nullptr) {
                std::cout << "IMG_LoadTexture Error: " << SDL_GetError() << std::endl;
                return;
            }
            SDL_Rect res_rect{W / 5, H * 3 / 10, W * 3 / 5, H * 2 / 5};
            SDL_RenderCopy(ren, result_texture, NULL, &res_rect);
            SDL_DestroyTexture(result_texture);
        }
        
        SDL_RenderPresent(ren);
        // next rows for mac os
        SDL_Delay(6);
        SDL_Event windowEvent;
        SDL_PollEvent(&windowEvent);
    }
    
public:
    int W = 0;
    int H = 0;
private:
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    SDL_Texture* board = nullptr;
    SDL_Texture* w_piece = nullptr;
    SDL_Texture* b_piece = nullptr;
    SDL_Texture* w_queen = nullptr;
    SDL_Texture* b_queen = nullptr;
    const string project_path = "/Users/ifrair/Desktop/Проекты С++/xcode_projects/cpp_lesson/cpp_lesson/Textures/";
    const string board_path = project_path + "board.png";
    const string piece_white_path = project_path + "piece_white.png";
    const string piece_black_path = project_path + "piece_black.png";
    const string queen_white_path = project_path + "queen_white.png";
    const string queen_black_path = project_path + "queen_black.png";
    const string white_path = project_path + "white_wins.png";
    const string black_path = project_path + "black_wins.png";
    const string draw_path = project_path + "draw.png";
    int active_x = -1, active_y = -1;
    int game_results = -1;
    vector<vector<bool>> is_highlighted_ = vector<vector<bool>>(8, vector<bool>(8, 0));
    vector<vector<POS_T>> mtx = vector<vector<POS_T>>(8, vector<POS_T>(8, 0));
    // 1 - white, 2 - black, 3 - white queen, 4 - black queen
};

