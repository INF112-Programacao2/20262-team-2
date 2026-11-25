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

double FiltroGanhoCanal::get_ganho_esquerdo_db() const { 
    return ganho_esquerdo_db; 
}
double FiltroGanhoCanal::get_ganho_direito_db() const { 
    return ganho_direito_db; 
}

std::string FiltroGanhoCanal::get_tipo() const {
    return "GanhoCanal";
}

std::string FiltroGanhoCanal::get_parametros() const {
    return "ganho_esquerdo_db=" + std::to_string(ganho_esquerdo_db) +
           ",ganho_direito_db=" + std::to_string(ganho_direito_db);
}


void FiltroGanhoCanal::processar(DadosAudio &audio) {
    int16_t *amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras(); //total de amostras (canais intercalados)

    if (amostras == nullptr || total_amostras == 0)
        return;
    if (audio.get_num_canais() != 2)
        return; //só faz sentido em estéreo, se for 1 canal só está em mono
    if (ganho_esquerdo_db == 0.0 && ganho_direito_db == 0.0)
        return; //nada a fazer

    //converte dB para multiplicador: +6 dB dobra a amplitude, -6 dB divide por 2
    const double mult_esquerdo = std::pow(10.0, ganho_esquerdo_db / 20.0);
    const double mult_direito = std::pow(10.0, ganho_direito_db / 20.0);

    //no vetor intercalado (L R L R...), posições pares são o esquerdo e ímpares são o direito
    for (uint32_t i = 0; i < total_amostras; i++) {
        double mult = (i % 2 == 0) ? mult_esquerdo : mult_direito; //usando o operador pra indicar qual lado aumentar
        amostras[i] = clipping(static_cast<double>(amostras[i]) * mult);
    }
}
