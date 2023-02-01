#pragma once
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Config {
public:
    auto operator()(const string& setting_dir, const string& setting_name) const {
        std::ifstream fin("../../../cpp_lesson/settings.json");
        json conf;
        fin >> conf;
        return conf[setting_dir][setting_name];
    }
};
