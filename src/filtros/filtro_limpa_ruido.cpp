#include"filtro_limpa_ruido.hpp"
#include"funcoes_auxiliares.hpp"    
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

//calcula a passa alta
FiltroLimpaRuido::Biquad FiltroLimpaRuido::passa_alta(double fc, double fs) {
    double w0 = 2.0 * PI * fc / fs;
    double alfa = std::sin(w0) / (2.0 * Q_BUTTERWORTH);
    double cos_w0 = std::cos(w0);
    double a0 = 1.0 + alfa;
    return { (1.0 + cos_w0) / 2.0 / a0, -(1.0 + cos_w0) / a0, (1.0 + cos_w0) / 2.0 / a0,
             (-2.0 * cos_w0) / a0, (1.0 - alfa) / a0 };
}

//calcula passa baixa
FiltroLimpaRuido::Biquad FiltroLimpaRuido::passa_baixa(double fc, double fs) {
    double w0 = 2.0 * PI * fc / fs;
    double alfa = std::sin(w0) / (2.0 * Q_BUTTERWORTH);
    double cos_w0 = std::cos(w0);
    double a0 = 1.0 + alfa;
    return { (1.0 - cos_w0) / 2.0 / a0, (1.0 - cos_w0) / a0, (1.0 - cos_w0) / 2.0 / a0,
             (-2.0 * cos_w0) / a0, (1.0 - alfa) / a0 };
}


double FiltroLimpaRuido::aplicar(const Biquad &f, Estado &e, double x) {
    double y = f.b0 * x + f.b1 * e.x1 + f.b2 * e.x2 - f.a1 * e.y1 - f.a2 * e.y2;
    e.x2 = e.x1;
    e.x1 = x;
    e.y2 = e.y1;
    e.y1 = y;
    return y;
}

//construtor
FiltroLimpaRuido::FiltroLimpaRuido(double corte_grave, double corte_agudo):
    corte_grave(corte_grave), corte_agudo(corte_agudo){ //inicializa as variáveis multiplicadoras com valores predefinidos no hpp

        if (!(corte_grave> 0.0)) //aqui usa o diferente (!) pra identificar valores negativos e nao digitos
            throw std::invalid_argument("FiltroLimpaRuido: o corte de graves deve ser maior que 0 Hz");
        if (!(corte_agudo > corte_grave))
            throw std::invalid_argument("FiltroLimpaRuido: o corte de agudos deve ser maior que o corte de graves");
    } 

//destrutor
FiltroLimpaRuido::~FiltroLimpaRuido(){
}


double FiltroLimpaRuido::get_corte_grave() const { 
    return corte_grave; 
}
double FiltroLimpaRuido::get_corte_agudo() const { 
    return corte_agudo;
}

std::string FiltroLimpaRuido::get_parametros() const {
    return "corte_grave=" + std::to_string(corte_grave) +
           ",corte_agudo=" + std::to_string(corte_agudo);
}



//processar
void FiltroLimpaRuido::processar(DadosAudio &audio){
    int16_t *amostra_audio = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras();
    uint16_t canais = audio.get_num_canais();
    double fs = static_cast<double>(audio.get_sample_rate());

    if (amostra_audio == nullptr || total_amostras == 0 || canais == 0)
        return;
    if (canais > MAX_CANAIS)
        throw std::invalid_argument("FiltroLimpaRuido: numero de canais acima do suportado");
    if (corte_grave >= fs / 2.0)
        throw std::invalid_argument("FiltroLimpaRuido: corte de graves deve ser menor que metade da taxa de amostragem");

    //os cortes são em Hz, então os coeficientes dependem da taxa de amostragem do áudio
    Biquad alta = passa_alta(corte_grave, fs);
    //se o corte de agudos está acima de Nyquist, não há nada acima dele para remover: pula o passa-baixa
    bool usa_passa_baixa = corte_agudo < fs / 2.0;
    Biquad baixa = usa_passa_baixa ? passa_baixa(corte_agudo, fs) : Biquad{1.0, 0.0, 0.0, 0.0, 0.0};

    //memória separada para cada canal, para L e R não se misturarem
    Estado estado_alta[MAX_CANAIS];
    Estado estado_baixa[MAX_CANAIS];

    for (uint32_t i = 0; i < total_amostras; i++) {
        uint32_t c = i % canais; //canal da amostra atual
        double y = aplicar(alta, estado_alta[c], static_cast<double>(amostra_audio[i])); //corta graves
        if (usa_passa_baixa)
            y = aplicar(baixa, estado_baixa[c], y); //corta agudos
        amostra_audio[i] = clipping(y);
    }
}





