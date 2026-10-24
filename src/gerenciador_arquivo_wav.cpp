#include "gerenciador_arquivo_wav.hpp"
#include <fstream>  //biblioteca responsável pela leitura e escrita de arquivos no disco.
#include <iostream>
#include <cstring>
#include <stdexcept>    //biblioteca que define as classes de exceções padrão

DadosAudio* GerenciadorArquivoWav::carregar(const std::string &caminho){
    //Tenta abrir o arquivo em modo binário
    std::ifstream arquivo(caminho, std::ios::binary);
    if(!arquivo.is_open()){     //tratamento de erro
        throw std::runtime_error("Erro: Nao foi possivel abrir o arquivo (caminho inexistente ou invalido): " + caminho);
    }


    //Leitura dos 44 bytes do cabewçalho
    CabecalhoWav cabecalho;
    arquivo.read((char*)&cabecalho, sizeof(CabecalhoWav));
    //Validação dos marcadores RIFF e WAVE(história 1)
    if(std::memcmp(cabecalho.riffID, "RIFF", 4) != 0 || std::memcmp(cabecalho.waveID, "WAVE", 4) != 0){
        arquivo.close();
        throw std::invalid_argument("Erro: Formato de arquivo invalido! Marcadores RIFF/WAVE nao encontrados.");
    }