#include "filtro_realce_freq.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>

//construtor
FiltroRealceFreq::FiltroRealceFreq(double frequencia_central, double ganho_db, double largura_faixa)
    : frequencia_central(frequencia_central), ganho_db(ganho_db), largura_faixa(largura_faixa) {

    if (!(frequencia_central > 0.0))
        throw std::invalid_argument("FiltroRealceFreq: frequencia central deve ser maior que 0 Hz");
    if (!(largura_faixa > 0.0))
        throw std::invalid_argument("FiltroRealceFreq: largura deve ser maior que 0");
    if (!(ganho_db >= -ganho_max_db && ganho_db <= ganho_max_db))
        throw std::invalid_argument("FiltroRealceFreq: ganho fora do intervalo permitido (-24 a +24 dB)");
}

//destrutor
FiltroRealceFreq::~FiltroRealceFreq() {
}

