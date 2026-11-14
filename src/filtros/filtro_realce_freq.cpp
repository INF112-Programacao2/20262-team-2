#include "filtro_realce_freq.hpp"
#include "funcoes_auxiliares.hpp"
#include <cmath>
#include <cstdint>
#include <stdexcept>

//construtor
FiltroRealceFreq::FiltroRealceFreq(double frequencia_central, double ganho_db, double largura_faixa)
    : frequencia_central(frequencia_central), ganho_db(ganho_db), largura_faixa(largura_faixa) {

    if (!(frequencia_central > 0.0))
        throw std::invalid_argument("FiltroRealceFreq: frequencia central deve ser maior que 0 Hz");
    if (!(largura_faixa > 0.0))
        throw std::invalid_argument("FiltroRealceFreq: largura deve ser maior que 0");
    if (!(ganho_db >= -ganho_max_db && ganho_db <= ganho_max_db))
        throw std::invalid_argument("FiltroRealceFreq: ganho fora do intervalo permitido (-24 a +24 dB)");
}

//destrutor
FiltroRealceFreq::~FiltroRealceFreq() {
}

void FiltroRealceFreq::processar(DadosAudio &audio) {
    int16_t *amostras = audio.get_audio();
    uint32_t total_amostras = audio.get_num_amostras(); //total de amostras (canais intercalados)
    uint32_t canais = audio.get_num_canais();
    double fs = static_cast<double>(audio.get_sample_rate());

    if (amostras == nullptr || total_amostras == 0 || canais == 0)
        return;
    if (ganho_db == 0.0)
        return; //ganho zero = filtro transparente, nada a fazer
    if (canais > MAX_CANAIS) //protege os arrays de estado de tamanho fixo
        throw std::invalid_argument("FiltroRealceFreq: numero de canais acima do suportado");
    if (frequencia_central >= fs / 2.0) //acima de Nyquist o filtro não faz sentido
        throw std::invalid_argument("FiltroRealceFreq: frequencia central deve ser menor que metade da taxa de amostragem");

    //coeficientes do biquad "peaking" 
    const double PI = 3.14159265358979323846;
    double A = std::pow(10.0, ganho_db / 40.0);
    double w0 = 2.0 * PI * frequencia_central / fs;
    double alfa = std::sin(w0) / (2.0 * largura_faixa);
    double cos_w0 = std::cos(w0);

    double a0 = 1.0 + alfa / A;
    double b0 = (1.0 + alfa * A) / a0;
    double b1 = (-2.0 * cos_w0) / a0;
    double b2 = (1.0 - alfa * A) / a0;
    double a1 = (-2.0 * cos_w0) / a0;
    double a2 = (1.0 - alfa / A) / a0;

    //estado (memória do filtro) separado para cada canal, para L e R não se misturarem
    double x1[MAX_CANAIS] = {0.0}, x2[MAX_CANAIS] = {0.0};
    double y1[MAX_CANAIS] = {0.0}, y2[MAX_CANAIS] = {0.0};

    for (uint32_t i = 0; i < total_amostras; i++) {
        uint32_t c = i % canais; //canal da amostra atual
        double x = static_cast<double>(amostras[i]);

        double y = b0 * x + b1 * x1[c] + b2 * x2[c] - a1 * y1[c] - a2 * y2[c];

        x2[c] = x1[c];
        x1[c] = x;
        y2[c] = y1[c];
        y1[c] = y;

        amostras[i] = clipping(static_cast<float>(y)); //mantém dentro do intervalo do int16
    }
}