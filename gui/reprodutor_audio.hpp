#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>

#include <functional>
#include <memory>

class DadosAudio;
class QAudioSink;
class QBuffer;

// A GUI mantém uma cópia PCM do original, pois o núcleo processa DadosAudio
// in-place. O áudio processado é reproduzido diretamente das amostras atuais.
class ReprodutorAudio final : public QObject {
public:
    explicit ReprodutorAudio(QObject *parent = nullptr);
    ~ReprodutorAudio() override;

    void definirOriginal(const DadosAudio &audio);
    void limpar();
    void parar();
    bool temOriginal() const;
    bool tocando() const;
    bool pausado() const;
    bool reproduzindo() const;
    void pausar();
    void continuar();
    bool tocarOriginal(QString &erro);
    bool tocarProcessado(const DadosAudio &audio, QString &erro);
    qint64 duracaoMs() const;
    qint64 posicaoMs() const;
    bool buscarMs(qint64 posicao, QString &erro);
    void aoAlterarEstado(std::function<void()> callback);

private:
    bool iniciar(const QByteArray &pcm, int taxa, int canais, QString &erro, qint64 inicioMs = 0);
    void notificar();

    QByteArray originalPcm;
    QByteArray fontePcm; // também aceita visualização sem cópia (fromRawData)
    int taxaOriginal = 0;
    int canaisOriginais = 0;
    int taxaAtual = 0;
    int canaisAtuais = 0;
    qint64 inicioAtualMs = 0;
    std::unique_ptr<QBuffer> fonte;
    std::unique_ptr<QAudioSink> saida;
    std::function<void()> callbackEstado;
};
