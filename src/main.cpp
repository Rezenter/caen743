//
// Created by ts_group on 6/28/2020.
//


#include "FastSystem.h" //should be before "root"
#include "root.h"

void delay(){
    for(int i  = 0; i < 100000000; i++){
        double fuck = 1.0/i;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "TS fast acquisition, revision:" << REVISION << "\n\n\n" << std::endl << std::flush;
    delay();

    std::cout << std::endl;

    Config config;

    //load config

    FastSystem fs(config);

    while (!fs.exitRequested()){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\nNormal exit." << std::endl << std::flush;
    delay();
}