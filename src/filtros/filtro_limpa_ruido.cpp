#include"filtro_limpa_ruido.hpp"
#include"funcoes_auxiliares.hpp"    
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace {
    const double PI = 3.14159265358979323846;
    const double Q_BUTTERWORTH = 0.70710678118654752440; //1/raiz(2): resposta mais plana possível na banda de passagem

    //coeficientes normalizados de um filtro biquad
    struct Biquad {
        double b0, b1, b2, a1, a2;
    };

//construtor
FiltroLimpaRuido::FiltroLimpaRuido():
    corte_grave(0.028), corte_agudo(0.32){ //inicializa as variáveis multiplicadoras com valores predefinidos
        if (corte_grave < 0.0 || corte_agudo < 0.0) {
        throw std::invalid_argument("Frequencias de corte devem ser positivas.");
        }
    } 

//destrutor
FiltroLimpaRuido::~FiltroLimpaRuido(){
    
}

//processar
void FiltroLimpaRuido::processar(DadosAudio &audio){
    int16_t *amostra_audio = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();
    uint16_t canais = audio.get_num_channels();
    uint32_t sample_rate = audio.get_sample_rate();

    double graves_acumulados = 0; //guarda o sons de baixa frequência
    double audio_limpo = 0; //recebe o som já sem os graves

    for(uint32_t i= 0; i<total_amostras;i++){
        double audio_inicial = static_cast<float>(amostra_audio[i]); //copia o áudio atual e converte pra double

        graves_acumulados += corte_grave * (audio_inicial - graves_acumulados); //soma e guarda o audio removendo os graves
        float sem_grave = audio_inicial - graves_acumulados;

        audio_limpo += corte_agudo * (sem_grave - audio_limpo); // remove alta frequência, agudos

        amostra_audio[i] = clipping(audio_limpo);//aplica o clipping

    }








}


