#include "modo_headless.hpp"

#include <fstream>
#include <iostream>

ModoHeadless::ModoHeadless(const std::string &caminho_script)
    : caminho_script(caminho_script), sucessos(0), falhas(0) {
}

bool ModoHeadless::executar() {
    std::ifstream arquivo(caminho_script);
    if (!arquivo.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir o script: " << caminho_script << std::endl;
        return false;
    }

    // Le o script linha por linha.
    std::string linha;
    int num_linha = 0;
    while (std::getline(arquivo, linha)) {
        ++num_linha;
        interpretar_linha(linha, num_linha);
    }

    exibir_resumo();
    return falhas == 0;
}

bool ModoHeadless::interpretar_linha(const std::string &linha, int num_linha) {
    (void)num_linha;  // TODO: usar nas mensagens de erro ("linha 3: ...")

    // Ignora linhas vazias e comentarios (#).
    if (linha.empty() || linha[0] == '#') {
        return true;
    }

    // TODO: separar a primeira palavra (carregar / filtro / exportar)
    // TODO: carregar  -> GerenciadorArquivoWav::carregar(caminho)
    // TODO: filtro    -> criar_filtro(...) e adicionar na CadeiaFiltros
    // TODO: exportar  -> cadeia.processar(audio) e GerenciadorArquivoWav::exportar(...)
    // TODO: comando desconhecido -> reportar erro sem interromper o programa
    return true;
}

FiltroAudio *ModoHeadless::criar_filtro(const std::string &nome,
                                        const std::vector<double> &parametros) {
    (void)nome;
    (void)parametros;
    // TODO: devolver o filtro certo conforme o nome (ganho, distorcao, eco, ...)
    return nullptr;
}

void ModoHeadless::exibir_resumo() const {
    std::cout << "Resumo: " << sucessos << " sucesso(s), " << falhas << " falha(s)" << std::endl;
}
