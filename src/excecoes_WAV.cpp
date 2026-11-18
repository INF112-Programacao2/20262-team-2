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








