#ifndef FILTRO_REALCE_FREQ_HPP
#define FILTRO_REALCE_FREQ_HPP
#include"filtro_audio.hpp"

//filtro de realce (ou atenuação) de uma faixa de frequências em torno de uma frequência central
//útil para compensar a perda auditiva do usuário em determinada faixa

class FiltroRealceFreq : public FiltroAudio {
    private:
        double frequencia_central; //frequência (Hz) onde o ganho é máximo
        double ganho_db;           //ganho em dB na frequência central (positivo realça, negativo atenua)
        double largura_faixa;                  //largura da faixa: quanto maior, mais estreita a faixa afetada
       
     public:
        static const double largura_faixa_padrao = 1.0;
        static const double ganho_max_db = 24.0; //limite de segurança para não estourar o áudio
        static const uint32_t MAX_CANAIS = 8;

         //lança std::invalid_argument se algum parâmetro for inválido
        explicit FiltroRealceFreq(double frequencia_central, double ganho_db, double largura_faixa = largura_faixa_padrao);
        ~FiltroRealceFreq() override;

        void processar(DadosAudio &audio) override;

        double get_frequencia_central() const; 
        double get_ganho_db() const;
        double get_largura_faixa() const;

};



#endif 