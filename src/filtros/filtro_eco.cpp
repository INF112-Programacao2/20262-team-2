#include "filtro_eco.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

//construtor
FiltroEco::FiltroEco(double atraso_ms, double repeticao)
    : atraso_ms(atraso_ms), repeticao(repeticao) {

    if (!(atraso_ms > 0.0 && atraso_ms <= atraso_max_ms))
        throw std::invalid_argument("FiltroEco: atraso deve estar entre 0 e 2000 ms");
    if (!(repeticao >= 0.0 && repeticao <= repeticao_max))
        throw std::invalid_argument("FiltroEco: repeticao deve estar entre 0.0 e 0.95");
}

#include "filtro_eco.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

//construtor
FiltroEco::FiltroEco(double atraso_ms, double repeticao)
    : atraso_ms(atraso_ms), repeticao(repeticao) {

    if (!(atraso_ms > 0.0 && atraso_ms <= atraso_max_ms))
        throw std::invalid_argument("FiltroEco: atraso deve estar entre 0 (exclusivo) e 2000 ms");
    if (!(repeticao >= 0.0 && repeticao <= repeticao_max))
        throw std::invalid_argument("FiltroEco: repeticao deve estar entre 0.0 e 0.95");
}



double FiltroEco::get_atraso_ms() const { 
    return atraso_ms; 
}
double FiltroEco::get_repeticao() const { 
    return repeticao; 
}

std::string FiltroEco::get_tipo() const {
    return "Eco";
}

std::string FiltroEco::get_parametros() const {
    return "atraso_ms=" + std::to_string(atraso_ms) +
           ",repeticao=" + std::to_string(repeticao);
}