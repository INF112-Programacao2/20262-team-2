cmake_minimum_required(VERSION 3.16)
project(ProcessadorAudioGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # necessário para signals/slots
set(CMAKE_AUTORCC ON)   # incorpora a identidade visual ao executavel

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
    identidade_visual.qrc
)
target_link_libraries(gui PRIVATE Qt6::Widgets nucleo)

# ----- Diagnostico da GUI sem compilar ou vincular o nucleo -----
# Biblioteca de objetos: verifica a compilacao de cada arquivo da interface,
# incluindo sinais/slots Qt (AUTOMOC), mas nao produz executavel.
# Mantida separada de "gui" para nao alterar a compilacao normal do projeto.
add_library(gui_verificacao OBJECT
    main.cpp
    interfacegrafica.cpp
    interfacegrafica.hpp
    catalogo_filtros.hpp
    ponte_filtros.cpp
    ponte_filtros.hpp
)
target_include_directories(gui_verificacao PRIVATE
    ${RAIZ_PROJETO}/include
    ${RAIZ_PROJETO}/include/filtros
)
target_link_libraries(gui_verificacao PRIVATE Qt6::Widgets)

# ----- Reprodução opcional (Qt Multimedia) -----
# Mantém a GUI compilável mesmo sem o módulo de áudio instalado.
find_package(Qt6 QUIET COMPONENTS Multimedia)
if(TARGET Qt6::Multimedia)
    target_sources(gui PRIVATE reprodutor_audio.cpp reprodutor_audio.hpp)
    target_link_libraries(gui PRIVATE Qt6::Multimedia)
    target_compile_definitions(gui PRIVATE GUI_COM_AUDIO=1)

    target_sources(gui_verificacao PRIVATE reprodutor_audio.cpp reprodutor_audio.hpp)
    target_link_libraries(gui_verificacao PRIVATE Qt6::Multimedia)
    target_compile_definitions(gui_verificacao PRIVATE GUI_COM_AUDIO=1)
else()
    message(STATUS "Qt Multimedia não encontrado: botões de reprodução desabilitados")
    target_compile_definitions(gui PRIVATE GUI_COM_AUDIO=0)
    target_compile_definitions(gui_verificacao PRIVATE GUI_COM_AUDIO=0)
endif()
