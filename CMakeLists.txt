cmake_minimum_required(VERSION 3.1)

set(ENGINE splitspace)

project(${ENGINE})

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_library(gl_lib GL)
find_library(sdl2_lib SDL2)
find_library(assimp_lib assimp)
find_library(SOIL_lib SOIL)
find_library(GLEW_lib GLEW)

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(SPLITSPACE_LIBS GL GLEW SDL2 assimp SOIL PARENT_SCOPE)
else()
    set(SPLITSPACE_LIBS GL GLEW SDL2 assimp SOIL)
endif()

message("SPLITSPACE_LIBS: ${SPLITSPACE_LIBS}")

set(ENGINE_SRC 
    src/Engine.cpp
    src/LogManager.cpp
    src/EventManager.cpp
    src/WindowManager.cpp
    src/RenderManager.cpp
    src/ResourceManager.cpp
    src/PhysicsManager.cpp
    src/Config.cpp
    src/Resource.cpp
    src/Texture.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/Object.cpp
    src/Material.cpp
    src/Mesh.cpp
    src/Light.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/RenderTechnique.cpp
    src/ForwardRenderTechnique.cpp
    src/DefferedRenderTechnique.cpp
    )

include_directories(include ${CMAKE_SOURCE_DIR})

add_library(${ENGINE} STATIC ${ENGINE_SRC})

if(${RUN_TESTS})
    add_subdirectory(test)
endif()

