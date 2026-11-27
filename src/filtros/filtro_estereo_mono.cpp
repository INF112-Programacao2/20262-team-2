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

void FiltroEstereoMono::processar(DadosAudio &audio) {
    int16_t *amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

    if (amostras == nullptr || total_amostras < 2)
        return;
    if (audio.get_num_canais() != 2)
        return; //só faz sentido em estéreo, ou seja, precisa de ter dois canais

    //no vetor intercalado (L R L R), cada par de posições é um frame de som: [i] = esquerdo, [i + 1] = direito

        for (uint32_t i = 0; i + 1 < total_amostras; i += 2) {
            double media = (static_cast<double>(amostras[i]) + static_cast<double>(amostras[i + 1])) / 2.0; // calcula a media do par de som do canal esquerdo e direito
            int16_t valor = clipping(media);
            amostras[i] = valor;      //aplica o valor medio em ambos os canais
            amostras[i + 1] = valor;
    }




}