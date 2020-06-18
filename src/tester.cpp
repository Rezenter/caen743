//
// Created by user on 6/18/2020.
//

#include <iostream>
#include "CAEN743.h"

int main(int argc, char* argv[]){
    std::cout << "let the test begin" << std::endl;
    for(int i = 0; i < 6; i++){
        std::cout << int(arm(i)) << std::endl;
    }
    std::cout << "totally clean" << std::endl;
    return 0;
}
