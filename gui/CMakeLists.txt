cmake_minimum_required(VERSION 3.16)
project(ProcessadorAudioGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # necessário para signals/slots

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ----- Núcleo do projeto (src/ e include/ na raiz do repositório) -----
# Compila todos os .cpp de ../src, menos main.cpp (a interface tem o seu próprio main).
# Arquivos novos em src/ entram automaticamente na próxima compilação.
set(RAIZ_PROJETO ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB_RECURSE FONTES_NUCLEO CONFIGURE_DEPENDS ${RAIZ_PROJETO}/src/*.cpp)
list(FILTER FONTES_NUCLEO EXCLUDE REGEX "/main\\.cpp$")

add_library(nucleo STATIC ${FONTES_NUCLEO})
target_include_directories(nucleo PUBLIC
    ${RAIZ_PROJETO}/include
    ${RAIZ_PROJETO}/include/filtros
)

# ----- Interface gráfica -----
add_executable(gui
    main.cpp
    interfacegrafica.cpp
    interfacegrafica.hpp
    catalogo_filtros.hpp
    ponte_filtros.cpp
    ponte_filtros.hpp
)
target_link_libraries(gui PRIVATE Qt6::Widgets nucleo)
