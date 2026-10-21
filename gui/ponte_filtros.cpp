#include "ponte_filtros.hpp"
#include "filtro_limpa_ruido.hpp"
#include "filtro_volume.hpp"
#include <cmath>

bool filtroDisponivel(TipoFiltro tipo) {
    switch (tipo) {
        case TipoFiltro::Volume:
        case TipoFiltro::LimpezaFala:
            return true;
        default:   // distorção, eco e reforço de frequência ainda não existem no núcleo
            return false;
    }
}

double decibelsParaPorcentagem(int decibels) {
    return 100.0 * std::pow(10.0, decibels / 20.0);
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
                    decibelsParaPorcentagem(valorDoItem(dados, 1, def)));
                break;
            case TipoFiltro::LimpezaFala:
                filtro = std::make_unique<FiltroLimpaRuido>();
                break;
            default:
                break;   // ainda não existe no núcleo
        }

        if (!filtro) {
            resultado.ignorados << def.nome;
            continue;
        }
        if (!resultado.cadeia->adicionar_filtro(filtro.get())) {
            resultado.erro = QString("A cadeia aceita no máximo %1 filtros.").arg(LIMITE_FILTROS_CADEIA);
            return resultado;
        }
        resultado.filtros.push_back(std::move(filtro));
    }
    return resultado;
}
