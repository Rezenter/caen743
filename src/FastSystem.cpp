//
// Created by ts_group on 6/28/2020.
//

#include "FastSystem.h"

FastSystem::FastSystem(Config& config) : config(config), crate(Crate(config)), storage(config){
    if(init()){
        exit = false;
        if(!isAlive()){
            std::cout << "FastSystem init failed" << std::endl;
            return;
        }

        associatedThread = std::thread([&](){
            run();
        });

        std::cout << "System initialised" <<std::endl;
    }else{
        std::cout << "Fast system init failed!" << std::endl;
    }
}

bool FastSystem::arm() {
    if(!crate.arm()){
        return false;
    }
    return true;
}

bool FastSystem::disarm() {
    return storage.saveDischarge(crate.disarm());;
}

bool FastSystem::isAlive() {
    if(!crate.isAlive()){
        return false;
    }
    if(!storage.isAlive()){
        return false;
    }

    return true;
}

bool FastSystem::init() {
    if(!chatter.init(config)){
        return false;
    }
    if(!crate.init()){
        return false;
    }
    return true;
}

bool FastSystem::payload() {
    Message fromChatter = chatter.messages.getMessage();
    if(fromChatter.id != -1){
        messagePayload = {
            {"id", fromChatter.id}
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
                    config.isPlasma = fromChatter.payload["isPlasma"];
                    if(config.isPlasma){
                        config.plasmaShot = fromChatter.payload["shotn"];
                    }else{
                        config.debugShot = fromChatter.payload["shotn"];
                    }
                    if(fromChatter.payload.contains("aux")){
                        config.aux_args = fromChatter.payload["aux"];
                    }else{
                        config.aux_args = {};
                    }
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
            case 3:
                //exit
                if(isAlive()){
                    disarm();
                }
                exit = true;
                requestStop();
                break;
            default:
                std::cout << "Unknown message from chatter: " << fromChatter.id << std::endl;
                break;
        }
        chatter.sendPacket(messagePayload);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(config.messagePoolingInterval));
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
