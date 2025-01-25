#pragma once
#include <tuple>

#include "../Models/Move.h"
#include "../Models/Response.h"
#include "Board.h"

// methods for hands класс навашивающий на поределенной фрагменты программы обработчики событий от пользователя
class Hand
{
  public:
    // инициализация игрового поля
    Hand(Board *board) : board(board)
    {
    }

    // функция обработчиков события пользователя при взаимодействии с игровым полем и возвращает соответствующий респонс по нажатию
    tuple<Response, POS_T, POS_T> get_cell() const
    {
        SDL_Event windowEvent;
        Response resp = Response::OK;
        int x = -1, y = -1;
        int xc = -1, yc = -1;
        while (true)
        {
            if (SDL_PollEvent(&windowEvent))
            {
                switch (windowEvent.type)
                {
                // нажатие на кнопку окончания программы
                case SDL_QUIT:
                    resp = Response::QUIT;
                    break;
                // нажатия на кнопки игрового поля
                case SDL_MOUSEBUTTONDOWN:
                    x = windowEvent.motion.x;
                    y = windowEvent.motion.y;
                    xc = int(y / (board->H / 10) - 1);
                    yc = int(x / (board->W / 10) - 1);
                    if (xc == -1 && yc == -1 && board->history_mtx.size() > 1)
                    {
                        resp = Response::BACK; // события шаг назад
                    }
                    else if (xc == -1 && yc == 8)
                    {
                        resp = Response::REPLAY; // событие начать игру заново
                    }
                    else if (xc >= 0 && xc < 8 && yc >= 0 && yc < 8)
                    {
                        resp = Response::CELL; // событи выбора клетки
                    }
                    else
                    {
                        xc = -1;
                        yc = -1;
                    }
                    break;

                    // событие изменение шириниы экрана приложения
                case SDL_WINDOWEVENT:
                    if (windowEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        board->reset_window_size();
                        break;
                    }
                }
                if (resp != Response::OK)
                    break;
            }
        }
        // если произошло нажатие то возврашается ответ. занчение события и координаты
        return {resp, xc, yc};
    }

    // метод ожидания события при окончании игры
    Response wait() const
    {
        SDL_Event windowEvent;
        Response resp = Response::OK;
        while (true)
        {
            if (SDL_PollEvent(&windowEvent))
            {
                switch (windowEvent.type)
                {
                case SDL_QUIT:
                    resp = Response::QUIT;
                    break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    board->reset_window_size();
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    int x = windowEvent.motion.x;
                    int y = windowEvent.motion.y;
                    int xc = int(y / (board->H / 10) - 1);
                    int yc = int(x / (board->W / 10) - 1);
                    if (xc == -1 && yc == 8)
                        resp = Response::REPLAY;
                }
                break;
                }
                if (resp != Response::OK)
                    break;
            }
        }
        return resp;
    }

  private:
    Board *board;
};
