cmake_minimum_required(VERSION 3.16)
project(caen743)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_ARCH "-m64")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${BUILD_ARCH}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${BUILD_ARCH}")

include_directories(${CMAKE_SOURCE_DIR} include lib)

add_executable(fastAcquisition src/main.cpp version.h src/CAEN743.cpp include/root.h src/FastSystem.cpp
        src/Crate.cpp include/Stoppable.h include/common.h src/Config.cpp
        src/Storage.cpp src/Chatter.cpp src/MessageQueue.cpp include/MessageQueue.h)
target_link_libraries(fastAcquisition ${CMAKE_CURRENT_SOURCE_DIR}/lib/CAENDigitizer.dll wsock32 ws2_32)
add_dependencies(fastAcquisition auto_increment_version)

add_executable(test src/tester.cpp)

ADD_CUSTOM_TARGET(
        auto_increment_version
        ${CMAKE_COMMAND}
        -D VERSION_FILE=${CMAKE_SOURCE_DIR}/version.h
        -P ${CMAKE_SOURCE_DIR}/AutoVersion.cmake
)