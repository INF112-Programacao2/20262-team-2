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

    if (amostra_audio == nullptr)
        return;
    

    for(uint32_t i=0;i<total_amostras;i++){
           amostra_audio[i] = clipping(static_cast<double>(amostra_audio[i]) * multiplicador); //aplica o clipping e o ganho
    }
}

std::string FiltroVolume::get_parametros() const {
    return "multiplicador=" + std::to_string(multiplicador);
}

std::string FiltroVolume::get_tipo() const {
    return "Volume";
}

