#pragma once
#include <QString>
#include <QVector>

// Catálogo dos filtros que a interface oferece (nome + parâmetros ajustáveis).
// É só a descrição para a tela: os filtros de verdade (FiltroAudio) ficam no núcleo do projeto
// e são criados em ponte_filtros.cpp.

// Identifica cada filtro da tela (usado para ligar ao filtro real do núcleo)
enum class TipoFiltro {
    Volume,
    Distorcao,
    Eco,
    ReforcoFrequencia,
    LimpezaFala,
};

struct ParametroDef {
    QString nome;
    int min;
    int max;
    int padrao;
    QString unidade;
};

struct FiltroDef {
    QString nome;
    TipoFiltro tipo;
    QVector<ParametroDef> parametros;   // no máximo 2 (a tela tem 2 sliders)
};

inline const QVector<FiltroDef> &catalogoFiltros() {
    static const QVector<FiltroDef> filtros = {
        {"Ganho de volume", TipoFiltro::Volume,
            {{"Ganho", -20, 20, 0, " dB"}}},
        {"Distorção", TipoFiltro::Distorcao,
            {{"Intensidade", 0, 100, 30, " %"}, {"Limite", 10, 100, 80, " %"}}},
        {"Eco / Delay", TipoFiltro::Eco,
            {{"Atraso", 50, 1000, 300, " ms"}, {"Repetição", 0, 90, 40, " %"}}},
        {"Reforço de frequência (perda auditiva)", TipoFiltro::ReforcoFrequencia,
            {{"Frequência central", 250, 8000, 2000, " Hz"}, {"Ganho", -24, 24, 6, " dB"}}},
        // Faixas separadas garantem corte grave menor que corte agudo.
        {"Limpeza de fala", TipoFiltro::LimpezaFala,
            {{"Corte grave", 100, 1000, 300, " Hz"}, {"Corte agudo", 1200, 6000, 3400, " Hz"}}},
    };
    return filtros;
}
