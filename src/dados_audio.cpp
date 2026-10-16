#include "dados_audio.hpp"

//Construtor
DadosAudio::DadosAudio(const CabecalhoWav &cabecalho, int16_t *audio, uint32_t num_amostras) 
    : cabecalho(cabecalho), audio(audio), num_amostras(num_amostras){  
}

//Destrutor: Libera a memória RAM alocada dinamicamente
DadosAudio::~DadosAudio(){
    if (audio != nullptr){
        delete[] audio;
        audio = nullptr;
    }
}