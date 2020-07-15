//
// Created by ts_group on 6/28/2020.
//

#include "FastSystem.h"

FastSystem::FastSystem(Config& config) : config(config), crate(Crate(config)), storage(config){
    init();

    if(!isAlive()){
        std::cout << "FastSystem init failed" << std::endl;
        return;
    }

    associatedThread = std::thread([&](){
        run();
    });

    /*
    arm();

    std::this_thread::sleep_for(std::chrono::seconds(config.acquisitionTime));

    disarm();
     */
}

bool FastSystem::arm() {
    //set shot number
    crate.arm();
    return false;
}

bool FastSystem::disarm() {
    return storage.saveDischarge(crate.disarm());;
}

bool FastSystem::isAlive() {
    /*
    if(!crate.isAlive()){
        return false;
    }
     */
    if(!storage.isAlive()){
        return false;
    }

    return true;
}

bool FastSystem::init() {
    if(!chatter.init(config)){
        return false;
    }
    if(false && !crate.init()){
        return false;
    }
    return true;
}

bool FastSystem::payload() {
    Message fromChatter = chatter.messages.getMessage();
    if(fromChatter.id != -1){
        messagePayload = {
            {"id", fromChatter.id},
            {"response", true}
        };
        switch (fromChatter.id) {
            case 0:
                //isAlive
                messagePayload["status"] = isAlive();
                if(!messagePayload["status"]){
                    messagePayload["error"] = "Fast system is dead.";
                }
                break;
            case 1:
                //arm
                if(isAlive()){
                    //get shot number
                    messagePayload["status"] = arm();
                    if(!messagePayload["status"]){
                        messagePayload["error"] = "Failed to arm.";
                    }
                }else{
                    messagePayload["error"] = "Fast system is dead.";
                }
                break;
            case 2:
                //disarm
                if(isAlive()){
                    messagePayload["status"] = disarm();
                    if(!messagePayload["status"]){
                        messagePayload["error"] = "Failed to disarm.";
                    }
                }else{
                    messagePayload["error"] = "Fast system is dead.";
                }
                break;
            default:
                std::cout << "Unknown message from chatter: " << fromChatter.id << std::endl;
                break;
        }
        chatter.sendPacket(messagePayload);
    }
    return false;
}

FastSystem::~FastSystem() {
    std::cout << "fastSystem destructor... ";
    requestStop();
    if(associatedThread.joinable()){
        associatedThread.join();
    }
    std::cout << "OK" << std::endl;
}
