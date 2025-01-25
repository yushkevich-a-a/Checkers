#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../Models/Project_path.h"

class Config
{
  public:
    // круглые скобки для вызова функции конструктора и инициализации начальной конфигурации игры
    Config() 
    {
        reload();
    }

    // функция чтения конфигурации игры из файла settings.json и 
    // сохранение в переменной config
    void reload()     {
        std::ifstream fin(project_path + "settings.json"); // открытие файла settings.json 
        fin >> config; // чтение файла setting.json и сохранение конфига в переменную config
        fin.close(); // закрытие файла settings.json
    } 

    auto operator()(const string &setting_dir, const string &setting_name) const
    {
        return config[setting_dir][setting_name];
    }

  private:
    json config;
};
