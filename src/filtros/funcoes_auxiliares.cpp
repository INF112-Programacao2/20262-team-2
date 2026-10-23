#include"funcoes_auxiliares.hpp"
#include<iostream>
#include<cstdint>


//função de clipping
int16_t clipping (float valor) {
    if (valor > 32767.0)
        return 32767;
    if (valor < -32767.0)
        return -32767;

    return static_cast<int16_t>(valor); //retorna o valor float convertido para binário
}
