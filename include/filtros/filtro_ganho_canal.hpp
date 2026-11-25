#ifndef FILTRO_GANHO_CANAL_HPP
#define FILTRO_GANHO_CANAL_HPP
#include "filtro_audio.hpp"

//ganho separado para cada ouvido: um volume para o canal esquerdo e outro para o direito
//útil para quem ouve pior de um ouvido (acessibilidade) - funciona bem usando fones de ouvido
//só atua em áudio estéreo (2 canais); em mono ou em outro número de canais não altera nada
class FiltroGanhoCanal : public FiltroAudio {
    private:
        double ganho_esquerdo_db; //ganho do canal esquerdo em dB (0 = igual, positivo aumenta, negativo diminui)
        double ganho_direito_db;  //ganho do canal direito em dB

    public:
        static constexpr double ganho_max_db = 24.0; //limite de segurança: de -24 a +24 dB

        //lança std::invalid_argument se algum ganho estiver fora do intervalo
        FiltroGanhoCanal(double ganho_esquerdo_db, double ganho_direito_db);
        ~FiltroGanhoCanal() override = default;

        void processar(DadosAudio &audio) override;

        double get_ganho_esquerdo_db() const;
        double get_ganho_direito_db() const;

        std::string get_tipo() const override;
        std::string get_parametros() const override;
};

#endif