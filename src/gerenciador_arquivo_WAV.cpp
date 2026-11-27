#include "gerenciador_arquivo_WAV.hpp"
#include "excecoes_WAV.hpp"
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

// Obtém o tamanho do arquivo e trata possíveis erros ao acessar essa informação.
    uint64_t tamanho_no_disco = fs::file_size(origem, erro);
    if(erro){
        throw ErroAcessoArquivo("nao foi possivel obter o tamanho do arquivo: " + caminho);
    }


    //Leitura do cabeçalho RIFF (12 bytes): "RIFF" + tamanho + "WAVE" (história 1)
    unsigned char riff[12];
    arquivo.read(reinterpret_cast<char*>(riff), sizeof(riff));
    if(arquivo.gcount() != static_cast<std::streamsize>(sizeof(riff))){
        throw FormatoWavInvalido("o arquivo tem menos de 12 bytes, nao ha cabecalho WAV completo.");
    }
    if(std::memcmp(riff, "RIFF", 4) != 0 || std::memcmp(riff + 8, "WAVE", 4) != 0){
        throw FormatoWavInvalido("marcadores RIFF/WAVE nao encontrados.");
    }


    // Um .wav é uma sequencia de "chunks": [id (4 bytes)][tamanho (4 bytes)][conteudo]
    // Não posso dizer que"data" comeca no byte 36: pq tem programas como o Audacity que colocam
    // chunks extras (LIST, id3, fact...) antes dele, fazendo o fmt pode ter mais de 16 bytes.

    // Logo, vamos percorrer os chunks, guardar o "fmt" e pular tudo que nao interessa até achar o "data"
    bool achou_fmt = false;
    uint16_t formato = 0, canais = 0, block_align = 0, bits = 0;
    uint32_t taxa = 0;
    uint32_t bytes_audio = 0;
    uint64_t inicio_dados = 0;
    bool achou_data = false;
    uint64_t posicao = 12;   //posicao (no arquivo) do proximo chunk

// Repetição que vai ser responsável por percorrer os chunks do arquivo WAV até encontrar o chunk data
 while(posicao + 8 <= tamanho_no_disco){
        unsigned char cab[8];
        arquivo.seekg(static_cast<std::streamoff>(posicao));
        arquivo.read(reinterpret_cast<char*>(cab), sizeof(cab));
        if(arquivo.gcount() != static_cast<std::streamsize>(sizeof(cab))){
            break;
        }
        uint32_t tam_chunk = ler_u32(cab + 4);
        uint64_t inicio_conteudo = posicao + 8;

        if(std::memcmp(cab, "fmt ", 4) == 0){
            if(tam_chunk < 16){
                throw FormatoWavInvalido("o chunk fmt e menor que 16 bytes.");
            }
            if(inicio_conteudo + tam_chunk > tamanho_no_disco){
                throw FormatoWavInvalido("o chunk fmt esta truncado (arquivo incompleto ou corrompido).");
            }

            //Só os primeiros 40 bytes importam (16 do PCM + extensao WAVE_FORMAT_EXTENSIBLE)
            unsigned char fmt[40] = {0};
            std::streamsize a_ler = static_cast<std::streamsize>(tam_chunk < sizeof(fmt) ? tam_chunk : sizeof(fmt));
            arquivo.read(reinterpret_cast<char*>(fmt), a_ler);
            if(arquivo.gcount() != a_ler){
                throw ErroAcessoArquivo("falha ao ler o chunk fmt de: " + caminho);
            }
            formato     = ler_u16(fmt);
            canais      = ler_u16(fmt + 2);
            taxa        = ler_u32(fmt + 4);
            block_align = ler_u16(fmt + 12);
            bits        = ler_u16(fmt + 14);
            //WAVE_FORMAT_EXTENSIBLE (0xFFFE): o codigo real (1 = PCM) ficva nos 2 primeiros
            //bytes do SubFormat, no offset 24 do chunk
            if(formato == 0xFFFE && tam_chunk >= 40){
                formato = ler_u16(fmt + 24);
            }
            achou_fmt = true;
        }

        else if(std::memcmp(cab, "data", 4) == 0){
            if(!achou_fmt){
                throw FormatoWavInvalido("o chunk data aparece antes do chunk fmt.");
            }
            bytes_audio = tam_chunk;
            inicio_dados = inicio_conteudo;
            achou_data = true;
            break;
        }
        //qualquer outro chunk (LIST, id3, fact, bext, C2PA...) vai sert ignorado

        //Chunks tem tamanho par: se o tamanho for impar existe 1 byte de preencvhimento depois
        posicao = inicio_conteudo + tam_chunk + (tam_chunk & 1u);
    }

    if(!achou_fmt){
        throw FormatoWavInvalido("chunk fmt nao encontrado.");
    }

    if(!achou_data){
        throw FormatoWavInvalido("chunk data nao encontrado.");
    }

    //DadosAudio guarda int16_t: só PCM de 16 bits
    if(formato != 1 || bits != 16 || canais < 1 || block_align != canais * (bits / 8)){
        throw FormatoWavInvalido("somente WAV PCM de 16 bits e suportado.");
    }

    if(taxa == 0){
        throw FormatoWavInvalido("a taxa de amostragem do arquivo e 0 Hz.");
    }

    //Monta o cabeçalho "normal" de 44 bytes (é ele que o exportar grava) com o que foi lido
    CabecalhoWav cabecalho;
    cabecalho.configurar_formato(formato, canais, taxa, bits);
    if(!cabecalho.e_valido() || !cabecalho.e_suportado()){
        throw FormatoWavInvalido("somente WAV PCM de 16 bits e suportado.");
    }

    //O tamanho dos dados informado no cabeçalho não pode passar do que existe no arquivo.
    //também impede alocar uma quantidade absurda de memória por causa de um cabeçalho corrompido
    if(bytes_audio > tamanho_no_disco - inicio_dados){
        throw FormatoWavInvalido("o cabecalho informa mais dados do que o arquivo possui (arquivo truncado ou corrompido).");
    }
    cabecalho.set_subchunk2_size(bytes_audio);



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

    //leitura dos dados brutos de áudio para a memória RAM (a partir do inicio do chunk data)
    arquivo.clear();
    arquivo.seekg(static_cast<std::streamoff>(inicio_dados));
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

    //Integridade: não vaigravar um arquivo que o próprio programa não conseguiria ler de volta
    if(!cabecalho.e_valido() || !cabecalho.e_suportado()){
        throw ExcecaoWav("o cabecalho do audio e invalido (canais ou taxa de amostragem incorretos); nada foi gravado.");
    }
    if(dados.get_num_amostras() % cabecalho.get_num_de_canais() != 0){
        throw ExcecaoWav("o audio termina no meio de um frame (amostras nao sao multiplo do numero de canais); nada foi gravado.");
    }

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

        //Integridade: a posição de escrita (tellp) tem que ser 44 bytes de cabeçalho + os dados
        std::streamoff escritos = saida.tellp();
        gravou = saida.good() && escritos == static_cast<std::streamoff>(TAMANHO_CABECALHO_WAV + bytes_audio);

    }   //  **** o arquivo temporário é fechado aqui  **** 

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