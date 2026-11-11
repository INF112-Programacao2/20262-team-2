#include "cabecalho_WAV.hpp"

CabecalhoWav::CabecalhoWav()          //Construtor
    : tamanho_arquivo(0), fmtSize(16), formato_audio(1),
      num_de_canais(2), sampleRate(44100), byteRate(176400),
      blockAlign(4), bitsPerSample(16), dataSize(0){
}