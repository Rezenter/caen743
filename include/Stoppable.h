//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_STOPPABLE_H
#define CAEN743_STOPPABLE_H

#include <thread>
#include <chrono>
#include <future>

class Stoppable{
private:
    std::promise<void> exitSignal;
    std::future<void> futureObj;
    virtual void run() = 0;

protected:
    void stop(){
        exitSignal.set_value();
    }

public:

    std::thread associatedThread;

    Stoppable() : futureObj(exitSignal.get_future()) {}
    Stoppable(Stoppable && obj)  noexcept : exitSignal(std::move(obj.exitSignal)), futureObj(std::move(obj.futureObj)){}
    Stoppable & operator = (Stoppable && obj) noexcept {
        exitSignal = std::move(obj.exitSignal);
        futureObj = std::move(obj.futureObj);
        return *this;
    }

    bool stopRequested(){
        return !(futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout);
    }
};


#endif //CAEN743_STOPPABLE_H
