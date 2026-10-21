#pragma once
#include <memory>
#include <vector>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include "catalogo_filtros.hpp"
#include "cadeia_filtros.hpp"
#include "filtro_audio.hpp"

// Ponte entre a tela e o núcleo do projeto: transforma a lista de filtros da interface
// na CadeiaFiltros real (classes do grupo).

// Mesmo limite de CadeiaFiltros::capacidade_max_filtros (constante privada do núcleo)
constexpr int LIMITE_FILTROS_CADEIA = 64;

// true se já existe a classe do filtro no núcleo
bool filtroDisponivel(TipoFiltro tipo);

// O FiltroVolume recebe porcentagem (100 = sem mudança) e a tela usa dB
double decibelsParaPorcentagem(int decibels);

struct CadeiaMontada {
    // A CadeiaFiltros NÃO é dona dos filtros: quem cria precisa guardá-los e liberá-los.
    std::vector<std::unique_ptr<FiltroAudio>> filtros;
    // Declarada depois dos filtros, então é destruída antes deles
    std::unique_ptr<CadeiaFiltros> cadeia;
    QStringList ignorados;   // filtros da lista que ainda não existem no núcleo
    QString erro;            // preenchido se algo impediu a montagem
};

// Cada item é o que a lista da tela guarda: [índice no catálogo, valor1, valor2...]
CadeiaMontada montarCadeia(const QList<QVariantList> &itens);
