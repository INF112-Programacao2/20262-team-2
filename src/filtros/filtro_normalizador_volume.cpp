#include "filtro_normalizador_volume.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

//construtor
FiltroNormalizador::FiltroNormalizador(double alvo, double ganho_max_db)
    : alvo(alvo), ganho_max_db(ganho_max_db) {

    if (!(alvo > 0.0 && alvo <= 1.0))
        throw std::invalid_argument("FiltroNormalizador: o alvo deve estar no intervalo (0.0, 1.0]");
    if (!(ganho_max_db >= 0.0 && ganho_max_db <= ganho_max_limite_db))
        throw std::invalid_argument("FiltroNormalizador: o ganho maximo deve estar entre 0 e 40 dB");
}

double FiltroNormalizador::get_alvo() const { 
    return alvo; 
}
double FiltroNormalizador::get_ganho_max_db() const { 
    return ganho_max_db; 
}


std::string FiltroNormalizador::get_tipo() const {
    return "Normalizador";
}

std::string FiltroNormalizador::get_parametros() const {
    return "alvo=" + std::to_string(alvo) +
           ",ganho_max_db=" + std::to_string(ganho_max_db);
}