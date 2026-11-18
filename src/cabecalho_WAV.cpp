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

// Função que carrega os dados do cabeçalhoWAV a partir de um buffer (retornando true pra permitir compilar)
bool CabecalhoWav::e_valida(const char *buffer) {
    (void)buffer;
    return true;
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
