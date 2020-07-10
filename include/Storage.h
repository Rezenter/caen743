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

using Json = nlohmann::json;

class Storage {
private:
    Config& config;

public:
    explicit Storage(Config& config);

    bool saveDischarge(Json data);
};


#endif //CAEN743_STORAGE_H
