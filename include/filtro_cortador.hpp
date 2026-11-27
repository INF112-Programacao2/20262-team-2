#ifndef FILTRO_CORTADOR_HPP
#define FILTRO_CORTADOR_HPP
#include "filtro_audio.hpp"

//cortador de áudio: mantém apenas o trecho [inicio_ms, fim_ms) e descarta o resto
//diferente dos outros filtros, esse muda a duração do áudio (troca o vetor de amostras e atualiza o cabeçalho)
//o corte é feito em "frames" (uma amostra de cada canal), então nunca desalinha os canais do estéreo
class FiltroCortador : public FiltroAudio {
    private:
        double inicio_ms; //onde o trecho mantido começa, em ms: >= 0
        double fim_ms;    //onde o trecho mantido termina, em ms: > inicio_ms, ou ate_o_fim

    public:
        static constexpr double ate_o_fim = -1.0; //valor especial: mantém tudo até o final do áudio

        //lança std::invalid_argument se inicio_ms < 0 ou se fim_ms <= inicio_ms (exceto ate_o_fim)
        explicit FiltroCortador(double inicio_ms, double fim_ms = ate_o_fim);
        ~FiltroCortador() override = default;

        //lança std::out_of_range se o início estiver além da duração do áudio
        //se o fim passar da duração, o corte vai até o final do áudio
        void processar(DadosAudio &audio) override;

        double get_inicio_ms() const;
        double get_fim_ms() const;

        std::string get_tipo() const override;
        std::string get_parametros() const override;
};

#endif
