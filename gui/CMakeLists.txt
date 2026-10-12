cmake_minimum_required(VERSION 3.16)
project(ProcessadorAudioGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # necessário para signals/slots

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(gui
    main.cpp
    interfacegrafica.cpp
    interfacegrafica.hpp
    catalogo_filtros.hpp
)
target_link_libraries(gui PRIVATE Qt6::Widgets)
