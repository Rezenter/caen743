cmake_minimum_required(VERSION 3.16)
project(caen743)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_ARCH "-m64")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${BUILD_ARCH}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${BUILD_ARCH}")

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)


include_directories(${CMAKE_SOURCE_DIR} include lib include/mds lib/mds)

add_executable(example src/ReadoutTest_Digitizer.c version.h)
target_link_libraries(example ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll)

add_executable(fastAcquisition src/main.cpp version.h src/CAEN743.cpp include/root.h src/FastSystem.cpp
        src/Crate.cpp include/Stoppable.h include/common.h src/Config.cpp
        src/Storage.cpp src/Chatter.cpp)
target_link_libraries(fastAcquisition ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll wsock32 ws2_32)
add_dependencies(fastAcquisition auto_increment_version)

#add_library(caen743 SHARED src/CAEN743.cpp src/CAEN743.h src/root.h include/mds/mdsobjects.h)
#target_link_libraries(caen743 ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll
#        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mds/mdsobjectscppshr.lib)

add_executable(test src/tester.cpp)
#target_link_libraries(test caen743)
#add_dependencies(test caen743)

#add_executable(copyTree src/copyTreeExample.cpp )
#target_link_libraries(copyTree ${CMAKE_CURRENT_SOURCE_DIR}/lib/mds/mdsobjectscppshr.lib)

#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSiSens.so
#        DESTINATION /usr/share/labbot/modules/)

ADD_CUSTOM_TARGET(
        auto_increment_version
        ${CMAKE_COMMAND}
        -D VERSION_FILE=${CMAKE_SOURCE_DIR}/version.h
        -P ${CMAKE_SOURCE_DIR}/AutoVersion.cmake
)