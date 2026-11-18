#ifndef GERENCIADOR_ARQUIVO_WAV_HPP
#define GERENCIADOR_ARQUIVO_WAV_HPP

#include <string>
#include <memory>
#include "dados_audio.hpp"

// Leitura e gravação de arquivos .wav. Todos os erros são lançados como
// exceções da hierarquia ExcecaoWav (veja excecoes_wav.hpp)
class GerenciadorArquivoWav{
public:
    ~GerenciadorArquivoWav();
    
    //Leitura do arquivo WAV (História 1)
    static std::unique_ptr<DadosAudio> carregar(const std::string &caminho);

    //Gravação do arquivo WAV (História 2)
    static void exportar(const std::string &caminho, const DadosAudio &dados, bool sobrescrever = false);
};
#endif