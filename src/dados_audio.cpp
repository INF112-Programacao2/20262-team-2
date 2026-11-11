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

// --- Getters dos metadados ---
uint16_t DadosAudio::get_num_canais() const {
    return cabecalho.get_num_de_canais();
}

uint32_t DadosAudio::get_sample_rate() const {
    return cabecalho.get_sample_rate();
}

// --- Setters dos metadados ---
void DadosAudio::set_num_canais(uint16_t canais) {
    cabecalho.num_de_canais = canais;
}

void DadosAudio::set_sample_rate(uint32_t sample_rate) {
    cabecalho.sample_rate = sample_rate;
}