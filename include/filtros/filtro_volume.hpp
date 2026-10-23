#ifndef FILTRO_VOLUME
#define FILTRO_VOLUME
#include "filtro_audio.hpp"

//filtro de aumento e redução do volume (ganho)
class FiltroVolume : public FiltroAudio {
    private:
        double multiplicador; //recebe uma porcentagem que será o multiplicador de volume do som

    public:

        //construtor
        explicit FiltroVolume(double porcentagem); //explicit impede que o compilador faça conversões de tipos de variáveis
        ~FiltroVolume() override;
        //sobreescreve o método original
        void processar(DadosAudio &amostras) override;

};
#endif