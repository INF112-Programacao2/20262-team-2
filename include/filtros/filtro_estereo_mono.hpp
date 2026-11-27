#ifndef FILTRO_ESTEREO_MONO_HPP
#define FILTRO_ESTEREO_MONO_HPP
#include "filtro_audio.hpp"

//estéreo para mono: troca os dois canais pela média deles, ou seja, o mesmo som sai nos dois lados
//útil para quem ouve melhor de um ouvido ou usa um fone só: nada fica escondido em um lado só
//o arquivo continua com 2 canais (agora iguais), então o cabeçalho do WAV não muda

class FiltroEstereoMono : public FiltroAudio {
    public:
        FiltroEstereoMono() = default;
        ~FiltroEstereoMono() override = default;

        void processar(DadosAudio &audio) override;

        std::string get_tipo() const override;
        std::string get_parametros() const override; //não tem parâmetros: devolve texto vazio
};

#endif