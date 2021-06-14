cmake_minimum_required(VERSION 2.8) 

project(pcan)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG -g -w")
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -DDEBUG -g -w")

#add_definitions(-DSiE_EXTERNAL_CONFIG)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third-party/pcan/Include
    )

file(GLOB uds_SRC
    )

set(SOURCES 
    ${CMAKE_SOURCE_DIR}/src/PCANBasicWrapper.cpp
    ${CMAKE_SOURCE_DIR}/src/SimpleTimer.cpp
    )


add_executable(pcan src/main.cpp ${SOURCES})

target_link_libraries(pcan ${CMAKE_SOURCE_DIR}/third-party/pcan/x64/VC_LIB/PCANBasic.lib)