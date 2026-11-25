#pragma once

#include "dados_audio.hpp"

#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QVector>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

// Visualizacao de amplitudes: apenas resume as amostras, sem alterar o audio.
class VisualizacaoOnda final : public QWidget {
public:
    explicit VisualizacaoOnda(const QColor &cor, QWidget *parent = nullptr)
        : QWidget(parent), corOnda(cor) {
        setMinimumHeight(100);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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

        // Ate 768 trechos representam toda a duracao sem guardar o WAV na GUI.
        const uint32_t quantidade = std::min<uint32_t>(768, total);
        QVector<Faixa> resumo(static_cast<qsizetype>(quantidade));
        for (uint32_t i = 0; i < total; ++i) {
            const uint32_t posicao = static_cast<uint32_t>(
                (static_cast<uint64_t>(i) * quantidade) / total);
            const float amplitude = std::abs(static_cast<float>(amostras[i])) / 32768.0f;
            Faixa &faixa = resumo[static_cast<qsizetype>(posicao)];
            faixa.pico = std::max(faixa.pico, amplitude);
            faixa.somaQuadrados += static_cast<double>(amplitude) * amplitude;
            ++faixa.contagem;
        }

        for (Faixa &faixa : resumo) {
            if (faixa.contagem > 0) {
                const float mediaQuadratica = static_cast<float>(
                    std::sqrt(faixa.somaQuadrados / faixa.contagem));
                faixa.nivel = std::clamp(
                    0.55f * mediaQuadratica + 0.45f * faixa.pico, 0.0f, 1.0f);
            }
        }
        faixas = std::move(resumo);
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter pintor(this);
        pintor.fillRect(rect(), QColor(12, 12, 11));
        const QRect area = rect().adjusted(7, 7, -7, -7);
        if (area.width() < 12 || area.height() < 12)
            return;

        pintor.setPen(QColor(48, 48, 41));
        pintor.drawRoundedRect(area.adjusted(0, 0, -1, -1), 5, 5);
        if (faixas.isEmpty()) {
            pintor.setPen(QColor(152, 152, 142));
            pintor.drawText(area, Qt::AlignCenter, "Sem amostras para exibir");
            return;
        }

        // Barras finas, espelhadas no centro, com um pixel de separacao.
        const int passo = 3;
        const int larguraBarra = 2;
        const int larguraUtil = area.width() - 10;
        const int barras = std::max(1, larguraUtil / passo);
        const int centro = area.center().y();
        const int alturaMaxima = std::max(1, (area.height() - 10) / 2);
        const uint64_t totalFaixas = static_cast<uint64_t>(faixas.size());

        for (int coluna = 0; coluna < barras; ++coluna) {
            const qsizetype inicio = static_cast<qsizetype>(
                (static_cast<uint64_t>(coluna) * totalFaixas) / barras);
            const qsizetype fim = static_cast<qsizetype>(
                (static_cast<uint64_t>(coluna + 1) * totalFaixas) / barras);
            float nivel = 0.0f;
            for (qsizetype i = inicio;
                 i < std::max(inicio + 1, fim) && i < faixas.size(); ++i) {
                nivel = std::max(nivel, faixas[i].nivel);
            }
            // Realca volumes baixos apenas no desenho, sem alterar o som.
            const float alturaVisual = std::sqrt(nivel);
            const int metade = std::max(1, static_cast<int>(alturaVisual * alturaMaxima));
            const int x = area.left() + 5 + coluna * passo;
            pintor.fillRect(x, centro - metade, larguraBarra, 2 * metade + 1, corOnda);
        }
    }

private:
    struct Faixa {
        float pico = 0.0f;
        double somaQuadrados = 0.0;
        uint32_t contagem = 0;
        float nivel = 0.0f;
    };

    QColor corOnda;
    QVector<Faixa> faixas;
};
