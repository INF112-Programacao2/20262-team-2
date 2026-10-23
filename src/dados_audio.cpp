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
//Getters
//Retorna o ponteiro para o array dinâmico contendo as amostras de áudio na RAM
int16_t* DadosAudio::get_audio() { 
    return audio; 
}

//Retorna a quantidade total de amostras de áudio armazenadas no vetor
uint32_t DadosAudio::get_num_amostras() const { 
    return num_amostras; 
}

//Retorna uma referencia editável do cabeçalho, vai permitir alterar os metadados 
CabecalhoWav& DadosAudio::get_cabecalho() { 
    return cabecalho; 
}

//Retorna uma referência constante do cabeçalho, garantindo acesso somente de leitura aos metadados
const CabecalhoWav& DadosAudio::get_cabecalho() const { 
    return cabecalho; 
}