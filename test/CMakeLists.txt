cmake_minimum_required(VERSION 2.8)

set(PNAME splitspace-test)

project(${PNAME})

set(TEST_SRC 
    main.cpp
    splitspace/ConfigTest.cpp
    )

add_executable( ${PNAME} ${TEST_SRC} )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}../build/)
target_link_libraries (${PNAME} ${ENGINE} ${LIBS})

include_directories(../include/ ./)

