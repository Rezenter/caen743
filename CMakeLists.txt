cmake_minimum_required(VERSION 3.16)
project(caen743)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static")
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)


include_directories(include lib include/mds lib/mds)

add_executable(example src/ReadoutTest_Digitizer.c version.h)
target_link_libraries(example ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll)

add_library(caen743 SHARED src/CAEN743.cpp src/CAEN743.h src/root.h include/mds/mdsobjects.h)
target_link_libraries(caen743 ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mds/mdsobjectscppshr.lib)

add_executable(test src/tester.cpp src/CAEN743.h)
add_dependencies(test caen743)


#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSiSens.so
#        DESTINATION /usr/share/labbot/modules/)





ADD_CUSTOM_TARGET(
        auto_increment_version
        ${CMAKE_COMMAND}
        -D VERSION_FILE=${CMAKE_SOURCE_DIR}/version.h
        -P ${CMAKE_SOURCE_DIR}/AutoVersion.cmake
)
ADD_DEPENDENCIES(caen743 auto_increment_version)