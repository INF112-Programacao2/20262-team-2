#include "cabecalho_WAV.hpp"

#include <cstring>

//Construtor que já cria um cabeçalho válido (PCM, 16 bits, estéreo, 44100 Hz, sem dados)
CabecalhoWav::CabecalhoWav()          
    : tamanho_arquivo(36), fmtSize(16), formato_audio(1),
      num_de_canais(2), sampleRate(44100), byteRate(176400),
      blockAlign(4), bitsPerSample(16), dataSize(0){
    std::memcpy(riffID, "RIFF", 4);
    std::memcpy(waveID, "WAVE", 4);
    std::memcpy(fmtID, "fmt ", 4);
    std::memcpy(dataID, "data", 4);
}

CabecalhoWav::~CabecalhoWav(){
}

void CabecalhoWav::recalcular_derivados(){
    blockAlign = static_cast<uint16_t>(num_de_canais * (bitsPerSample / 8));  //blockAlign = canais * bytes por amostra
    byteRate = sampleRate * blockAlign;  // byteRate = taxa * canais * bytes por amostra
    tamanho_arquivo = 36 + dataSize;     // tamanho_arquivo = tudo que vem dep´ois dos 8 primeiros bytes (36 de cabeçalho + dados)
}

//História 1: marcadores obrigatórios do formato .wav
bool CabecalhoWav::e_valido() const {
    return std::memcmp(riffID, "RIFF", 4) == 0 && std::memcmp(waveID, "WAVE", 4) == 0 && std::memcmp(fmtID, "fmt ", 4) == 0 && 
           std::memcmp(dataID, "data", 4) == 0 && fmtSize == 16; 
}

//DadosAudio guarda anmostras int16_t, só PCM de 16 bits que vai servir   
bool CabecalhoWav::e_suportado() const {
    return formato_audio == 1 && bitsPerSample == 16 &&
           num_de_canais >= 1 && blockAlign == num_de_canais * (bitsPerSample / 8);
}


// Getters
uint16_t CabecalhoWav::get_num_de_canais() const {
    return num_de_canais;
}

uint32_t CabecalhoWav::get_sample_rate() const {
    return sampleRate;
}

uint16_t CabecalhoWav::get_bits_per_sample() const {
    return bitsPerSample;
}

uint32_t CabecalhoWav::get_subchunk2_size() const {
    return dataSize;
}

uint16_t CabecalhoWav::get_audio_format() const {
    return formato_audio;
}

uint16_t CabecalhoWav::get_block_align() const {
    return blockAlign;
}

// Setters
void CabecalhoWav::set_num_de_canais(uint16_t canais) {
    num_de_canais = canais;
}

void CabecalhoWav::set_sample_rate(uint32_t sr) {
    sampleRate = sr;
}

void CabecalhoWav::set_subchunk2_size(uint32_t size) {
    dataSize = size;
}
