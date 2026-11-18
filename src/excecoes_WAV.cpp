#include "excecoes_WAV.hpp"


ExcecaoWav::ExcecaoWav(const std::string& mensagem)
    : std::runtime_error(mensagem) {}

ExcecaoWav::~ExcecaoWav() noexcept {}

ArquivoNaoEncontrado::ArquivoNaoEncontrado(const std::string& caminho)
    : ExcecaoWav("Erro: arquivo nao encontrado: " + caminho) {}

ArquivoNaoEncontrado::~ArquivoNaoEncontrado() noexcept {}

FormatoWavInvalido::FormatoWavInvalido(const std::string& motivo)
    : ExcecaoWav("Erro: formato de arquivo invalido: " + motivo) {}

FormatoWavInvalido::~FormatoWavInvalido() noexcept {}

CaminhoInvalido::CaminhoInvalido(const std::string& motivo)
    : ExcecaoWav("Erro: caminho invalido: " + motivo) {}

CaminhoInvalido::~CaminhoInvalido() noexcept {}

ArquivoJaExiste::ArquivoJaExiste(const std::string& caminho)
    : ExcecaoWav("Erro: o arquivo ja existe (use a opcao de sobrescrever): " + caminho) {}

ArquivoJaExiste::~ArquivoJaExiste() noexcept {}

ErroAcessoArquivo::ErroAcessoArquivo(const std::string& mensagem)
    : ExcecaoWav("Erro de acesso ao arquivo: " + mensagem) {}

ErroAcessoArquivo::~ErroAcessoArquivo() noexcept {}


