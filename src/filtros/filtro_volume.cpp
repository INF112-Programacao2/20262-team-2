#include "filtro_volume.hpp"
#include "funcoes_auxiliares.hpp"
#include <cstdint>
#include <stdexcept>

//construtor
 FiltroVolume :: FiltroVolume (double multiplicador) 
: multiplicador(multiplicador) {
    if(multiplicador <0.0){
        throw std::invalid_argument ("Fator de volume deve ser maior que 0.");
    }
}



void FiltroVolume :: processar(DadosAudio &audio) {
    int16_t* amostra_audio = audio.get_audio(); 
    uint32_t total_amostras = audio.get_num_amostras();

    for(int i=0;i<total_amostras;i++){
        double valor_processado = static_cast<double>(amostra_audio[i]) * multiplicador; //aplica o multiplicador no volume

        valor_processado = clipping(); //função auxiliar de clipping limita o som no intervalo correto
        amostra_audio[i] = static_cast<int16_t>(valor_processado); //convertendo de volta pra int16_t
    }

}

std::string FiltroVolume::get_parametros() const {
    return "multiplicador=" + std::to_string(multiplicador);
}