#include"filtro_distorcao.hpp"
#include"funcoes_auxiliares.hpp"
#include<stdexcept>

//construtor
FiltroDistorcao::FiltroDistorcao (double drive, double limite, double reducao_volume_saida)
    :drive(drive),limite(limite),reducao_volume_saida(reducao_volume_saida){
        if (drive < 1.0) {
            throw std::invalid_argument("FiltroDistorcao: Drive deve ser >= 1.0");
        }
        if (limite <= 0.0 || limite > 1.0) {
            throw std::invalid_argument("FiltroDistorcao: Limite deve estar no intervalo (0.0, 1.0]");
        }
        if (reducao_volume_saida < 0.0 || reducao_volume_saida > 1.0) {
            throw std::invalid_argument("FiltroDistorcao: Reducao de volume de saida deve estar entre 0.0 e 1.0");
        }
    }

//destrutor
FiltroDistorcao::~FiltroDistorcao(){

}

void FiltroDistorcao::processar(DadosAudio &audio){
    int16_t* amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

    if (amostras == nullptr)
        return;

    const double teto = limite * 32767.0; //define um teto pra não deixar o audio estourar

    for (uint32_t i = 0; i < total_amostras; ++i) {
        //Amplifica o som usando o drive 
        double sinal = static_cast<double>(amostras[i]) * drive;

        //Limitação (Clipping)
        if (sinal > teto) {
            sinal = teto;
        } else if (sinal < -teto) {
            sinal = -teto;
        }

        //reduz o volume para a saída e garante o intervalo do int16
        amostras[i] = clipping(sinal * reducao_volume_saida);
    }
}

std::string FiltroDistorcao::get_parametros() const {
    return "drive=" + std::to_string(drive) +
           ",limite=" + std::to_string(limite) +
           ",reducao=" + std::to_string(reducao_volume_saida);
}

std::string FiltroDistorcao::get_tipo() const {
    return "Distorcao";
}