#include "filtro_volume.hpp"
#include "funcoes_auxiliares.hpp"
#include <cstdint>

//construtor
 FiltroVolume :: FiltroVolume (double porcentagem) 
: multiplicador(porcentagem/100.0) {}



void FiltroVolume :: processar(DadosAudio &audio) {
    int16_t* amostra_audio = audio.get_audio(); 
    uint32_t total_amostras = audio.get_num_amostras();

    for(int i=0;i<total_amostras;i++){
        double valor_processado = amostra_audio[i] * multiplicador; //aplica o multiplicador no volume

        amostra_audio[i] = clipping(valor_processado); //função auxiliar de clipping limita o som no intervalo correto
    }

}