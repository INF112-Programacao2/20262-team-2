#ifndef FILTRO_AUDIO_HPP
#define FILTRO_AUDIO_HPP

#include <string>
#include "dados_audio.hpp"

class FiltroAudio {
public:
    virtual ~FiltroAudio() = default;
    virtual void processar(DadosAudio& audio) = 0;


    virtual std::string get_tipo() const = 0;
    virtual std::string get_parametros() const = 0;
};

#endif // FILTRO_AUDIO_HPP

