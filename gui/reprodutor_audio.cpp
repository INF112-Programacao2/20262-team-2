#include "reprodutor_audio.hpp"
#include "dados_audio.hpp"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QBuffer>
#include <QMediaDevices>
#include <QTimer>

#include <algorithm>
#include <cstdint>
#include <utility>

ReprodutorAudio::ReprodutorAudio(QObject *parent) : QObject(parent) {}
ReprodutorAudio::~ReprodutorAudio() { callbackEstado = {}; parar(); }

void ReprodutorAudio::aoAlterarEstado(std::function<void()> callback) {
    callbackEstado = std::move(callback);
}

void ReprodutorAudio::notificar() {
    if (callbackEstado) callbackEstado();
}

void ReprodutorAudio::parar() {
    // Para o dispositivo ANTES de liberar o buffer de onde ele lê as amostras.
    if (saida) {
        auto antigo = std::move(saida);
        antigo->stop();
    }
    fonte.reset();
    fontePcm.clear();
    taxaAtual = 0;
    canaisAtuais = 0;
    inicioAtualMs = 0;
    notificar();
}

void ReprodutorAudio::limpar() {
    parar();
    originalPcm.clear();
    taxaOriginal = 0;
    canaisOriginais = 0;
    notificar();
}

bool ReprodutorAudio::temOriginal() const { return !originalPcm.isEmpty(); }
bool ReprodutorAudio::tocando() const {
    return reproduzindo() || pausado();
}

bool ReprodutorAudio::reproduzindo() const {
    return saida && saida->state() == QtAudio::ActiveState;
}

bool ReprodutorAudio::pausado() const {
    return saida && saida->state() == QtAudio::SuspendedState;
}

void ReprodutorAudio::pausar() {
    if (reproduzindo()) {
        saida->suspend();
        notificar();
    }
}

void ReprodutorAudio::continuar() {
    if (pausado()) {
        saida->resume();
        notificar();
    }
}

void ReprodutorAudio::definirOriginal(const DadosAudio &audio) {
    limpar();
    if (!audio.get_amostras() || audio.get_num_amostras() == 0) return;
    // O formato PCM usado pelo núcleo é int16_t intercalado por canal.
    const qsizetype bytes = static_cast<qsizetype>(audio.get_num_amostras()) * sizeof(int16_t);
    originalPcm = QByteArray(reinterpret_cast<const char *>(audio.get_amostras()), bytes);
    taxaOriginal = static_cast<int>(audio.get_sample_rate());
    canaisOriginais = static_cast<int>(audio.get_num_canais());
    notificar();
}

bool ReprodutorAudio::tocarOriginal(QString &erro) {
    if (!temOriginal()) {
        erro = "Carregue um WAV para ouvir o original.";
        return false;
    }
    return iniciar(originalPcm, taxaOriginal, canaisOriginais, erro);
}

bool ReprodutorAudio::tocarProcessado(const DadosAudio &audio, QString &erro) {
    if (!audio.get_amostras() || audio.get_num_amostras() == 0) {
        erro = "Não há amostras processadas para reprodução.";
        return false;
    }
    const qsizetype bytes = static_cast<qsizetype>(audio.get_num_amostras()) * sizeof(int16_t);
    // Não duplica o áudio processado: os dados permanecem sob posse de DadosAudio.
    const QByteArray visualizacao = QByteArray::fromRawData(
        reinterpret_cast<const char *>(audio.get_amostras()), bytes);
    return iniciar(visualizacao, static_cast<int>(audio.get_sample_rate()),
                   static_cast<int>(audio.get_num_canais()), erro);
}

qint64 ReprodutorAudio::duracaoMs() const {
    if (taxaAtual <= 0 || canaisAtuais <= 0) return 0;
    const qint64 quadros = fontePcm.size() / (static_cast<qint64>(canaisAtuais) * sizeof(int16_t));
    return quadros * 1000 / taxaAtual;
}

