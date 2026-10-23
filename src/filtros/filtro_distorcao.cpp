#include"filtro_distorcao.hpp"
#include"funcoes_auxiliares.hpp"

//construtor
FiltroDistorcao::FiltroDistorcao (double drive, double limite, double reducao_volume_saida)
    :drive(drive),limite(limite),reducao_volume_saida(reducao_volume_saida){
        if (this->drive < 1.0) //impede que o empurrão seja menor que 1
            this->drive = 1.0;
    }

//destrutor
FiltroDistorcao::~FiltroDistorcao(){

}

void FiltroDistorcao::processar(DadosAudio &audio){
    int16_t* amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

    for (uint32_t i = 0; i < total_amostras; ++i) {
        //Amplifica o som usando o drive 
        float sinal = static_cast<float>(amostras[i]) * drive;

        //Limitação (Clipping)
        if (sinal > limite) {
            sinal = limite;
        } else if (sinal < -limite) {
            sinal = -limite;
        }

        //reduz a altura do volume para a saída
        sinal *= reducao_volume_saida;

        //Salva com o clipping padrão
        amostras[i] = clipping(sinal);
    }
}
