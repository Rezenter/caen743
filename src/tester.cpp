//
// Created by user on 6/18/2020.
//

#include <iostream>

#include <windows.h>
#include <thread>


typedef UINT (CALLBACK* ARM_FNC)(unsigned char);
typedef UINT (CALLBACK* DISARM_FNC)(unsigned char);

typedef int (__cdecl *MYPROC)(LPWSTR);

int main(int argc, char* argv[]){
    std::cout << "let the test begin\n" << std::endl;

    HINSTANCE hinstLib;

    ARM_FNC arm;    // Function pointer
    DISARM_FNC disarm;    // Function pointer
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    std::cout << "loading dll" << std::endl;

#if defined(__CYGWIN__)
#define LIB "cygcaen743.dll"
#else
#define LIB "libcaen743.dll"
#endif

    hinstLib = LoadLibrary(TEXT(LIB));
    if (hinstLib != nullptr)
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
            std::cout << "alive" <<std::endl;
            std::cout << disarm(0) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }else{
            std::cout << "Handle GetProcAddress error " << GetLastError() << std::endl;
        }
        // Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
    }

    if (! fRunTimeLinkSuccess)
        std::cout << "Handle LoadLibrary error " << GetLastError() << std::endl;

    std::cout << "\ntotally clean" << std::endl;
    return 0;
}
