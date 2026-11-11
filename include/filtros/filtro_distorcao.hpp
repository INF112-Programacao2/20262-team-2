#ifndef FILTRO_DISTORCAO_HPP
#define FILTRO_DISTORCAO_HPP
#include "filtro_audio.hpp"

class FiltroDistorcao : public FiltroAudio{
    private:
    double drive; //intensidade do empurrao do som
    double limite; //ponto onde a onda sera achatada - 0.0 a 1.0
    double reducao_volume_saida; //reducao de volume para nao estourar 0.0 a 1.0

    public:
    FiltroDistorcao(double drive, double limite, double reducao_volume_saida);
    ~FiltroDistorcao() override = default;
    
    void processar(DadosAudio &audio) override;
    std::string get_tipo() const override { return "Distorcao"; }
    std::string get_parametros() const override;

};

#endif