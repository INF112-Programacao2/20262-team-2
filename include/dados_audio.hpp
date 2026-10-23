#ifndef DADOS_AUDIO_HPP
#define DADOS_AUDIO_HPP

#include <cstddef>
#include <cstdint>
#include "cabecalho_WAV.hpp"


//Guarda o áudio em memória (RAM): metadados + amostras.
class DadosAudio {
private:
    CabecalhoWav cabecalho;   // metadados (composição)
    //int16_t = tipo de variável que armazena dados binários
    int16_t *audio;          //ponteiro pro vetor dinâmico
    uint32_t num_amostras;       //quantidade de elementos do vetor

public:
    //Construtor
    DadosAudio(const CabecalhoWav &cabecalho, int16_t *audio, uint32_t num_amostras);

    //Destrutor
    ~DadosAudio();

    //Getter array dinâmico
    int16_t* get_audio(); 
    //Getter quantidade de elementos do array
    uint32_t get_num_amostras() const;
    //Getters que retornam o objeto cabecalhowav, assim pra utilizar um dado do cabecalho basta chamar o getter e colocar .variavel
    CabecalhoWav& get_cabecalho(); //esse permite alteração dos dados
    const CabecalhoWav& get_cabecalho()const; //esse permite apenas leitura e impede que o método modifique algo


};

#endif
