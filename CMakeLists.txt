cmake_minimum_required(VERSION 2.8)

set(ENGINE splitspace)
set(DEMO splitspace-demo)

project(${ENGINE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")

find_library(gl_lib GL)
find_library(sdl2_lib SDL2)
find_library(assimp_lib assimp)
find_library(SOIL_lib SOIL)
find_library(GLEW_lib GLEW)
set(bullet_lib BulletDynamics BulletCollision LinearMath)
set(bullet_incdir /usr/include/bullet)

message("LIBS: ${gl_lib} ${sdl2_lib} ${assimp_lib} ${bullet_lib} ${SOIL_lib}")

set(ENGINE_SRC 
    src/Engine.cpp
    src/LogManager.cpp
    src/EventManager.cpp
    src/WindowManager.cpp
    src/RenderManager.cpp
    src/SceneManager.cpp
    src/ResourceManager.cpp
    src/PhysicsManager.cpp
    src/Config.cpp
    src/Resource.cpp
    src/Texture.cpp
    src/Entity.cpp
    src/Scene.cpp
    )

set(DEMO_SRC
    demo/main.cpp
    )

include_directories(include ${bullet_incdir} ${CMAKE_SOURCE_DIR})

add_library(${ENGINE} STATIC ${ENGINE_SRC})
add_executable(${DEMO} ${DEMO_SRC})

target_link_libraries(${DEMO} ${ENGINE} ${gl_lib} ${sdl2_lib} ${assimp_lib} ${bullet_lib} ${SOIL_lib} ${GLEW_lib})

