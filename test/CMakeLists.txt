cmake_minimum_required(VERSION 2.8)

set(PNAME splitspace-test)

project(${PNAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(TEST_SRC 
    main.cpp
    splitspace/ConfigTest.cpp
    splitspace/EntityTest.cpp
    splitspace/EventManagerTest.cpp
    splitspace/ResourceManagerTest.cpp
    )

link_directories(${CMAKE_SOURCE_DIR}/build/ ${CMAKE_SOURCE_DIR}/lib/)

add_executable(${PNAME} ${TEST_SRC})
target_link_libraries (${PNAME} splitspace ${SPLITSPACE_LIBS})

include_directories(${CMAKE_SOURCE_DIR}/include/ )

