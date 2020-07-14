//
// Created by ts_group on 13.07.2020.
//

#ifndef CAEN743_MESSAGEQUEUE_H
#define CAEN743_MESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include <shared_mutex>
#include "json.hpp"

using Json = nlohmann::json;

struct Message{
    const int id;
    const Json payload;
};

class MessageQueue{
private:
    mutable std::shared_mutex mutex;
    std::queue<Message> messages;
public:
    [[nodiscard]] Json getMessage();
    void putMessage(int id, Json& payload);
    void clear();
};

#endif //CAEN743_MESSAGEQUEUE_H
