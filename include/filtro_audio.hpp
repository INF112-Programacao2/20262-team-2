#ifndef FILTRO_AUDIO_HPP
#define FILTRO_AUDIO_HPP
#include"dados_audio.hpp"

class FiltroAudio {
    private:


    public:
    virtual ~FiltroAudio(); //destrutor com virtual pra todas as classes derivadas 
    virtual void processar(DadosAudio &amostras);

};














#endif