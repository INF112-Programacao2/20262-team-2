#ifndef FILTRO_DISTORCAO_HPP
#define FILTRO_DISTORCAO_HPP
#include "filtro_audio.hpp"

class FiltroDistorcao : public FiltroAudio{
    private:
    double drive; //intensidade do empurrao do som
    double limite; //ponto onde a onda sera achatada
    double reducao_volume_saida; //reducao de volume para nao estourar

    public:
    explicit FiltroDistorcao(double drive, double limite, double reducao_volume_saida);
    ~FiltroDistorcao() override;
    
    void processar(DadosAudio &audio) override;

};

#endif