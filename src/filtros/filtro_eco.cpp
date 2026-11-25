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

void FiltroEco::processar(DadosAudio &audio) {
    int16_t *amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras(); //total de amostras (canais intercalados)
    uint32_t canais = audio.get_num_canais();
    double fs = static_cast<double>(audio.get_sample_rate());

    if (amostras == nullptr || total_amostras == 0 || canais == 0 || repeticao == 0.0)
        return;

    //atraso em frames (uma amostra de cada canal); mínimo de 1 frame
    uint64_t atraso_frames = static_cast<uint64_t>(std::llround(atraso_ms * fs / 1000.0));
    if (atraso_frames < 1)
        atraso_frames = 1;

    //no vetor intercalado (L R L R...), voltar "atraso_frames * canais" posições cai no MESMO canal
    uint64_t atraso = atraso_frames * canais;
    if (atraso >= total_amostras)
        return; //o áudio é mais curto que o atraso: nenhuma repetição cabe

    //realimentação: a amostra anterior já inclui os ecos passados, por isso as repetições vão diminuindo
    for (uint64_t i = atraso; i < total_amostras; i++) {
        double y = static_cast<double>(amostras[i]) + repeticao * static_cast<double>(amostras[i - atraso]);
        amostras[i] = clipping(y);
    }
}