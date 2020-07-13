//
// Created by ts_group on 09.07.2020.
//

#include "Storage.h"

Storage::Storage(Config &config) : config(config){

}

bool Storage::saveDischarge(const Json& data) const {
    std::stringstream path;
    if(config.isPlasma){
        path << config.plasmaPath << std::setw(5) << std::setfill('0') << config.plasmaShot << '/';
    }else{
        path << config.debugPath << std::setw(5) << std::setfill('0') << config.debugShot << '/';
    }
    std::string pathStr = path.str();
    mkdir(pathStr.c_str());
    std::ofstream outFile;

    outFile.open(pathStr + "header.json");
    outFile << std::setw(2) << data["header"] << std::endl;
    outFile.close();

    int count = 0;
    std::stringstream filename;
    for(auto& board : data["boards"]){
        filename.str(std::string());
        filename << count++ << ".json";
        outFile.open(pathStr + filename.str());
        outFile << std::setw(2) << board << std::endl;
        outFile.close();
    }
    return true;
}

bool Storage::isAlive() const {
    if(!isDir(config.plasmaPath)){
        std::cout << "Directory" << config.plasmaPath << "for plasma shots not found." << std::endl;
        return false;
    }
    if(!isDir(config.debugPath)){
        std::cout << "Directory" << config.debugPath << "for debug shots not found." << std::endl;
        return false;
    }
    return true;
}

bool Storage::isDir(const std::string& path) {
    struct stat info;
    if(stat(path.c_str(), &info ) != 0) {
        return false;
    }
    else return (info.st_mode & S_IFDIR) != 0;
}
