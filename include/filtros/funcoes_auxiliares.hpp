#ifndef FUNCOES_AUXILIARES_HPP
#define FUNCOES_AUXILIARES_HPP
#include<cstdint>
#include"filtro_audio.hpp"

//função de Clipping - mantém o valor das amostras de áudio entre o limite correto
    int16_t clipping(float valor);
    
#endif