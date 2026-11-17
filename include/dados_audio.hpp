#ifndef DADOS_AUDIO_HPP
#define DADOS_AUDIO_HPP

#include <cstdint>
#include <memory>
#include "cabecalho_WAV.hpp"


//Guarda o áudio em memória (RAM): metadados + amostras.
class DadosAudio {
private:
    CabecalhoWav cabecalho;   // metadados (composição)

    //int16_t = tipo de variável que armazena dados binários
    std::unique_ptr<int16_t[]> audio;   //ponteiro inteligente pro vetor dinâmico
    uint32_t num_amostras;       //quantidade de elementos do vetor

public:
    //Construtor
    DadosAudio(const CabecalhoWav &cabecalho, std::unique_ptr<int16_t[]> audio, uint32_t num_amostras);

    // Bloqueio de cópia para prevenir double-free
    DadosAudio(const DadosAudio&) = delete;
    DadosAudio& operator=(const DadosAudio&) = delete;

    // Getters de amostras
    int16_t* get_amostras();
    const int16_t* get_amostras() const;
    int16_t* get_audio();
    uint32_t get_num_amostras() const;

    // Getters de cabeçalho
    CabecalhoWav& get_cabecalho();
    const CabecalhoWav& get_cabecalho() const;

    // Getters e Setters dos metadados para os filtros DSP
    uint16_t get_num_canais() const;
    uint32_t get_sample_rate() const;
    void set_num_canais(uint16_t canais);
    void set_sample_rate(uint32_t sample_rate);

};

#endif
