#ifndef FILTRO_LIMPA_RUIDO_HPP
#define FILTRO_LIMPA_RUIDO_HPP
#include"filtro_audio.hpp"

//filtro que limpa ruidos -> sons fora da faixa de frequência da fala humana
//passa-alta (corta os graves) seguido de passa-baixa (corta os agudos)
//por padrão deixa passar de 300 Hz a 3400 Hz, a faixa padrão da fala
class FiltroLimpaRuido : public FiltroAudio {
    private:
    double corte_grave; //abaixo dessa frequencia o som eh atenuado
    double corte_agudo; //acima dessa frequencia o som eh atenuado

    public:

    static const double corte_grave_padrao_hz = 300.0;
    static const double corte_agudo_padrao_hz = 3400.0;

    //construtor
    explicit FiltroLimpaRuido(double corte_grave_hz = corte_grave_padrao_hz, double corte_agudo_hz = corte_agudo_padrao_hz);
    //destrutor
    ~FiltroLimpaRuido() override;
    //processar
    void processar(DadosAudio &audio) override;

    double get_corte_grave_hz() const;
    double get_corte_agudo_hz() const;

    std::string get_tipo() const override;
    std::string get_parametros() const override;

};

#endif