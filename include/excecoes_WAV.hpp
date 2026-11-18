#ifndef EXCECOES_WAV_HPP
#define EXCECOES_WAV_HPP

#include <stdexcept>
#include <string>

//Hierarquia de exceções do módulo de leitura/gravação de arquivos .wav

// Todas classes herdam de ExcecaoWav (que herda de std::runtime_error), entrão quem
// chama pode tratar qualquer erro de leitura/gravação com um único catch (const ExcecaoWav&) ou pode
// distinguir cada caso capturando a exceção específica (ex.: ArquivoJaExiste)

//Base de todas as exceções do módulo WAV
class ExcecaoWav : public std::runtime_error {
public:
    explicit ExcecaoWav(const std::string& mensagem);
    ~ExcecaoWav() noexcept override;
};

//------------------------------------//

// História 1: tentativa de importar um arquivo que não existe
class ArquivoNaoEncontrado : public ExcecaoWav {
public:
    explicit ArquivoNaoEncontrado(const std::string& caminho);
    ~ArquivoNaoEncontrado() noexcept override;
};

// História 1: arquivo existe, mas não é um WAV válido/suportado
//(marcadores RIFF/WAVE ausentes, arquivo incompleto/corrompido, formato não PCM 16 bits...)
class FormatoWavInvalido : public ExcecaoWav {
public:
    explicit FormatoWavInvalido(const std::string& motivo);
    ~FormatoWavInvalido() noexcept override;
};

//------------------------------------//

// História 2: nome e/ou caminho de saída inválidos
// (vazio, sem nome de arquivo, sem extensão .wav, pasta inexistente...)
class CaminhoInvalido : public ExcecaoWav {
public:
    explicit CaminhoInvalido(const std::string& motivo);
    ~CaminhoInvalido() noexcept override;
};

// História 2: o arquivo de saída já existe e o usuário não pediu para sobrescrever
class ArquivoJaExiste : public ExcecaoWav {
public:
    explicit ArquivoJaExiste(const std::string& caminho);
    ~ArquivoJaExiste() noexcept override;
};

//------------------------------------//

// Falha de permissão, de abertura ou de leitrura/gravação (ex.: disco cheio)
class ErroAcessoArquivo : public ExcecaoWav {
public:
    explicit ErroAcessoArquivo(const std::string& mensagem);
    ~ErroAcessoArquivo() noexcept override;
};


#endif

