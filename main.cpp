//main de teste: carrega um WAV, aplica uma cadeia de filtros e exporta o resultado
//uso: ./processador_audio [entrada.wav] [saida.wav]
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include "cadeia_filtros.hpp"
#include "filtro_distorcao.hpp"
#include "filtro_limpa_ruido.hpp"
#include "filtro_realce_freq.hpp"
#include "filtro_volume.hpp"
#include "gerenciador_arquivo_WAV.hpp"

int main(int argc, char *argv[]) {
    std::string entrada = (argc > 1) ? argv[1] : "data/03_voz_com_ruido.wav";
    std::string saida = (argc > 2) ? argv[2] : "data/saida/ruidoprocessado.wav";

    try {
        std::cout << "Carregando: " << entrada << std::endl;
        std::unique_ptr<DadosAudio> audio = GerenciadorArquivoWav::carregar(entrada);
        std::cout << "  canais=" << audio->get_num_canais()
                  << "  taxa=" << audio->get_sample_rate() << " Hz"
                  << "  amostras=" << audio->get_num_amostras() << std::endl;

        CadeiaFiltros cadeia;
        cadeia.adicionar_filtro(std::unique_ptr<FiltroAudio>(new FiltroLimpaRuido()));
        cadeia.adicionar_filtro(std::unique_ptr<FiltroAudio>(new FiltroRealceFreq(2500.0, 6.0)));
        cadeia.adicionar_filtro(std::unique_ptr<FiltroAudio>(new FiltroVolume(1.5)));

        std::cout << "Aplicando " << cadeia.get_quantidade() << " filtros:" << std::endl;
        for (uint32_t i = 0; i < cadeia.get_quantidade(); i++)
            std::cout << "  " << (i + 1) << ". " << cadeia.get_filtro(i)->get_tipo()
                      << " (" << cadeia.get_filtro(i)->get_parametros() << ")" << std::endl;
        cadeia.processar(*audio);

        std::filesystem::path pasta = std::filesystem::path(saida).parent_path();
        if (!pasta.empty())
            std::filesystem::create_directories(pasta); //cria a pasta de saída se não existir

        GerenciadorArquivoWav::exportar(saida, *audio);
        std::cout << "Exportado: " << saida << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "ERRO: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}