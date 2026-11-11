#include"filtro_distorcao.hpp"
#include"funcoes_auxiliares.hpp"
#include<stdexcept>

//construtor
FiltroDistorcao::FiltroDistorcao (double drive, double limite, double reducao_volume_saida)
    :drive(drive),limite(limite),reducao_volume_saida(reducao_volume_saida){
        if (drive < 1.0) {
            throw std::invalid_argument("Drive deve ser >= 1.0");
        }
        if (limite <= 0.0 || limite > 1.0) {
            throw std::invalid_argument("Limite deve estar no intervalo (0.0, 1.0]");
        }
        if (reducao_volume_saida < 0.0 || reducao_volume_saida > 1.0) {
            throw std::invalid_argument("Reducao de volume de saida deve estar entre 0.0 e 1.0");
        }
    }

//destrutor
FiltroDistorcao::~FiltroDistorcao(){

}

void FiltroDistorcao::processar(DadosAudio &audio){
    int16_t* amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();

    const double teto = limite * 32767.0; //define um teto pra não deixar o audio estourar

    for (uint32_t i = 0; i < total_amostras; ++i) {
        //Amplifica o som usando o drive 
        float sinal = static_cast<double>(amostras[i]) * drive;

        //Limitação (Clipping)
        if (sinal > teto) {
            sinal = teto;
        } else if (sinal < -teto) {
            sinal = -teto;
        }

        //reduz a altura do volume para a saída
        sinal *= reducao_volume_saida;

        //Salva com o clipping padrão
        sinal = clipping(sinal);
        amostras[i] = static_cast<int16_t>(sinal);
    }
}

std::string FiltroDistorcao::get_parametros() const {
    return "drive=" + std::to_string(drive) +
           ",limite=" + std::to_string(limite) +
           ",reducao=" + std::to_string(reducao_volume_saida);
}
