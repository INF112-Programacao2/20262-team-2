#pragma once

#include "dados_audio.hpp"

#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QVector>
#include <QWidget>

#include <algorithm>
#include <cstdint>
#include <utility>

// Widget independente do nucleo: le as amostras e guarda somente um resumo
// de amplitudes (minimo/maximo), sem copiar o WAV inteiro para a interface.
class VisualizacaoOnda final : public QWidget {
public:
    explicit VisualizacaoOnda(const QColor &cor, QWidget *parent = nullptr)
        : QWidget(parent), corOnda(cor) {
        setMinimumHeight(75);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        // Paleta monocromatica coerente com a identidade visual da GUI.
        QPalette cores = palette();
        cores.setColor(QPalette::Base, QColor(22, 22, 22));
        cores.setColor(QPalette::Mid, QColor(78, 78, 78));
        cores.setColor(QPalette::Text, QColor(178, 178, 178));
        setPalette(cores);
    }

    void limpar() {
        faixas.clear();
        update();
    }

    void definirAudio(const DadosAudio &audio) {
        const int16_t *amostras = audio.get_amostras();
        const uint32_t total = audio.get_num_amostras();
        if (!amostras || total == 0) {
            limpar();
            return;
        }

        // Um resumo de ate 1024 faixas evita manter duas copias de um WAV grande.
        const uint32_t quantidade = std::min<uint32_t>(1024, total);
        QVector<Faixa> resumo(static_cast<qsizetype>(quantidade));
        for (uint32_t i = 0; i < total; ++i) {
            const uint32_t posicao = static_cast<uint32_t>(
                (static_cast<uint64_t>(i) * quantidade) / total);
            const float amplitude = static_cast<float>(amostras[i]) / 32768.0f;
            Faixa &faixa = resumo[static_cast<qsizetype>(posicao)];
            faixa.minimo = std::min(faixa.minimo, amplitude);
            faixa.maximo = std::max(faixa.maximo, amplitude);
        }
        faixas = std::move(resumo);
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter pintor(this);
        const QRect area = rect().adjusted(5, 5, -5, -5);
        if (area.width() <= 2 || area.height() <= 2)
            return;

        pintor.fillRect(area, palette().color(QPalette::Base));
        pintor.setPen(palette().color(QPalette::Mid));
        pintor.drawRect(area);

        if (faixas.isEmpty()) {
            pintor.setPen(palette().color(QPalette::Text));
            pintor.drawText(area, Qt::AlignCenter, "Sem amostras para exibir");
            return;
        }

        const int centro = area.center().y();
        const int altura = (area.height() - 4) / 2;
        pintor.drawLine(area.left() + 1, centro, area.right() - 1, centro);
        pintor.setPen(corOnda);

        const int largura = area.width() - 2;
        const uint64_t totalFaixas = static_cast<uint64_t>(faixas.size());
        for (int x = 0; x < largura; ++x) {
            const qsizetype inicio = static_cast<qsizetype>(
                (static_cast<uint64_t>(x) * totalFaixas) / largura);
            const qsizetype fim = static_cast<qsizetype>(
                (static_cast<uint64_t>(x + 1) * totalFaixas) / largura);
            float minimo = 0.0f;
            float maximo = 0.0f;
            for (qsizetype i = inicio; i < std::max(inicio + 1, fim) && i < faixas.size(); ++i) {
                minimo = std::min(minimo, faixas[i].minimo);
                maximo = std::max(maximo, faixas[i].maximo);
            }
            const int px = area.left() + 1 + x;
            pintor.drawLine(px,
                            centro - static_cast<int>(maximo * altura),
                            px,
                            centro - static_cast<int>(minimo * altura));
        }
    }

private:
    struct Faixa {
        float minimo = 0.0f;
        float maximo = 0.0f;
    };

    QColor corOnda;
    QVector<Faixa> faixas;
};
