#include "gerenciador_arquivo_WAV.hpp"
#include <fstream>  //biblioteca responsável pela leitura e escrita de arquivos no disco.
#include <iostream>
#include <cstring>
#include <stdexcept>    //biblioteca que define as classes de exceções padrão

DadosAudio* GerenciadorArquivoWav::carregar(const std::string &caminho){
    //Tenta abrir o arquivo em modo binário
    std::ifstream arquivo(caminho, std::ios::binary);
    if(!arquivo.is_open()){    //tratamento de erro
        throw std::runtime_error("Erro: Nao foi possivel abrir o arquivo (caminho inexistente ou invalido): " + caminho);
    }


    //Leitura dos 44 bytes do cabewçalho
    CabecalhoWav cabecalho;
    arquivo.read(reinterpret_cast<char*>(&cabecalho),sizeof(CabecalhoWav));
    //Validação dos marcadores RIFF e WAVE(história 1)
    if (!cabecalho.e_valido()) {
    arquivo.close();
    throw std::invalid_argument("Erro: Formato de arquivo invalido! Marcadores RIFF/WAVE nao encontrados.");
    }
    
    //Cálculo do numero de amostras
    //dataSize é o total de bytes de áudio
    //cada amostra int16_t possui 2 bytes
    uint32_t num_amostras = cabecalho.get_subchunk2_size()/sizeof(int16_t);  //fórmula do numero de amostras

    //Alocação dinâmica do vetor de áudio na RAM (História 3)
    int16_t *amostras = new int16_t[num_amostras];

    //leitura dos dados brutos de áudio para a memória RAM
    arquivo.read(reinterpret_cast<char*> (amostras), cabecalho.get_subchunk2_size());
    arquivo.close();

    //Retorna uma nova instância encapsdulada da classe DadosAudio
    return new DadosAudio(cabecalho, amostras, num_amostras);
}
void GerenciadorArquivoWav::exportar(const std::string &caminho, const DadosAudio &dados){   //Cria/sobrescreve o arquivo em modo binário

    std::ofstream arquivo(caminho, std::ios::binary);

    
    if(!arquivo.is_open()){
        throw std::runtime_error("Erro: Nao foi possivel criar ou exportar o arquivo em: " + caminho);
    }

    //Escreve o cabeçalho de 44 bytes no artquivo de saída(história 2)
    const CabecalhoWav &cabecalho = dados.get_cabecalho();
    arquivo.write(reinterpret_cast<const char*>(&cabecalho), sizeof(CabecalhoWav));

    //escreve os dados brutos de áudio modificados
    const int16_t *amostras = dados.get_amostras();
    arquivo.write(reinterpret_cast<const char*>(amostras), cabecalho.get_subchunk2_size());

    arquivo.close();
} 