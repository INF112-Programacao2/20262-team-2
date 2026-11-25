#include "ponte_filtros.hpp"
#include "filtro_distorcao.hpp"
#include "filtro_eco.hpp"
#include "filtro_ganho_canal.hpp"
#include "filtro_limpa_ruido.hpp"
#include "filtro_realce_freq.hpp"
#include "filtro_volume.hpp"
#include <cmath>

bool filtroDisponivel(TipoFiltro tipo) {
    switch (tipo) {
        case TipoFiltro::Volume:
        case TipoFiltro::Distorcao:
        case TipoFiltro::Eco:
        case TipoFiltro::GanhoPorCanal:
        case TipoFiltro::ReforcoFrequencia:
        case TipoFiltro::LimpezaFala:
            return true;
        default:
            return false;
    }
}

double decibelsParaMultiplicador(int decibels) {
    return std::pow(10.0, decibels / 20.0);
}

namespace {

// Valor do parâmetro (posição 1 ou 2 no item); se faltar, usa o padrão do catálogo
int valorDoItem(const QVariantList &dados, int posicao, const FiltroDef &def) {
    if (posicao < dados.size())
        return dados[posicao].toInt();
    if (posicao - 1 < def.parametros.size())
        return def.parametros[posicao - 1].padrao;
    return 0;
}

} // namespace

CadeiaMontada montarCadeia(const QList<QVariantList> &itens) {
    CadeiaMontada resultado;
    resultado.cadeia = std::make_unique<CadeiaFiltros>();

    for (const QVariantList &dados : itens) {
        int indice = dados.isEmpty() ? -1 : dados[0].toInt();
        if (indice < 0 || indice >= catalogoFiltros().size()) {
            resultado.erro = "Item inválido na cadeia de filtros.";
            return resultado;
        }
        const FiltroDef &def = catalogoFiltros()[indice];

        std::unique_ptr<FiltroAudio> filtro;
        switch (def.tipo) {
            case TipoFiltro::Volume:
                filtro = std::make_unique<FiltroVolume>(
                    decibelsParaMultiplicador(valorDoItem(dados, 1, def)));
                break;
            case TipoFiltro::Distorcao: {
                // Intensidade de 0 a 100% vira drive de 1.0 a 5.0.
                const double intensidade = valorDoItem(dados, 1, def) / 100.0;
                const double drive = 1.0 + 4.0 * intensidade;
                const double limite = valorDoItem(dados, 2, def) / 100.0;
                // Reducao de saida fixa: a GUI oferece somente dois controles por filtro.
                filtro = std::make_unique<FiltroDistorcao>(drive, limite, 0.8);
                break;
            }
            case TipoFiltro::Eco:
                filtro = std::make_unique<FiltroEco>(
                    valorDoItem(dados, 1, def), valorDoItem(dados, 2, def) / 100.0);
                break;
            case TipoFiltro::GanhoPorCanal:
                filtro = std::make_unique<FiltroGanhoCanal>(
                    valorDoItem(dados, 1, def), valorDoItem(dados, 2, def));
                break;
            case TipoFiltro::ReforcoFrequencia:
                filtro = std::make_unique<FiltroRealceFreq>(
                    valorDoItem(dados, 1, def), valorDoItem(dados, 2, def));
                break;
            case TipoFiltro::LimpezaFala:
                filtro = std::make_unique<FiltroLimpaRuido>(
                    valorDoItem(dados, 1, def), valorDoItem(dados, 2, def));
                break;
            default:
                break;
        }

        if (!filtro) {
            resultado.ignorados << def.nome;
            continue;
        }
        // a cadeia passa a ser a dona do filtro
        if (!resultado.cadeia->adicionar_filtro(std::move(filtro))) {
            resultado.erro = QString("A cadeia aceita no máximo %1 filtros.").arg(LIMITE_FILTROS_CADEIA);
            return resultado;
        }
    }
    return resultado;
}
