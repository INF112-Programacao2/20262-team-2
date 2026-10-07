#ifndef CABECALHO_HPP
#define CABECALHO_HPP

#include <cstdint>
#pragma pack(push, 1)

class CabecalhoWAV{
    private:

    char riffID[4];            // Bytes 0-3: Guarda os caracteres ASCII "RIFF", identificador do tipo de arquivo
    uint32_t tamanho_arquivo;  // Bytes 4-7: Armazena o tamanho total do ficheiro em bytes
    char waveID[4];            // Bytes 8-11: Guarda os caracteres ASCII "WAVE", confirma que o formato do áudio dentro do bloco RIFF é um ficheiro WAV
    char fmtID[4];             // Bytes 12-15: Guarda os caracteres ASCII "fmt ", marca o início do sub-bloco de formatação técnica do áudio
    uint32_t fmtSize;          // Bytes 16-19: Define o tamanho em bytes das informações de formato
    uint16_t formato_audio;    // Bytes 20-21: Indica o código do tipo de compressão/codificação do áudio
    uint16_t num_de_canais;    // Bytes 22-23: Guarda o número de canais de áudio. 1 para Mono ou 2 para Estéreo
    uint32_t sampleRate;       // Bytes 24-27: Taxa/frequência de amostragem em Hertz -> quantas leituras do sinal de áudio são feitas a cada segundo
    uint32_t byteRate;         // Bytes 28-31: Indica a quantidade de bytes de áudio processados por segundo
    uint16_t blockAlign;       // Bytes 32-33: Alinhamento de bloco: quantidade de bytes pra guardar exatamente 1 frame de áudio (considerando todos os canais)

    uint16_t bitsPerSample;    /** Bytes 34-35: Profundidade de bits: quantidade de bits de informação usada pra representar cada amostra individual de áudio.
                               Define a resolução vertical. Mais bits = maior precisão da qualidade do som.*/
    
    char dataID[4];            // Bytes 36-39: Guarda os caracteres ASCII "data", indica que a ficha técnica terminou e que o bloco de áudio real começa a seguir
    uint32_t dataSize;         // Bytes 40-43: Tamanho total do som/das amostras em bytes
};

#pragma pack(pop)
#endif