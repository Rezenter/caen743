//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_STOPPABLE_H
#define CAEN743_STOPPABLE_H

#include <thread>
#include <chrono>
#include <future>
#include <mutex>
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
        return !(futureObj.wait_for(std::chrono::nanoseconds (0)) == std::future_status::timeout);
    }
};

class StoppableMutex{
private:
    std::mutex mutex;
    bool stop;

protected:
    virtual bool payload() = 0;
    virtual void beforePayload() = 0;
    virtual void afterPayload() = 0;

public:
    std::thread associatedThread;

    void requestStop(){
        mutex.lock();
        stop = true;
        mutex.unlock();
    };

    void run(){
        stop = false;
        beforePayload();
        while(true){
            mutex.lock();
            if(stop){
                mutex.unlock();
                break;
            }
            mutex.unlock();
            if(payload()){
                break;
            };
        }
        afterPayload();
    };
};

#endif //CAEN743_STOPPABLE_H
