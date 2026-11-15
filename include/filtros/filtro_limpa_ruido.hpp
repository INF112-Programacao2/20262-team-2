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

    static const double PI = 3.14159265358979323846;
    static const double Q_BUTTERWORTH = 0.70710678118654752440;

    struct Biquad { double b0, b1, b2, a1, a2; };          // coeficientes do filtro
    struct Estado { double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0; }; // memória de um canal

    static Biquad passa_alta(double fc, double fs); //calcula os coeficientes do passa-alta
    static Biquad passa_baixa(double fc, double fs); //calcula os coeficientes do passa-baixa
    static double aplicar(const Biquad &f, Estado &e, double x); //calcula 1 amostra de saída e atualiza a memória

    public:

    static const double corte_grave_padrao = 300.0;
    static const double corte_agudo_padrao = 3400.0;

    //construtor
    explicit FiltroLimpaRuido(double corte_grave_hz = corte_grave_padrao, double corte_agudo_hz = corte_agudo_padrao);
    //destrutor
    ~FiltroLimpaRuido() override;
    //processar
    void processar(DadosAudio &audio) override;

    double get_corte_grave() const;
    double get_corte_agudo() const;

    std::string get_tipo() const override;
    std::string get_parametros() const override;

};

#endif