#include "filtro_estereo_mono.hpp"
#include "funcoes_auxiliares.hpp"
#include <cstdint>
#include <string>

//não tem construtor pois só processa o audio com os dados dele mesmo, nenhum parametro é passado ou inicializado

std::string FiltroEstereoMono::get_tipo() const {
    return "EstereoMono";
}

std::string FiltroEstereoMono::get_parametros() const {
    return "";
}