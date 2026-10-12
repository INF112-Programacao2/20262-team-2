#pragma once
#include <QString>
#include <QVector>

// Catálogo dos filtros que a interface oferece (nome + parâmetros ajustáveis).
// É só a descrição para a tela: os filtros de verdade (FiltroAudio) são dos colegas.

struct ParametroDef {
    QString nome;
    int min;
    int max;
    int padrao;
    QString unidade;
};

struct FiltroDef {
    QString nome;
    QVector<ParametroDef> parametros;   // no máximo 2 (a tela tem 2 sliders)
};

inline const QVector<FiltroDef> &catalogoFiltros() {
    static const QVector<FiltroDef> filtros = {
        {"Ganho de volume",
            {{"Ganho", -20, 20, 0, " dB"}}},
        {"Distorção",
            {{"Intensidade", 0, 100, 30, " %"}}},
        {"Eco / Delay",
            {{"Atraso", 50, 1000, 300, " ms"}, {"Repetição", 0, 90, 40, " %"}}},
        {"Reforço de frequência (perda auditiva)",
            {{"Frequência central", 250, 8000, 2000, " Hz"}, {"Ganho", 0, 24, 6, " dB"}}},
        {"Limpeza de fala",
            {{"Atenuação fora da fala", 0, 100, 70, " %"}}},
    };
    return filtros;
}
