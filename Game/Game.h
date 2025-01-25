#pragma once
#include <chrono>
#include <thread>

#include "../Models/Project_path.h"
#include "Board.h"
#include "Config.h"
#include "Hand.h"
#include "Logic.h"

class Game
{
public:
    Game() : board(config("WindowSize", "Width"), config("WindowSize", "Hight")), hand(&board), logic(&board, &config)
    {
        ofstream fout(project_path + "log.txt", ios_base::trunc);
        fout.close();
    }

    // to start checkers
    int play()
    {
        auto start = chrono::steady_clock::now();
        if (is_replay)
        {
            logic = Logic(&board, &config);
            config.reload();
            board.redraw();
        }
        else
        {
            board.start_draw();
        }
        is_replay = false;

        int turn_num = -1;
        bool is_quit = false;
        const int Max_turns = config("Game", "MaxNumTurns");
        while (++turn_num < Max_turns)
        {
            beat_series = 0;
            logic.find_turns(turn_num % 2);
            if (logic.turns.empty())
                break;
            logic.Max_depth = config("Bot", string((turn_num % 2) ? "Black" : "White") + string("BotLevel"));
            if (!config("Bot", string("Is") + string((turn_num % 2) ? "Black" : "White") + string("Bot")))
            {
                auto resp = player_turn(turn_num % 2);
                if (resp == Response::QUIT)
                {
                    is_quit = true;
                    break;
                }
                else if (resp == Response::REPLAY)
                {
                    is_replay = true;
                    break;
                }
                else if (resp == Response::BACK)
                {
                    if (config("Bot", string("Is") + string((1 - turn_num % 2) ? "Black" : "White") + string("Bot")) &&
                        !beat_series && board.history_mtx.size() > 2)
                    {
                        board.rollback();
                        --turn_num;
                    }
                    if (!beat_series)
                        --turn_num;

                    board.rollback();
                    --turn_num;
                    beat_series = 0;
                }
            }
            else
                bot_turn(turn_num % 2);
        }
        auto end = chrono::steady_clock::now();
        ofstream fout(project_path + "log.txt", ios_base::app);
        fout << "Game time: " << (int)chrono::duration<double, milli>(end - start).count() << " millisec\n";
        fout.close();

        if (is_replay)
            return play();
        if (is_quit)
            return 0;
        int res = 2;
        if (turn_num == Max_turns)
        {
            res = 0;
        }
        else if (turn_num % 2)
        {
            res = 1;
        }
        board.show_final(res);
        auto resp = hand.wait();
        if (resp == Response::REPLAY)
        {
            is_replay = true;
            return play();
        }
        return res;
    }

private:
    // функция хода бота
    void bot_turn(const bool color)
    {
        auto start = chrono::steady_clock::now();

        auto delay_ms = config("Bot", "BotDelayMS");
        // new thread for equal delay for each turn
        thread th(SDL_Delay, delay_ms);
        // находим лучший ход для бота в зависимости от цвета кода
        auto turns = logic.find_best_turns(color);
        // объединение потока
        th.join();
        bool is_first = true;
        // making moves
        // выполняем ход и передвигаем фигуры
        for (auto turn : turns)
        {
            if (!is_first)
            {
                SDL_Delay(delay_ms);
            }
            is_first = false;
            beat_series += (turn.xb != -1);
            board.move_piece(turn, beat_series);
        }

        auto end = chrono::steady_clock::now();
        ofstream fout(project_path + "log.txt", ios_base::app); // логгирование
        // продсчет времени хода бота для отображения в логах
        fout << "Bot turn time: " << (int)chrono::duration<double, milli>(end - start).count() << " millisec\n";
        fout.close();
        fout << "Bot turn time: " << (int)chrono::duration<doub
    }

