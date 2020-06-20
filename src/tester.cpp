//
// Created by user on 6/18/2020.
//

#include <iostream>

#include <windows.h>
#include <thread>

typedef UINT (CALLBACK* ARM_FNC)(unsigned char);
typedef UINT (CALLBACK* DISARM_FNC)(unsigned char);

//#include "CAEN743.h"

typedef int (__cdecl *MYPROC)(LPWSTR);

int main(int argc, char* argv[]){
    std::cout << "let the test begin" << std::endl;

    HINSTANCE hinstLib;

    ARM_FNC arm;    // Function pointer
    DISARM_FNC disarm;    // Function pointer
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    std::cout << "loading dll" << std::endl;
    hinstLib = LoadLibrary(TEXT("cygcaen743.dll"));
    if (hinstLib != NULL)
    {
        std::cout << "found dll" << std::endl;
        arm = (ARM_FNC)GetProcAddress(hinstLib,"arm");
        disarm = (DISARM_FNC)GetProcAddress(hinstLib,"disarm");

        if (arm && disarm){
            std::cout << "found process" << std::endl;
            fRunTimeLinkSuccess = TRUE;
            int res = arm(0);
            std::cout << "result = " << res << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << disarm(0) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }else{
            std::cout << "Handle error " << GetLastError() << std::endl;
        }
        // Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
    }

    // If unable to call the DLL function, use an alternative.
    if (! fRunTimeLinkSuccess)
        printf("Message printed from executable\n");

    std::cout << "totally clean" << std::endl;
    return 0;
}
