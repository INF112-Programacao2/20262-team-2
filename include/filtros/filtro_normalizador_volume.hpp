#ifndef FILTRO_NORMALIZADOR_HPP
#define FILTRO_NORMALIZADOR_HPP
#include "filtro_audio.hpp"

//normalizador de volume: acha o ponto mais alto (pico) do áudio inteiro e aplica UM ganho único
//que leva esse pico até o valor "alvo". Útil para áudios baixos demais, sem precisar escolher o ganho à mão
//o mesmo ganho vale para todos os canais, então a proporção entre esquerdo e direito é mantida
//se o áudio já estiver mais alto que o alvo, o ganho é menor que 1 (o áudio é abaixado até o alvo)
class FiltroNormalizador : public FiltroAudio {
    private:
        double alvo;          //nível desejado para o pico, como fração do máximo: (0.0, 1.0] (0.9 = 90%)
        double ganho_max_db;  //limite de amplificação em dB, para não ampliar muito um áudio quase silencioso

    public:
        static constexpr double alvo_padrao = 0.9;
        static constexpr double ganho_max_padrao_db = 30.0;
        static constexpr double ganho_max_limite_db = 40.0; //valor máximo aceito para ganho_max_db

        //lança std::invalid_argument se alvo estiver fora de (0, 1] ou ganho_max_db fora de [0, 40]
        explicit FiltroNormalizador(double alvo = alvo_padrao, double ganho_max_db = ganho_max_padrao_db);
        ~FiltroNormalizador() override = default;

        void processar(DadosAudio &audio) override;

        double get_alvo() const;
        double get_ganho_max_db() const;

        std::string get_tipo() const override;
        std::string get_parametros() const override;
};

#endif