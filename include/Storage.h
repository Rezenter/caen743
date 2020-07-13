//
// Created by ts_group on 09.07.2020.
//

#ifndef CAEN743_STORAGE_H
#define CAEN743_STORAGE_H

#include "common.h"
#include "json.hpp"
#include <io.h>
#include <iomanip>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

//debug
#include <iostream>
//debug

using Json = nlohmann::json;

class Storage {
private:
    Config& config;

public:
    explicit Storage(Config& config);

    bool saveDischarge(const Json& data) const;
    [[nodiscard]] bool isAlive() const;
    static bool isDir(const std::string&);
};


#endif //CAEN743_STORAGE_H