qint64 ReprodutorAudio::posicaoMs() const {
    if (!saida) return 0;
    return std::min(duracaoMs(), inicioAtualMs + saida->processedUSecs() / 1000);
}

bool ReprodutorAudio::buscarMs(qint64 posicao, QString &erro) {
    if (!tocando() || fontePcm.isEmpty()) {
        erro = "Inicie a reprodução antes de avançar ou voltar.";
        return false;
    }
    const QByteArray pcm = fontePcm;
    const bool estavaPausado = pausado();
    if (!iniciar(pcm, taxaAtual, canaisAtuais, erro, posicao)) return false;
    if (estavaPausado) pausar();
    return true;
}

bool ReprodutorAudio::iniciar(const QByteArray &pcm, int taxa, int canais, QString &erro, qint64 inicioMs) {
    parar();
    if (pcm.isEmpty() || taxa <= 0 || canais <= 0) {
        erro = "Formato de áudio inválido para reprodução.";
        return false;
    }

    const QAudioDevice dispositivo = QMediaDevices::defaultAudioOutput();
    if (dispositivo.isNull()) {
        erro = "Nenhum dispositivo de saída de áudio foi encontrado.";
        return false;
    }
    QAudioFormat formato;
    formato.setSampleRate(taxa);
    formato.setChannelCount(canais);
    formato.setSampleFormat(QAudioFormat::Int16);
    if (!dispositivo.isFormatSupported(formato)) {
        erro = "O dispositivo de som não suporta esta taxa/canais em PCM de 16 bits.";
        return false;
    }

    fontePcm = pcm; // compartilhamento de memória do QByteArray (sem cópia profunda)
    taxaAtual = taxa;
    canaisAtuais = canais;
    const qint64 msMaximo = duracaoMs() > 0 ? duracaoMs() - 1 : 0;
    inicioAtualMs = std::max<qint64>(0, std::min(inicioMs, msMaximo));
    fonte = std::make_unique<QBuffer>(&fontePcm);
    if (!fonte->open(QIODevice::ReadOnly)) {
        erro = "Não foi possível preparar o áudio para reprodução.";
        parar();
        return false;
    }
    const qint64 tamanhoQuadro = static_cast<qint64>(canais) * sizeof(int16_t);
    const qint64 quadroInicial = inicioAtualMs * taxa / 1000;
    const qint64 deslocamento = std::min<qint64>(
        quadroInicial * tamanhoQuadro, fontePcm.size() - tamanhoQuadro);
    if (deslocamento > 0 && !fonte->seek(deslocamento)) {
        erro = "Não foi possível posicionar a reprodução.";
        parar();
        return false;
    }

    saida = std::make_unique<QAudioSink>(dispositivo, formato);
    QAudioSink *atual = saida.get();
    connect(atual, &QAudioSink::stateChanged, this, [this, atual](QtAudio::State estado) {
        notificar();
        if (estado == QtAudio::IdleState ||
            (estado == QtAudio::StoppedState && atual->error() != QtAudio::NoError)) {
            // Não destruir o QAudioSink dentro da sua própria emissão de sinal.
            QTimer::singleShot(0, this, [this, atual]() {
                if (saida.get() != atual) return;
                if (saida->state() == QtAudio::IdleState ||
                    saida->state() == QtAudio::StoppedState)
                    parar();
            });
        }
    });
    atual->start(fonte.get());
    if (atual->error() != QtAudio::NoError) {
        // QAudioSink informa falhas pelo enum QtAudio::Error (nao possui errorString()).
        switch (atual->error()) {
        case QtAudio::OpenError:
            erro = "Não foi possível abrir o dispositivo de saída de áudio.";
            break;
        case QtAudio::IOError:
            erro = "Ocorreu uma falha de leitura ou escrita no dispositivo de áudio.";
            break;
        case QtAudio::FatalError:
            erro = "O dispositivo de áudio apresentou uma falha grave.";
            break;
        default:
            erro = "Não foi possível iniciar a reprodução de áudio.";
            break;
        }
        parar();
        return false;
    }
    notificar();
    return true;
}
