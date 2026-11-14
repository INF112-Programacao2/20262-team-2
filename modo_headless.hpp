#ifndef MODO_HEADLESS_HPP
#define MODO_HEADLESS_HPP

#include <string>
#include <vector>

class FiltroAudio;  // TODO: trocar por #include "filtro_audio.hpp" quando a classe existir

// Modo headless (processamento em lote): le um script .txt com comandos de
// carregamento, aplicacao de filtros e exportacao, e executa tudo em sequencia.
//
// Formato do script (esboco, a sintaxe final ainda sera definida):
//   carregar entrada/aula01.wav
//   filtro ganho 150
//   exportar saida/aula01_tratada.wav
class ModoHeadless {
public:
    ModoHeadless(const std::string &caminho_script);

    // Executa o script. Retorna true se nenhum arquivo falhou.
    bool executar();

private:
    std::string caminho_script;
    int sucessos;
    int falhas;

    // Interpreta uma linha do script. Retorna false se a linha for invalida.
    bool interpretar_linha(const std::string &linha, int num_linha);

    // Cria o filtro a partir do nome e dos parametros lidos do script.
    FiltroAudio *criar_filtro(const std::string &nome, const std::vector<double> &parametros);

    void exibir_resumo() const;
};

#endif
