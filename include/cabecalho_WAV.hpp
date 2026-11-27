#ifndef CABECALHO_HPP
#define CABECALHO_HPP

#include <cstddef> //Pra std::size_t (usado para representar tamanhos em bytes)
#include <cstdint> //Garante inteiros com tamanho exatro em bytes (uint16_t, uint32_t)


//Tamanho, em bytes, de um cabeçalho "normal" de um .wav PCM simples (e o que o exportar vaigravar).
//Ao ler um arquivo, não é assumido que o arquivo tenha necessariamente 44bytes.
//O carregar percorre os chunks, pois o arquivo de entrada pode ter chunks extras (LIST, id3...) antes do "data".
constexpr std::size_t TAMANHO_CABECALHO_WAV = 44;


// CabecalhoWav: representa os metadados fixos (44 bytes) de um arquivo .wav
// Os campos ficam na MESMA ordem do arquivo (pack(1), sem espaços entre
// eles), por isso o objeto inteiro pode ser lido/gravado com read()/write().
#pragma pack(push, 1)
class CabecalhoWav{
    private:

    char riffID[4];            // Bytes 0-3: Guarda os caracteres ASCII "RIFF", identificador do tipo de arquivo
    uint32_t tamanho_arquivo;  // Bytes 4-7: (Tamanho do arquivo em bytes) - (8), (36 + dataSize)
    char waveID[4];            // Bytes 8-11: Guarda os caracteres ASCII "WAVE", confirma que o formato do áudio dentro do bloco RIFF é um ficheiro WAV
    char fmtID[4];             // Bytes 12-15: Guarda os caracteres ASCII "fmt ", marca o início do sub-bloco de formatação técnica do áudio
    uint32_t fmtSize;          // Bytes 16-19: Define o tamanho em bytes das informações de formato
    uint16_t formato_audio;    // Bytes 20-21: Indica o código do tipo de compressão/codificação do áudio
    uint16_t num_de_canais;    // Bytes 22-23: Guarda o número de canais de áudio. 1 para Mono ou 2 para Estéreo
    uint32_t sampleRate;       // Bytes 24-27: Taxa/frequência de amostragem em Hertz -> quantas leituras do sinal de áudio são feitas a cada segundo
    uint32_t byteRate;         // Bytes 28-31: Indica a quantidade de bytes de áudio processados por segundo
    uint16_t blockAlign;       // Bytes 32-33: Alinhamento de bloco: quantidade de bytes pra guardar exatamente 1 frame de áudio (considerando todos os canais)
    uint16_t bitsPerSample;    // Bytes 34-35: Profundidade de bits: quantidade de bits de informação usada pra representar cada amostra individual de áudio. Define a resolução vertical. Mais bits = maior precisão da qualidade do som.
    char dataID[4];            // Bytes 36-39: Guarda os caracteres ASCII "data", indica que a ficha técnica terminou e que o bloco de áudio real começa a seguir
    uint32_t dataSize;         // Bytes 40-43: Tamanho total do som/das amostras em bytes

    //Função que recalcula os campos dependentes para manter o cabeçalho sempre coerente
        void recalcular_derivados();

    public:
        CabecalhoWav();
        ~CabecalhoWav();

       
    //Validação dos marcadores (de História 1)
    bool e_valido() const;      //confere "RIFF", "WAVE", "fmt " e "data" e o fmtSize == 16.
    bool e_suportado() const;   //confere se é PCM de 16 bits com blockAlign coerente


    // Getters -- somente leitura, ninguem de fora altera um cabecalho ja lido
    uint16_t get_num_de_canais() const;
    uint32_t get_sample_rate() const;
    uint16_t get_bits_per_sample() const;
    uint32_t get_subchunk2_size() const;
    uint16_t get_audio_format() const;
    uint16_t get_block_align() const;
    uint32_t get_byte_rate() const;
    uint32_t get_tamanho_arquivo() const;

    // Setters
    void set_num_de_canais(uint16_t canais);
    void set_sample_rate(uint32_t sr);
    void set_subchunk2_size(uint32_t size);


    // Preenche os campos do chunk fmt lidos de um arquivo e recvalcula byteRate/blockAlign
    void configurar_formato(uint16_t formato, uint16_t canais, uint32_t sr, uint16_t bits);

};
#pragma pack(pop)

//Garante que o objeto tem exatamente 44 bytes
static_assert(sizeof(CabecalhoWav) == TAMANHO_CABECALHO_WAV,
              "O CabecalhoWav deve ter exatamente 44 bytes");


#endif
