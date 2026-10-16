#ifndef FUNCOES_AUXIXLIARES.HPP
#define FUNCOES_AUXILIARES.HPP
#include<cstdint>
#include"filtro_audio.hpp"

//função de Clipping - mantém o valor das amostras de áudio entre o limite correto
    static int16_t clipping(float valor);
    
#endif