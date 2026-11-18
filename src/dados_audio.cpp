#include "dados_audio.hpp"

#include <limits>  //Usado pra consultar os valores máximos e mínimos que cada tipo 
                   //numérico suporta na memória (através de std::numeric_limits)
#include <stdexcept>   //Fornece as classes padrão de exceções para interromper o programa 
#include <utility> //Necessário para usar std::move e transferir a propriedade do 
                   //ponteiro inteligente recebido pelo construtor para o atributo audio.

//Construtor
DadosAudio::DadosAudio(const CabecalhoWav &cabecalho,
    std::unique_ptr<int16_t[]> audio, uint32_t num_amostras) 
    : cabecalho(cabecalho), audio(std::move(audio)), num_amostras(num_amostras){  
    if(this->audio == nullptr && num_amostras > 0){
        throw std::invalid_argument("DadosAudio: vetor de amostras nulo, mas num_amostras > 0");
    }
}

//Destrutor: mas não há nada pra liberar manualmente, o unique_ptr (ponteiro inteligente) libera o vetor sozinho
DadosAudio::~DadosAudio(){
}

//Getters
//Retorna o ponteiro inteligente para o array dinâmico contendo as amostras de áudio na RAM
int16_t* DadosAudio::get_audio() { 
    return audio.get(); 
}

int16_t* DadosAudio::get_amostras() {      //retorna um ponteiro editável
    return audio.get();
}

const int16_t* DadosAudio::get_amostras() const {    // Retorna um ponteiro somente para leitura.
    return audio.get();
}

//Retorna a quantidade total de amostras de áudio armazenadas no vetor
uint32_t DadosAudio::get_num_amostras() const { 
    return num_amostras; 
}

//Troca o vetor: o antigo é liberado automaticamente ao receber o novo
void DadosAudio::substituir_amostras(std::unique_ptr<int16_t[]> novas, uint32_t quantidade){
    if(novas == nullptr && quantidade > 0){
        throw std::invalid_argument("DadosAudio: vetor de amostras nulo, mas quantidade > 0");
    }
    uint64_t bytes = static_cast<uint64_t>(quantidade) * sizeof(int16_t);
    if(bytes > std::numeric_limits<uint32_t>::max() - 36){
        throw std::length_error("DadosAudio: audio grande demais para o formato WAV (limite de 4 GB)");
    }

    audio = std::move(novas);
    num_amostras = quantidade;
    cabecalho.set_subchunk2_size(static_cast<uint32_t>(bytes));
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
    cabecalho.set_num_de_canais(canais);
}

void DadosAudio::set_sample_rate(uint32_t sample_rate) {
    cabecalho.set_sample_rate(sample_rate);
}