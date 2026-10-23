#ifndef GERENCIADOR_ARQUIVO_WAV_HPP
#define GERENCIADOR_ARQUIVO_WAV_HPP


#include<string>
#include "dados_audio.hpp"

class GerenciadorArquivoWav{
public:
    //Leitura do arquivo WAV (História 1)
    static DadosAudio* carregar(const std::string &caminho);

    //Gravação do arquivo WAV (História 2)
    static void exportar(const std::string &caminho, const DadosAudio &dados);
};
#endif