#include "filtro_ganho_canal.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

//construtor
FiltroGanhoCanal::FiltroGanhoCanal(double ganho_esquerdo_db, double ganho_direito_db)
    : ganho_esquerdo_db(ganho_esquerdo_db), ganho_direito_db(ganho_direito_db) {

    if (!(ganho_esquerdo_db >= -ganho_max_db && ganho_esquerdo_db <= ganho_max_db))
        throw std::invalid_argument("FiltroGanhoCanal: ganho esquerdo fora do intervalo permitido (-24 a +24 dB)");
    if (!(ganho_direito_db >= -ganho_max_db && ganho_direito_db <= ganho_max_db))
        throw std::invalid_argument("FiltroGanhoCanal: ganho direito fora do intervalo permitido (-24 a +24 dB)");
}

