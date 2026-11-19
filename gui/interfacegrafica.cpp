#include "interfacegrafica.hpp"
#include "catalogo_filtros.hpp"
#include "ponte_filtros.hpp"
#include "gerenciador_arquivo_WAV.hpp"
#include "dados_audio.hpp"
#include <exception>
#include <memory>
#include <utility>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QStringList>
#include <QVariantList>
#include <QVBoxLayout>
#include <QWidget>

namespace {

// Remove espaços e aspas nas pontas (o Windows põe aspas em "Copiar como caminho")
QString limparCaminho(QString caminho) {
    caminho = caminho.trimmed();
    if (caminho.size() >= 2 && caminho.startsWith('"') && caminho.endsWith('"'))
        caminho = caminho.mid(1, caminho.size() - 2);
    return caminho.trimmed();
}

// Texto exibido na cadeia, sempre derivado dos parametros salvos no item.
QString descricaoDoFiltro(const FiltroDef &f, const QVariantList &dados) {
    QStringList partes;
    for (int i = 0; i < f.parametros.size(); ++i) {
        const ParametroDef &p = f.parametros[i];
        const int valor = i + 1 < dados.size() ? dados[i + 1].toInt() : p.padrao;
        partes << p.nome + ": " + QString::number(valor) + p.unidade;
    }
    return partes.isEmpty() ? f.nome : f.nome + " (" + partes.join(", ") + ")";
}

} // namespace

InterfaceGrafica::~InterfaceGrafica() = default;

