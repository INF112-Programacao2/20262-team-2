#ifndef DADOS_AUDIO_H
#define DADOS_AUDIO_H

#include <cstddef>
#include <cstdint>
#include "cabecalho_WAV.hpp"


//Guarda o áudio em memória (RAM): metadados + amostras.
class DadosAudio {
private:
    CabecalhoWav _cabecalho;   // metadados (composição)
    double *_amostras;          // heap: num_canais * num_quadros floats

    std::size_t indice(int canal, int quadro) const;

public:
    
    explicit DadosAudio(const CabecalhoWav &cabecalho);

    //Libera o vetor de amostras (cada new[] -> um delete[]). 
    ~DadosAudio();

    // Cópia proibida: com ponteiro cru, copiar causaria delete duplo.
    DadosAudio(const DadosAudio &) = delete;
    DadosAudio &operator=(const DadosAudio &) = delete;

    // ---- Metadados (repassados do cabeçalho) ----
    int get_num_canais() const;
    int get_taxa_amostragem() const;
    int get_bits_por_amostra() const;
    int get_num_quadros() const;             
    std::size_t get_total_amostras() const;  
    double get_duracao_segundos() const;
    // Cabeçalho atualizado, usado pelo exportador. 
    const CabecalhoWav &get_cabecalho() const;

    // ---- Amostras ----
    float get_amostra(int canal, int quadro) const;
    
    void set_amostra(int canal, int quadro, float valor);

    void redimensionar(int novo_num_quadros);
};

#endif
