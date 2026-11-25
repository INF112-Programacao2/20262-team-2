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