InterfaceGrafica::InterfaceGrafica(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Processador de Áudio");
    resize(650, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // ----- Linha de entrada -----
    layout->addWidget(new QLabel("Arquivo de entrada (.wav):", central));
    QHBoxLayout *linhaEntrada = new QHBoxLayout();
    campoEntrada = new QLineEdit(central);
    QPushButton *botaoProcurarEntrada = new QPushButton("Procurar...", central);
    QPushButton *botaoCarregar = new QPushButton("Carregar", central);
    linhaEntrada->addWidget(campoEntrada);
    linhaEntrada->addWidget(botaoProcurarEntrada);
    linhaEntrada->addWidget(botaoCarregar);
    layout->addLayout(linhaEntrada);

    // ----- Linha de saída -----
    layout->addWidget(new QLabel("Arquivo de saída (.wav):", central));
    QHBoxLayout *linhaSaida = new QHBoxLayout();
    campoSaida = new QLineEdit(central);
    QPushButton *botaoProcurarSaida = new QPushButton("Procurar...", central);
    QPushButton *botaoExportar = new QPushButton("Exportar", central);
    linhaSaida->addWidget(campoSaida);
    linhaSaida->addWidget(botaoProcurarSaida);
    linhaSaida->addWidget(botaoExportar);
    layout->addLayout(linhaSaida);

    // ----- Grupo: escolha do filtro -----
    QGroupBox *grupoFiltros = new QGroupBox("Filtros", central);
    QVBoxLayout *layoutFiltros = new QVBoxLayout(grupoFiltros);

    comboFiltros = new QComboBox(grupoFiltros);
    for (const FiltroDef &f : catalogoFiltros())
        comboFiltros->addItem(filtroDisponivel(f.tipo) ? f.nome : f.nome + " (em breve)");
    layoutFiltros->addWidget(comboFiltros);

    QGridLayout *grade = new QGridLayout();
    for (int i = 0; i < 2; ++i) {
        rotuloParam[i] = new QLabel(grupoFiltros);
        slider[i] = new QSlider(Qt::Horizontal, grupoFiltros);
        valorParam[i] = new QLabel(grupoFiltros);
        valorParam[i]->setMinimumWidth(80);
        grade->addWidget(rotuloParam[i], i, 0);
        grade->addWidget(slider[i], i, 1);
        grade->addWidget(valorParam[i], i, 2);
    }
    grade->setColumnStretch(1, 1);
    layoutFiltros->addLayout(grade);

    botaoAdicionar = new QPushButton("Adicionar à cadeia", grupoFiltros);
    layoutFiltros->addWidget(botaoAdicionar);
    layout->addWidget(grupoFiltros);

    // ----- Grupo: cadeia de filtros -----
    QGroupBox *grupoCadeia = new QGroupBox("Cadeia de filtros (executa de cima para baixo)", central);
    QVBoxLayout *layoutCadeia = new QVBoxLayout(grupoCadeia);

    listaCadeia = new QListWidget(grupoCadeia);
    layoutCadeia->addWidget(listaCadeia);

    QHBoxLayout *botoesCadeia = new QHBoxLayout();
    botaoSubir   = new QPushButton("Subir", grupoCadeia);
    botaoDescer  = new QPushButton("Descer", grupoCadeia);
    botaoEditar  = new QPushButton("Editar parâmetros", grupoCadeia);
    botaoRemover = new QPushButton("Remover", grupoCadeia);
    botaoLimpar  = new QPushButton("Limpar tudo", grupoCadeia);
    botoesCadeia->addWidget(botaoSubir);
    botoesCadeia->addWidget(botaoDescer);
    botoesCadeia->addWidget(botaoEditar);
    botoesCadeia->addWidget(botaoRemover);
    botoesCadeia->addWidget(botaoLimpar);
    layoutCadeia->addLayout(botoesCadeia);

    layout->addWidget(grupoCadeia, 1);   // o 1 faz a cadeia ocupar o espaço que sobra

    botaoProcessar = new QPushButton("Processar", central);
    layout->addWidget(botaoProcessar);

    // (os próximos commits adicionam os outros widgets aqui, abaixo do botão Processar)

    // ----- Ligações: arquivos -----
    connect(botaoProcurarEntrada, &QPushButton::clicked, this, &InterfaceGrafica::escolherEntrada);
    connect(botaoProcurarSaida,   &QPushButton::clicked, this, &InterfaceGrafica::escolherSaida);
    connect(botaoCarregar,        &QPushButton::clicked, this, &InterfaceGrafica::carregar);
    connect(botaoExportar,        &QPushButton::clicked, this, &InterfaceGrafica::exportar);

    // ----- Ligações: filtros -----
    connect(comboFiltros, &QComboBox::currentIndexChanged, this, &InterfaceGrafica::aoMudarFiltro);
    for (int i = 0; i < 2; ++i)
        connect(slider[i], &QSlider::valueChanged, this, [this, i](int) { atualizarValor(i); });

    // ----- Ligações: cadeia -----
    connect(botaoAdicionar, &QPushButton::clicked, this, &InterfaceGrafica::aoAdicionarFiltro);
    connect(botaoRemover,   &QPushButton::clicked, this, &InterfaceGrafica::aoRemoverFiltro);
    connect(botaoEditar,    &QPushButton::clicked, this, &InterfaceGrafica::aoEditarFiltro);
    connect(listaCadeia, &QListWidget::itemDoubleClicked, this,
            [this](QListWidgetItem *) { aoEditarFiltro(); });
    connect(botaoLimpar,    &QPushButton::clicked, this, &InterfaceGrafica::aoLimparCadeia);
    connect(botaoSubir,     &QPushButton::clicked, this, [this]() { moverFiltro(-1); });
    connect(botaoDescer,    &QPushButton::clicked, this, [this]() { moverFiltro(+1); });
    connect(listaCadeia, &QListWidget::currentRowChanged, this, [this](int) { atualizarBotoesCadeia(); });

    // ----- Ligações: processamento -----
    connect(botaoProcessar, &QPushButton::clicked, this, &InterfaceGrafica::aoProcessar);
    connect(campoEntrada, &QLineEdit::textChanged, this, [this]() {
        // Caminho alterado: o audio anterior nao representa mais a entrada selecionada.
        audioCarregado.reset();
        arquivoCarregado = false;
        audioProcessado = false;
        atualizarEstado();
        statusBar()->showMessage("Entrada alterada. Carregue o novo arquivo WAV.");
    });

    aoMudarFiltro(0);   // inicializa os sliders com o primeiro filtro
    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage("Pronto.");
}

// ---------- Arquivos ----------
void InterfaceGrafica::escolherEntrada() {
    QString caminho = QFileDialog::getOpenFileName(
        this, "Escolher arquivo de áudio", QString(), "Arquivos WAV (*.wav)");
    if (!caminho.isEmpty())
        campoEntrada->setText(caminho);
}

void InterfaceGrafica::escolherSaida() {
    QString caminho = QFileDialog::getSaveFileName(
        this, "Escolher arquivo de saída", QString(), "Arquivos WAV (*.wav)",
        nullptr, QFileDialog::DontConfirmOverwrite);   // a confirmação é feita em exportar()
    if (!caminho.isEmpty())
        campoSaida->setText(caminho);
}

void InterfaceGrafica::carregar() {
    QString caminho = limparCaminho(campoEntrada->text());

    // Antes de qualquer validacao, descarta o audio anterior.
    // Assim, uma tentativa frustrada de carregamento nao deixa
    // um resultado antigo disponivel para processamento/exportacao.
    audioCarregado.reset();
    arquivoCarregado = false;
    audioProcessado = false;
    atualizarEstado();

    if (caminho.isEmpty()) {
        QMessageBox::warning(this, "Atenção", "Escolha um arquivo de entrada.");
        return;
    }

    QFileInfo info(caminho);
    if (!info.exists()) {
        QMessageBox::critical(this, "Erro", "O arquivo não foi encontrado:\n" + caminho);
        return;
    }
    if (!info.isFile()) {
        QMessageBox::critical(this, "Erro", "O caminho informado não é um arquivo:\n" + caminho);
        return;
    }
    if (!caminho.endsWith(".wav", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, "Formato inválido", "O arquivo precisa ser .wav.");
        return;
    }

    try {
        auto novoAudio = GerenciadorArquivoWav::carregar(caminho.toStdString());
        if (!novoAudio) {
            QMessageBox::critical(this, "Erro ao carregar WAV",
                                  "O gerenciador não retornou um áudio válido.");
            return;
        }

        audioCarregado = std::move(novoAudio);
        audioProcessado = false;
        arquivoCarregado = true;
        atualizarEstado();
        statusBar()->showMessage("Arquivo carregado: " + caminho);
    } catch (const std::exception &erro) {
        QMessageBox::critical(this, "Erro ao carregar WAV",
                              QString::fromUtf8(erro.what()));
    } catch (...) {
        QMessageBox::critical(this, "Erro ao carregar WAV",
                              "Ocorreu um erro inesperado durante o carregamento.");
    }
}

void InterfaceGrafica::exportar() {
    if (!audioCarregado || !audioProcessado) {
        QMessageBox::warning(this, "Atenção",
                             "Carregue e processe um arquivo WAV antes de exportar.");
        return;
    }

    QString caminho = limparCaminho(campoSaida->text());

    if (caminho.isEmpty()) {
        QMessageBox::warning(this, "Atenção", "Escolha o arquivo de saída.");
        return;
    }
    if (!caminho.endsWith(".wav", Qt::CaseInsensitive))
        caminho += ".wav";
    campoSaida->setText(caminho);   // mostra ao usuário o nome final

    QFileInfo info(caminho);
    if (info.isDir()) {
        QMessageBox::critical(this, "Erro", "O caminho informado é uma pasta:\n" + caminho);
        return;
    }
    if (!info.dir().exists()) {
        QMessageBox::critical(this, "Erro",
            "A pasta de destino não existe:\n" + info.absolutePath());
        return;
    }

    // Saída igual à entrada = sobrescrever o original
    QString entrada = limparCaminho(campoEntrada->text());
    bool mesmoArquivo = info.exists() && QFileInfo::exists(entrada)
        && QFileInfo(entrada).canonicalFilePath() == info.canonicalFilePath();

    if (mesmoArquivo || info.exists()) {
        QString texto = mesmoArquivo
            ? "O arquivo de saída é o mesmo arquivo de entrada.\n"
              "Continuar vai substituir o original pela versão processada."
            : "O arquivo já existe. Deseja sobrescrever?\n" + caminho;
        auto resposta = QMessageBox::question(
            this, "Sobrescrever?", texto,
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);   // padrão: Não
        if (resposta != QMessageBox::Yes)
            return;
    }

    try {
        GerenciadorArquivoWav::exportar(caminho.toStdString(), *audioCarregado);
        statusBar()->showMessage("Exportado para: " + caminho);
    } catch (const std::exception &erro) {
        QMessageBox::critical(this, "Erro ao exportar WAV",
                              QString::fromUtf8(erro.what()));
    } catch (...) {
        QMessageBox::critical(this, "Erro ao exportar WAV",
                              "Ocorreu um erro inesperado durante a exportação.");
    }
}

// ---------- Filtros ----------
void InterfaceGrafica::aoMudarFiltro(int indice) {
    if (indice < 0 || indice >= catalogoFiltros().size()) {
        botaoAdicionar->setEnabled(false);
        return;
    }
    const FiltroDef &f = catalogoFiltros()[indice];
    const bool disponivel = filtroDisponivel(f.tipo);
    botaoAdicionar->setEnabled(disponivel);
    botaoAdicionar->setToolTip(disponivel ? QString() :
        "Filtro ainda nao implementado pelo nucleo. Nao pode ser adicionado.");

    for (int i = 0; i < 2; ++i) {
        bool usado = i < f.parametros.size();
        rotuloParam[i]->setVisible(usado);
        slider[i]->setVisible(usado);
        valorParam[i]->setVisible(usado);

        if (usado) {
            const ParametroDef &p = f.parametros[i];
            rotuloParam[i]->setText(p.nome + ":");
            slider[i]->blockSignals(true);   // evita disparar valueChanged durante a configuração
            slider[i]->setRange(p.min, p.max);
            slider[i]->setValue(p.padrao);
            slider[i]->blockSignals(false);
            atualizarValor(i);
        }
    }
}

void InterfaceGrafica::atualizarValor(int i) {
    int indice = comboFiltros->currentIndex();
    if (indice < 0) return;
    const FiltroDef &f = catalogoFiltros()[indice];
    if (i >= f.parametros.size()) return;
    valorParam[i]->setText(QString::number(slider[i]->value()) + f.parametros[i].unidade);
}

// ---------- Cadeia de filtros ----------
void InterfaceGrafica::aoAdicionarFiltro() {
    int indice = comboFiltros->currentIndex();
    if (indice < 0 || indice >= catalogoFiltros().size()) return;
    const FiltroDef &f = catalogoFiltros()[indice];
    if (!filtroDisponivel(f.tipo)) {
        QMessageBox::information(this, "Filtro indisponivel",
                                 "Este filtro ainda nao esta implementado no nucleo.");
        return;
    }

    if (listaCadeia->count() >= LIMITE_FILTROS_CADEIA) {
        QMessageBox::warning(this, "Limite da cadeia",
            QString("A cadeia aceita no máximo %1 filtros.").arg(LIMITE_FILTROS_CADEIA));
        return;
    }

    // Guarda no item: [índice do filtro no catálogo, valor1, valor2...]
    // (é daqui que a CadeiaFiltros real será montada depois)
    QVariantList dados;
    dados << indice;
    for (int i = 0; i < f.parametros.size(); ++i)
        dados << slider[i]->value();

    QListWidgetItem *item = new QListWidgetItem(descricaoDoFiltro(f, dados), listaCadeia);
    item->setData(Qt::UserRole, dados);
    listaCadeia->setCurrentItem(item);

    const bool recarregar = invalidarAudioAposAlterarCadeia();
    atualizarBotoesCadeia();
    atualizarEstado();
    if (recarregar)
        statusBar()->showMessage("Cadeia alterada. Recarregue o WAV antes de processar novamente.");
    else if (filtroDisponivel(f.tipo))
        statusBar()->showMessage(QString("Filtro adicionado. Cadeia com %1 filtro(s).").arg(listaCadeia->count()));
    else
        statusBar()->showMessage("Filtro adicionado, mas ele ainda não existe no núcleo e será ignorado ao processar.");
}

// Edita um filtro ja adicionado, sem alterar os outros itens da cadeia.
void InterfaceGrafica::aoEditarFiltro() {
    QListWidgetItem *item = listaCadeia->currentItem();
    if (!item) return;

    const QVariantList dados = item->data(Qt::UserRole).toList();
    const int indice = dados.isEmpty() ? -1 : dados[0].toInt();
    if (indice < 0 || indice >= catalogoFiltros().size()) {
        QMessageBox::warning(this, "Filtro invalido", "O item selecionado e invalido.");
        return;
    }

    const FiltroDef &f = catalogoFiltros()[indice];
    if (!filtroDisponivel(f.tipo) || f.parametros.isEmpty()) return;

    QDialog dialog(this);
    dialog.setWindowTitle("Editar parametros - " + f.nome);
    QFormLayout formulario(&dialog);
    QVector<QSpinBox *> entradas;
    for (int i = 0; i < f.parametros.size(); ++i) {
        const ParametroDef &p = f.parametros[i];
        auto *controle = new QSpinBox(&dialog);
        controle->setRange(p.min, p.max);
        controle->setSuffix(p.unidade);
        controle->setValue(i + 1 < dados.size() ? dados[i + 1].toInt() : p.padrao);
        formulario.addRow(p.nome + ":", controle);
        entradas << controle;
    }

    QDialogButtonBox acoes(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    formulario.addRow(&acoes);
    connect(&acoes, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&acoes, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;

    QVariantList novosDados;
    novosDados << indice;
    for (QSpinBox *controle : entradas)
        novosDados << controle->value();
    if (novosDados == dados) return;

    item->setData(Qt::UserRole, novosDados);
    item->setText(descricaoDoFiltro(f, novosDados));
    const bool recarregar = invalidarAudioAposAlterarCadeia();
    atualizarEstado();
    statusBar()->showMessage(recarregar
        ? "Parametros alterados. Recarregue o WAV antes de processar novamente."
        : "Parametros do filtro atualizados.");
}

void InterfaceGrafica::aoRemoverFiltro() {
    int linha = listaCadeia->currentRow();
    if (linha < 0) return;

    delete listaCadeia->takeItem(linha);   // takeItem tira da lista e passa a posse; o delete libera
    if (listaCadeia->count() > 0)
        listaCadeia->setCurrentRow(qMin(linha, listaCadeia->count() - 1));

    const bool recarregar = invalidarAudioAposAlterarCadeia();
    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage(recarregar
        ? "Cadeia alterada. Recarregue o WAV antes de processar novamente."
        : "Filtro removido.");
}

void InterfaceGrafica::aoLimparCadeia() {
    if (listaCadeia->count() == 0) return;

    auto resposta = QMessageBox::question(
        this, "Limpar cadeia", "Remover todos os filtros da cadeia?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (resposta != QMessageBox::Yes)
        return;

    listaCadeia->clear();
    const bool recarregar = invalidarAudioAposAlterarCadeia();
    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage(recarregar
        ? "Cadeia alterada. Recarregue o WAV antes de processar novamente."
        : "Cadeia limpa.");
}

void InterfaceGrafica::moverFiltro(int delta) {
    int linha = listaCadeia->currentRow();
    int destino = linha + delta;
    if (linha < 0 || destino < 0 || destino >= listaCadeia->count())
        return;

    QListWidgetItem *item = listaCadeia->takeItem(linha);
    listaCadeia->insertItem(destino, item);
    listaCadeia->setCurrentRow(destino);
    const bool recarregar = invalidarAudioAposAlterarCadeia();
    atualizarBotoesCadeia();
    atualizarEstado();
    if (recarregar)
        statusBar()->showMessage("Ordem alterada. Recarregue o WAV antes de processar novamente.");
}

// O processamento modifica o áudio em memória. Depois de alterar a cadeia,
// não podemos aplicar outra cadeia sobre o resultado anterior nem exportá-lo.
bool InterfaceGrafica::invalidarAudioAposAlterarCadeia() {
    if (!audioProcessado)
        return false;

    audioCarregado.reset();
    arquivoCarregado = false;
    audioProcessado = false;
    return true;
}

void InterfaceGrafica::atualizarBotoesCadeia() {
    int linha = listaCadeia->currentRow();
    int total = listaCadeia->count();
    botaoSubir->setEnabled(linha > 0);
    botaoDescer->setEnabled(linha >= 0 && linha < total - 1);
    botaoRemover->setEnabled(linha >= 0);
    botaoLimpar->setEnabled(total > 0);

    bool podeEditar = false;
    if (linha >= 0) {
        const QVariantList dados = listaCadeia->item(linha)->data(Qt::UserRole).toList();
        const int indice = dados.isEmpty() ? -1 : dados[0].toInt();
        if (indice >= 0 && indice < catalogoFiltros().size()) {
            const FiltroDef &f = catalogoFiltros()[indice];
            podeEditar = filtroDisponivel(f.tipo) && !f.parametros.isEmpty();
        }
    }
    botaoEditar->setEnabled(podeEditar);
}

// ---------- Processamento ----------
// O botão Processar só fica ativo com um arquivo carregado e ao menos um filtro na cadeia.
void InterfaceGrafica::atualizarEstado() {
    bool temCadeia = listaCadeia->count() > 0;
    botaoProcessar->setEnabled(arquivoCarregado && audioCarregado && temCadeia && !audioProcessado);

    if (!arquivoCarregado || !audioCarregado)
        botaoProcessar->setToolTip("Carregue um arquivo .wav primeiro.");
    else if (!temCadeia)
        botaoProcessar->setToolTip("Adicione ao menos um filtro à cadeia.");
    else if (audioProcessado)
        botaoProcessar->setToolTip("Áudio já processado. Recarregue o WAV para processar novamente.");
    else
        botaoProcessar->setToolTip("");
}

// Trava a tela inteira enquanto o processamento roda (evita cliques no meio dele).
void InterfaceGrafica::definirOcupado(bool ocupado) {
    centralWidget()->setEnabled(!ocupado);
    if (!ocupado)
        atualizarEstado();
}

void InterfaceGrafica::aoProcessar() {
    int quantidade = listaCadeia->count();
    if (!arquivoCarregado || !audioCarregado || quantidade == 0) {   // proteção extra: o botão já deveria estar desabilitado
        QMessageBox::warning(this, "Atenção",
            "Carregue um arquivo .wav e adicione ao menos um filtro à cadeia.");
        return;
    }

    // Lista da tela -> CadeiaFiltros real (classes do grupo)
    QList<QVariantList> itens;
    for (int i = 0; i < quantidade; ++i)
        itens << listaCadeia->item(i)->data(Qt::UserRole).toList();

    CadeiaMontada montada;
    try {
        montada = montarCadeia(itens);
    } catch (const std::exception &erro) {
        QMessageBox::critical(this, "Erro ao preparar filtros",
                              QString::fromUtf8(erro.what()));
        return;
    } catch (...) {
        QMessageBox::critical(this, "Erro ao preparar filtros",
                              "Ocorreu um erro inesperado ao montar a cadeia.");
        return;
    }

    if (!montada.erro.isEmpty()) {
        QMessageBox::critical(this, "Erro", montada.erro);
        return;
    }
    if (!montada.cadeia) {
        QMessageBox::critical(this, "Erro ao preparar filtros",
                              "Nao foi possivel criar a cadeia de filtros.");
        return;
    }
    if (montada.cadeia->get_quantidade() == 0) {
        QMessageBox::warning(this, "Filtros indisponíveis",
            "Nenhum filtro da cadeia existe ainda no núcleo do projeto:\n\n- "
            + montada.ignorados.join("\n- "));
        return;
    }
    if (!montada.ignorados.isEmpty()) {
        auto resposta = QMessageBox::question(
            this, "Filtros indisponíveis",
            "Estes filtros ainda não existem no núcleo do projeto e serão ignorados:\n\n- "
                + montada.ignorados.join("\n- ") + "\n\nContinuar mesmo assim?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (resposta != QMessageBox::Yes)
            return;
    }

    definirOcupado(true);
    audioProcessado = false;

    try {
        montada.cadeia->processar(*audioCarregado);
        audioProcessado = true;
        statusBar()->showMessage(
            QString("Áudio processado com %1 filtro(s); %2 ignorado(s). Pronto para exportar.")
                .arg(montada.cadeia->get_quantidade()).arg(montada.ignorados.size()));
    } catch (const std::exception &erro) {
        // O núcleo pode ter alterado parte das amostras antes de lançar a exceção.
        // Invalida o áudio para impedir a exportação de um resultado incompleto.
        audioCarregado.reset();
        arquivoCarregado = false;
        QMessageBox::critical(this, "Erro ao processar áudio",
                              QString::fromUtf8(erro.what()) +
                              "\nCarregue o arquivo novamente antes de processar.");
    } catch (...) {
        audioCarregado.reset();
        arquivoCarregado = false;
        QMessageBox::critical(this, "Erro ao processar áudio",
                              "Falha inesperada. Carregue o arquivo novamente antes de processar.");
    }

    definirOcupado(false);
}
