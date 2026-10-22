#ifndef FILTRO_LIMPA_RUIDO_HPP
#define FILTRO_LIMPA_RUIDO_HPP
#include"filtro_audio.hpp"

//filtro que limpa ruidos -> sons fora da faixa de frequência da fala humana
class FiltroLimpaRuido : public FiltroAudio {
    private:
    double corte_grave; //fator multiplicativo pequeno -> remove frequências baixas
    double corte_agudo; //fator multiplicativo alto -> remove frequências altas

    public:

    //construtor
    explicit FiltroLimpaRuido();
    //destrutor
    ~FiltroLimpaRuido() override;
    //processar
    void processar(DadosAudio &audio) override;

};

#endif