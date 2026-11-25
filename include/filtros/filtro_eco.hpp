#ifndef FILTRO_ECO_HPP
#define FILTRO_ECO_HPP
#include "filtro_audio.hpp"

//eco/delay com realimentação: cada repetição soma o som de "atraso_ms" atrás, multiplicado por "repeticao"
//o áudio mantém a duração original: a "cauda" do eco que passaria do fim é cortada
class FiltroEco : public FiltroAudio {
    private:
        double atraso_ms; //tempo entre o som e cada repetição, em milissegundos: (0, 2000]
        double repeticao; //fração do volume que volta a cada repetição: [0.0, 0.95] (0 = sem eco)

    public:
        static constexpr double atraso_max_ms = 2000.0;
        static constexpr double repeticao_max = 0.95; //abaixo de 1.0 para o eco sempre diminuir até sumir

        //lança std::invalid_argument se algum parâmetro estiver fora dos intervalos acima
        FiltroEco(double atraso_ms, double repeticao);
        ~FiltroEco() override = default;

        void processar(DadosAudio &audio) override;

        double get_atraso_ms() const;
        double get_repeticao() const;

        std::string get_tipo() const override;
        std::string get_parametros() const override;
};

#endif