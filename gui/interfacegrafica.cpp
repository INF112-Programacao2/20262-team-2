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
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStatusBar>
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
    layout->addWidget(grupoFiltros);

    // (os próximos commits adicionam os outros widgets aqui, antes do stretch)
    layout->addStretch();

    // ----- Ligações: arquivos -----
    connect(botaoProcurarEntrada, &QPushButton::clicked, this, &InterfaceGrafica::escolherEntrada);
    connect(botaoProcurarSaida,   &QPushButton::clicked, this, &InterfaceGrafica::escolherSaida);
    connect(botaoCarregar,        &QPushButton::clicked, this, &InterfaceGrafica::carregar);
    connect(botaoExportar,        &QPushButton::clicked, this, &InterfaceGrafica::exportar);

    // ----- Ligações: filtros -----
    connect(comboFiltros, &QComboBox::currentIndexChanged, this, &InterfaceGrafica::aoMudarFiltro);
    for (int i = 0; i < 2; ++i)
        connect(slider[i], &QSlider::valueChanged, this, [this, i](int) { atualizarValor(i); });

    aoMudarFiltro(0);   // inicializa os sliders com o primeiro filtro
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
