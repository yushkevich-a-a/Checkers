#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Config
{
  public:
    Config()
    {
        std::ifstream fin("../../../cpp_lesson/settings.json");
        fin >> config;
        fin.close();
    }
    auto operator()(const string &setting_dir, const string &setting_name) const
    {
        return config[setting_dir][setting_name];
    }

  private:
    json config;
};
