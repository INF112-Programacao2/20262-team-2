#include"filtro_limpa_ruido.hpp"
#include"funcoes_auxiliares.hpp"    
#include<iostream>
#include<cstdint>


//construtor
FiltroLimpaRuido::FiltroLimpaRuido():
    corte_grave(0.028), corte_agudo(0.32){} //inicializa as variáveis multiplicadoras com valores predefinidos

//destrutor
FiltroLimpaRuido::~FiltroLimpaRuido(){
    
}

//processar
void FiltroLimpaRuido::processar(DadosAudio &audio){
    int16_t *amostra_audio = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

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


