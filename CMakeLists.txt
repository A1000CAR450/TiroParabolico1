cmake_minimum_required(VERSION 3.10)
project(TiroParabolicoRetro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agregar el ejecutable
add_executable(tiro_parabolico src/main.cpp)

# Opciones de compilación
if(MSVC)
    target_compile_options(tiro_parabolico PRIVATE /W4)
else()
    target_compile_options(tiro_parabolico PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Sistema operativo específico
if(WIN32)
    target_compile_definitions(tiro_parabolico PRIVATE WIN32)
endif()