    // функция хода игрока
    Response player_turn(const bool color)
    {
        // return 1 if quit
        // вектор возможных ходов игрока
        vector<pair<POS_T, POS_T>> cells;
        for (auto turn : logic.turns)
        {
            cells.emplace_back(turn.x, turn.y);
        }
        // отрисовка возможных ходов
        board.highlight_cells(cells);
        move_pos pos = {-1, -1, -1, -1};
        POS_T x = -1, y = -1;
        // trying to make first move
        // цикл отвечающий за обработку первого хода игрока
        while (true)
        {

            auto resp = hand.get_cell(); // ожидание клика пользователя
            // валидация события нажатия пользователя
            if (get<0>(resp) != Response::CELL)
                return get<0>(resp);
            pair<POS_T, POS_T> cell{get<1>(resp), get<2>(resp)};

            bool is_correct = false;
            
            for (auto turn : logic.turns)
            {
                if (turn.x == cell.first && turn.y == cell.second)
                {
                    is_correct = true;
                    break;
                }
                if (turn == move_pos{x, y, cell.first, cell.second})
                {
                    pos = turn;
                    break;
                }
            }
            // если сделан ход и начальная позиция поменялась то прерываем цикла первого хода

            if (pos.x != -1)
                break;
            // при клике на поля где нет подсветки происходит сброс выбранной шашки и итерация цикла начинается сначала
            if (!is_correct)
            {
                // если событи выбора сделано 
                if (x != -1)
                {
                    board.clear_active();
                    board.clear_highlight(); 
                    board.highlight_cells(cells);
                }
                x = -1;
                y = -1;
                continue;
            }
            
            //присваиваем координаты для выбранной пешки для перерасчета возможных ходовч
            x = cell.first;
            y = cell.second;

            // сбрасываем подсветку
            board.clear_highlight(); 

            // прдсвечиваем выбранную пешку
            board.set_active(x, y);
            vector<pair<POS_T, POS_T>> cells2;
            for (auto turn : logic.turns)
            {
                if (turn.x == x && turn.y == y)
                {
                    cells2.emplace_back(turn.x2, turn.y2);
                }
            }
            board.highlight_cells(cells2); // подсветка возможных ходов после выбора клетки
        }
        board.clear_highlight();
        board.clear_active();
        // перемешение шашки
        board.move_piece(pos, pos.xb != -1);

        // проверка если ли побитая шашка и может ли сделать еще ход
        if (pos.xb == -1)
            return Response::OK; // если нет побитых шашек то заканчиваем ход игрока
        // continue beating while can
        beat_series = 1;
        // выбор второго хода если была побита пешка
        while (true)
        {
            logic.find_turns(pos.x2, pos.y2);
            // если нет ходов то прервывается цикл
            if (!logic.have_beats)
                break;

            vector<pair<POS_T, POS_T>> cells;
            for (auto turn : logic.turns)
            {
                cells.emplace_back(turn.x2, turn.y2);
            }
            // подсветка возможных ходов
            board.highlight_cells(cells);
            board.set_active(pos.x2, pos.y2); // подсветка активной шашки
            // trying to make move
            
            while (true)
            {
                // ожидание второго хода пользователя
                auto resp = hand.get_cell();
                // проверка что клик по клетке
                if (get<0>(resp) != Response::CELL)
                    return get<0>(resp);
                pair<POS_T, POS_T> cell{get<1>(resp), get<2>(resp)};

                bool is_correct = false;
                for (auto turn : logic.turns)
                {
                    if (turn.x2 == cell.first && turn.y2 == cell.second)
                    {
                        is_correct = true;
                        pos = turn;
                        break;
                    }
                }
                // теперь если выбор поля некорректен то будем ожидать повтороного ввода события
                if (!is_correct)
                    continue;

                // сброс подсветок и активных полей
                board.clear_highlight();
                board.clear_active();
                beat_series += 1; // добавление к съеденным шашкам
                // перемешение шашки
                board.move_piece(pos, beat_series);
                break;
            }
        }

        return Response::OK;
    }

private:
    Config config;
    Board board;
    Hand hand;
    Logic logic;
    int beat_series;
    bool is_replay = false;
};
