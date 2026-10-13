#include "interfacegrafica.hpp"
#include "catalogo_filtros.hpp"
#include <QComboBox>
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

} // namespace

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
        comboFiltros->addItem(f.nome);
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

    QPushButton *botaoAdicionar = new QPushButton("Adicionar à cadeia", grupoFiltros);
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
    botaoRemover = new QPushButton("Remover", grupoCadeia);
    botaoLimpar  = new QPushButton("Limpar tudo", grupoCadeia);
    botoesCadeia->addWidget(botaoSubir);
    botoesCadeia->addWidget(botaoDescer);
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
    connect(botaoLimpar,    &QPushButton::clicked, this, &InterfaceGrafica::aoLimparCadeia);
    connect(botaoSubir,     &QPushButton::clicked, this, [this]() { moverFiltro(-1); });
    connect(botaoDescer,    &QPushButton::clicked, this, [this]() { moverFiltro(+1); });
    connect(listaCadeia, &QListWidget::currentRowChanged, this, [this](int) { atualizarBotoesCadeia(); });

    // ----- Ligações: processamento -----
    connect(botaoProcessar, &QPushButton::clicked, this, &InterfaceGrafica::aoProcessar);
    connect(campoEntrada, &QLineEdit::textChanged, this, [this]() {
        arquivoCarregado = false;   // se o caminho mudou, é preciso carregar de novo
        atualizarEstado();
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

    // TODO: chamar o GerenciadorArquivoWav aqui (caminho.toStdString())
    arquivoCarregado = true;
    atualizarEstado();
    statusBar()->showMessage("Arquivo carregado: " + caminho);
}

void InterfaceGrafica::exportar() {
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

    // TODO: chamar a exportação do GerenciadorArquivoWav aqui, dentro de try/catch
    statusBar()->showMessage("Exportado para: " + caminho);
}

// ---------- Filtros ----------
void InterfaceGrafica::aoMudarFiltro(int indice) {
    if (indice < 0) return;
    const FiltroDef &f = catalogoFiltros()[indice];

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
    if (indice < 0) return;
    const FiltroDef &f = catalogoFiltros()[indice];

    // Guarda no item: [índice do filtro no catálogo, valor1, valor2...]
    // (é daqui que a CadeiaFiltros real será montada depois)
    QVariantList dados;
    dados << indice;
    QStringList partes;
    for (int i = 0; i < f.parametros.size(); ++i) {
        int v = slider[i]->value();
        dados << v;
        partes << f.parametros[i].nome + ": " + QString::number(v) + f.parametros[i].unidade;
    }

    QString texto = f.nome;
    if (!partes.isEmpty())
        texto += " (" + partes.join(", ") + ")";

    QListWidgetItem *item = new QListWidgetItem(texto, listaCadeia);   // a lista é a dona do item
    item->setData(Qt::UserRole, dados);
    listaCadeia->setCurrentItem(item);

    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage(QString("Filtro adicionado. Cadeia com %1 filtro(s).").arg(listaCadeia->count()));
}

void InterfaceGrafica::aoRemoverFiltro() {
    int linha = listaCadeia->currentRow();
    if (linha < 0) return;

    delete listaCadeia->takeItem(linha);   // takeItem tira da lista e passa a posse; o delete libera
    if (listaCadeia->count() > 0)
        listaCadeia->setCurrentRow(qMin(linha, listaCadeia->count() - 1));

    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage("Filtro removido.");
}

void InterfaceGrafica::aoLimparCadeia() {
    if (listaCadeia->count() == 0) return;

    auto resposta = QMessageBox::question(
        this, "Limpar cadeia", "Remover todos os filtros da cadeia?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (resposta != QMessageBox::Yes)
        return;

    listaCadeia->clear();
    atualizarBotoesCadeia();
    atualizarEstado();
    statusBar()->showMessage("Cadeia limpa.");
}

void InterfaceGrafica::moverFiltro(int delta) {
    int linha = listaCadeia->currentRow();
    int destino = linha + delta;
    if (linha < 0 || destino < 0 || destino >= listaCadeia->count())
        return;

    QListWidgetItem *item = listaCadeia->takeItem(linha);
    listaCadeia->insertItem(destino, item);
    listaCadeia->setCurrentRow(destino);
}

void InterfaceGrafica::atualizarBotoesCadeia() {
    int linha = listaCadeia->currentRow();
    int total = listaCadeia->count();
    botaoSubir->setEnabled(linha > 0);
    botaoDescer->setEnabled(linha >= 0 && linha < total - 1);
    botaoRemover->setEnabled(linha >= 0);
    botaoLimpar->setEnabled(total > 0);
}

// ---------- Processamento ----------
// O botão Processar só fica ativo com um arquivo carregado e ao menos um filtro na cadeia.
void InterfaceGrafica::atualizarEstado() {
    bool temCadeia = listaCadeia->count() > 0;
    botaoProcessar->setEnabled(arquivoCarregado && temCadeia);

    if (!arquivoCarregado)
        botaoProcessar->setToolTip("Carregue um arquivo .wav primeiro.");
    else if (!temCadeia)
        botaoProcessar->setToolTip("Adicione ao menos um filtro à cadeia.");
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
    if (!arquivoCarregado || quantidade == 0) {   // proteção extra: o botão já deveria estar desabilitado
        QMessageBox::warning(this, "Atenção",
            "Carregue um arquivo .wav e adicione ao menos um filtro à cadeia.");
        return;
    }

    definirOcupado(true);

    // TODO: montar a CadeiaFiltros a partir dos itens da lista (Qt::UserRole de cada um)
    //       e executá-la sobre o DadosAudio carregado, dentro de try/catch,
    //       mostrando erros com QMessageBox::critical.

    definirOcupado(false);
    statusBar()->showMessage(
        QString("Processamento ainda não integrado: %1 filtro(s) na cadeia.").arg(quantidade));
}
