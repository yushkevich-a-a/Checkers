#pragma once
#include <SDL2/SDL.h>

#include <tuple>

#include "../Models/Move.h"
#include "../Models/Response.h"
#include "Board.h"

// methods for hands
class Hand
{
  public:
    Hand(Board *board) : board(board)
    {
    }
    tuple<Response, POS_T, POS_T> get_cell() const
    {
        SDL_Event windowEvent;
        Response resp = Response::CELL;
        int x = -1, y = -1;
        int xc = -1, yc = -1;
        while (true)
        {
            if (SDL_PollEvent(&windowEvent))
            {
                switch (windowEvent.type)
                {
                case SDL_QUIT:
                    resp = Response::QUIT;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = windowEvent.motion.x;
                    y = windowEvent.motion.y;
                    xc = int(y / (board->H / 10) - 1);
                    yc = int(x / (board->W / 10) - 1);
                    if (xc == -1 && yc == -1 && board->history_mtx.size() > 1)
                        resp = Response::BACK;
                    if (xc == -1 && yc == 8)
                        resp = Response::REPLAY;
                    if (xc < 0 || xc >= 8 || yc < 0 || yc >= 8)
                    {
                        xc = -1;
                        yc = -1;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (windowEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        board->reset_window_size();
                        break;
                    }
                }
                if (xc != -1 || resp != Response::CELL)
                    break;
            }
        }
        return {resp, xc, yc};
    }

    void wait() const
    {
        SDL_Event windowEvent;
        bool quit = false;
        while (true)
        {
            if (SDL_PollEvent(&windowEvent))
            {
                switch (windowEvent.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    board->reset_window_size();
                    break;
                }
                if (quit)
                    break;
            }
        }
    }

  private:
    Board *board;
};
