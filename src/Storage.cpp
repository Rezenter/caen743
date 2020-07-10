//
// Created by ts_group on 09.07.2020.
//

#include "Storage.h"

Storage::Storage(Config &config) : config(config){

}

bool Storage::saveDischarge(Json data) {
    std::string path = config.savePath + "00000/";
    mkdir(path.c_str());
    std::stringstream filename;
    std::ofstream outFile;

    filename.str(std::string());
    filename << "header.json";
    outFile.open(path + filename.str());
    outFile << std::setw(2) << data << std::endl; //bad
    outFile.close();
    return false;
}
