cmake_minimum_required(VERSION 2.8)

set(PNAME splitspace-test)

project(${PNAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(TEST_SRC 
    main.cpp
    splitspace/ConfigTest.cpp
    splitspace/EntityTest.cpp
    splitspace/EventManagerTest.cpp
    )

add_executable( ${PNAME} ${TEST_SRC} )
target_link_libraries (${PNAME} ${CMAKE_SOURCE_DIR}/build/libsplitspace.a ${LIBS})

include_directories(../include/ ./)

