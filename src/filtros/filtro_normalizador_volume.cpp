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


void FiltroNormalizador::processar(DadosAudio &audio) {
    int16_t *amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

    if (amostras == nullptr || total_amostras == 0)
        return;

    //1 passada: procura o pico (maior valor absoluto) de todo o áudio
    //int32_t porque o valor absoluto de -32768 não cabe em int16_t, positivo so vai ate 32767
    int32_t pico = 0;
    for (uint32_t i = 0; i < total_amostras; i++) {
        int32_t valor = amostras[i];
        if (valor < 0)
            valor = -valor;
        if (valor > pico)
            pico = valor;
    }

    if (pico == 0)
        return; //áudio em silêncio total, não há o que normalizar

    //ganho que leva o pico até o alvo, limitado pelo ganho máximo permitido
    double ganho = (alvo * 32767.0) / static_cast<double>(pico);
    const double ganho_maximo = std::pow(10.0, ganho_max_db / 20.0);
    if (ganho > ganho_maximo)
        ganho = ganho_maximo;

    if (ganho == 1.0)
        return; //já está no nível desejado

    //2 passada: aplica o mesmo ganho em todas as amostras de audio
    for (uint32_t i = 0; i < total_amostras; i++)
        amostras[i] = clipping(static_cast<double>(amostras[i]) * ganho);
}