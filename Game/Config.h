#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef __APPLE__
    #define  settings_path string("../../../cpp_lesson/settings.json")
#else
    #define  settings_path string("settings.json")
#endif

class Config
{
  public:
    Config()
    {
        reload();
    }

    void reload()
    {
        std::ifstream fin(settings_path);
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
