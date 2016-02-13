cmake_minimum_required(VERSION 2.8)

set(PNAME TEST)

project(${PNAME})

set(TEST_SRC 
    main.cpp
    splitspace/ConfigTest.cpp
    )

add_definitions(-DTEST_DATA_DIR="test_data/")

add_executable( ${PNAME} ${TEST_SRC} )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}../build/)
target_link_libraries (${PNAME} ${ENGINE} ${LIBS})

include_directories(../include/ ./)

