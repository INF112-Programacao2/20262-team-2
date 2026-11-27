#include "gerenciador_arquivo_WAV.hpp"
#include "excecoes_wav.hpp"
#include <cctype>
#include <cstring>
#include <cstdint>
#include <filesystem>   // C++17: verificar existência/pasta e trocar o arquivo com segurança
#include <fstream>  //biblioteca responsável pela leitura e escrita de arquivos no disco.
#include <limits>
#include <new>
#include <iostream>
#include <string>
#include <utility>       //necessário para utilizar a função std::move.

namespace fs = std::filesystem;

GerenciadorArquivoWav::~GerenciadorArquivoWav(){
}

// Confere se o caminho termina em ".wav" (maiúsculas ou minúsculas)
static bool tem_extensao_wav(const fs::path &caminho){
    std::string extensao = caminho.extension().string();
    for(char &c : extensao){
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return extensao == ".wav";
}


// Lê um inteiro de 32 bits little-endian (formato dos campos de um .wav) a partir de 4 bytes
// Ou seja: recebe 4 bytes e monta um inteiro de 32 bits.
static uint32_t ler_u32(const unsigned char *b){
    return static_cast<uint32_t>(b[0]) | (static_cast<uint32_t>(b[1]) << 8) |
           (static_cast<uint32_t>(b[2]) << 16) | (static_cast<uint32_t>(b[3]) << 24);
}

// Lê um inteiro de 16 bits little-endian a partir de 2 bytes
// Ou seja:recebe 2 bytes e monta um inteiro de 16 bits.
static uint16_t ler_u16(const unsigned char *b){
    return static_cast<uint16_t>(b[0] | (b[1] << 8));
}



std::unique_ptr<DadosAudio> GerenciadorArquivoWav::carregar(const std::string &caminho){
    if(caminho.empty()){
        throw CaminhoInvalido("o caminho do arquivo de entrada esta vazio.");
    }

     //Arquivo inexistente (ou que é uma pasta): mensagem de erro (história 1)
    std::error_code erro;
    fs::path origem(caminho);
    if(!fs::is_regular_file(origem, erro)){
        throw ArquivoNaoEncontrado(caminho);
    }

    //Tenta abrir o arquivo em modo binário
    std::ifstream arquivo(origem, std::ios::binary);
    if(!arquivo.is_open()){    //tratamento de erro
        throw ErroAcessoArquivo("nao foi possivel abrir o arquivo (verifique as permissoes): " + caminho);
    }


    uint64_t tamanho_no_disco = fs::file_size(origem, erro);
    if(erro){
        throw ErroAcessoArquivo("nao foi possivel obter o tamanho do arquivo: " + caminho);
    }


    //Leitura do cabeçalho RIFF (12 bytes): "RIFF" + tamanho + "WAVE" (história 1)
    unsigned char riff[12];
    arquivo.read(reinterpret_cast<char*>(riff), sizeof(riff));
    if(arquivo.gcounmt() != static_cast<std::streamsize>(sizeof(riff))){
        throw FormatoWavInvalido("o arquivo tem menos de 12 bytes, nao ha cabecalho WAV completo.");
    }

    if(std::memcmp(riff, "RIFF", 4) != 0 || std::memcmp(riff + 8, "WAVE", 4) != 0){
        throw FormatoWavInvalido("marcadores RIFF/WAVE nao encontrados.");
    }


    //O tamanho dos dados informado no cabeçalho não pode passar do que existe no arquivo.
    //também impede alocar uma quantidade absurda de memória por causa de um cabeçalho corrompido
    uint64_t tamanho_no_disco = fs::file_size(origem, erro);
    if(erro){
        throw ErroAcessoArquivo("nao foi possivel obter o tamanho do arquivo: " + caminho);
    }
    uint32_t bytes_audio = cabecalho.get_subchunk2_size();
    if(bytes_audio > tamanho_no_disco - TAMANHO_CABECALHO_WAV){
        throw FormatoWavInvalido("o cabecalho informa mais dados do que o arquivo possui (arquivo truncado ou corrompido).");
    }





    //Só aceita quantidade de bytes igual a um número inteiro de frames (1 amostra de cada canal)
    uint32_t bytes_por_frame = cabecalho.get_num_de_canais() * sizeof(int16_t);
    if(bytes_audio % bytes_por_frame != 0){
        throw FormatoWavInvalido("o tamanho dos dados nao e multiplo do tamanho de um frame de audio.");
    }
    
    //Cálculo do numero de amostras
    //dataSize é o total de bytes de áudio
    //cada amostra int16_t possui 2 bytes
    uint32_t num_amostras = bytes_audio / sizeof(int16_t);

    //Alocação dinâmica, só do tamanho necessário (História 3). O unique_ptr garante a liberação
    //automática, mesmo que a leitura abaixo lance uma exceção.
    std::unique_ptr<int16_t[]> amostras;
    try{
        amostras = std::make_unique<int16_t[]>(num_amostras);
    }
    catch(const std::bad_alloc&){
        throw ExcecaoWav("memoria insuficiente para carregar o audio.");
    }

    //leitura dos dados brutos de áudio para a memória RAM
    arquivo.read(reinterpret_cast<char*>(amostras.get()), bytes_audio);
    if(arquivo.gcount() != static_cast<std::streamsize>(bytes_audio)){
        throw ErroAcessoArquivo("falha ao ler os dados de audio de: " + caminho);
    }

    //Retorna uma nova instância encapsdulada da classe DadosAudio
    // A função std::move irá transferir a propriedade do vetor de amostras para o objeto DadosAudio
    return std::make_unique<DadosAudio>(cabecalho, std::move(amostras), num_amostras);
}

void GerenciadorArquivoWav::exportar(const std::string &caminho, const DadosAudio &dados, bool sobrescrever){   //Cria/sobrescreve o arquivo em modo binário

     // Validação do nome e do caminho (história 2)
    if(caminho.empty()){
        throw CaminhoInvalido("o caminho esta vazio.");
    }
    fs::path destino(caminho);
    if(!destino.has_filename()){
        throw CaminhoInvalido("falta o nome do arquivo em: " + caminho);
    }
    if(!tem_extensao_wav(destino)){
        throw CaminhoInvalido("o nome do arquivo deve terminar em .wav: " + caminho);
    }

    std::error_code erro;
    fs::path pasta = destino.has_parent_path() ? destino.parent_path() : fs::path(".");
    if(!fs::is_directory(pasta, erro)){
        throw CaminhoInvalido("a pasta de destino nao existe: " + pasta.string());
    }
    if(fs::is_directory(destino, erro)){
        throw CaminhoInvalido("o destino e uma pasta, nao um arquivo: " + caminho);
    }
    if(fs::exists(destino, erro) && !sobrescrever){
        throw ArquivoJaExiste(caminho);
    }

    // Cabeçalho atualizado de acordo com o áudio final (história 2)
    //Cópia do cabeçalho: o tamanho dos dados é recalculado a partir do número de amostras atual
    uint64_t bytes_audio = static_cast<uint64_t>(dados.get_num_amostras()) * sizeof(int16_t);
    if(bytes_audio > std::numeric_limits<uint32_t>::max() - 36){
        throw ExcecaoWav("o audio e grande demais para o formato WAV (limite de 4 GB).");
    }
    CabecalhoWav cabecalho = dados.get_cabecalho();
    cabecalho.set_subchunk2_size(static_cast<uint32_t>(bytes_audio));   //também recalcula o tamanho do arquivo

    // Gravação segura: primeiro num arquivo temporário
    //Se algo falhar no meio da gravação, o arquivo original (se existir) continua intacto.
    fs::path temporario = destino;
    temporario += ".tmp";

    bool gravou = false;

    {
        std::ofstream saida(temporario, std::ios::binary | std::ios::trunc);
        if(!saida.is_open()){
            throw ErroAcessoArquivo("nao foi possivel criar o arquivo de saida (verifique as permissoes): " + caminho);
        }

        //Escreve o cabeçalho de 44 bytes no arquivo de saída(história 2)
        saida.write(reinterpret_cast<const char*>(&cabecalho), TAMANHO_CABECALHO_WAV);

        //escreve os dados brutos de áudio modificados
        saida.write(reinterpret_cast<const char*>(dados.get_amostras()), static_cast<std::streamsize>(bytes_audio));

        saida.flush();
        gravou = saida.good();
    }   //o arquivo temporário é fechado aqui

    if(!gravou){
        fs::remove(temporario, erro);
        throw ErroAcessoArquivo("falha ao gravar o arquivo (disco cheio ou sem permissao?): " + caminho);
    }

    //Troca o temporário pelo arquivo final (substitui o original, se houver)
    fs::rename(temporario, destino, erro);
    if(erro){
        std::error_code ignorado;
        fs::remove(temporario, ignorado);
        throw ErroAcessoArquivo("nao foi possivel finalizar a gravacao em " + caminho + ": " + erro.message());
    }
} 