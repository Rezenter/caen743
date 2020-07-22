//
// Created by ts_group on 6/28/2020.
//

#include "Config.h"

#include "iostream"

bool Config::load(std::string path) {
    if(!std::filesystem::exists(path)){
        return false;
    }
    std::ifstream configFile;
    configFile.open(path);
    Json config = Json::parse(configFile);
    configFile.close();

    std::cout << config["crate"] << std::endl;

    return true;
}
