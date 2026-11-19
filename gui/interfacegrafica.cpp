#include "interfacegrafica.hpp"
#include "catalogo_filtros.hpp"
#include "ponte_filtros.hpp"
#include "visualizacao_onda.hpp"
#ifndef GUI_COM_AUDIO
#define GUI_COM_AUDIO 0
#endif
#if GUI_COM_AUDIO
#include "reprodutor_audio.hpp"
#endif
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
#include <QFont>
#include <QFrame>
#include <QIcon>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
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
    setWindowTitle("Na Escuta?");
    setWindowIcon(QIcon(":/na_escuta/logo.png"));
    resize(940, 760);
    setMinimumSize(680, 520);
    setFont(QFont("Segoe UI", 10));

    // A identidade permanece fixa; o formulario rola embaixo dela.
    QWidget *raiz = new QWidget(this);
    QVBoxLayout *estrutura = new QVBoxLayout(raiz);
    estrutura->setContentsMargins(0, 0, 0, 0);
    estrutura->setSpacing(0);

    QFrame *cabecalho = new QFrame(raiz);
    cabecalho->setObjectName("cabecalhoMarca");
    QHBoxLayout *linhaMarca = new QHBoxLayout(cabecalho);
    linhaMarca->setContentsMargins(24, 12, 24, 12);
    linhaMarca->setSpacing(16);
    QLabel *logo = new QLabel(cabecalho);
    logo->setObjectName("logoMarca");
    logo->setFixedSize(108, 86);
    logo->setAlignment(Qt::AlignCenter);
    logo->setAccessibleName("Logo do Na Escuta?");
    const QPixmap imagemLogo(":/na_escuta/logo.png");
    if (!imagemLogo.isNull())
        logo->setPixmap(imagemLogo.scaled(104, 82, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    linhaMarca->addWidget(logo);
    QVBoxLayout *titulos = new QVBoxLayout();
    titulos->setSpacing(2);
    QLabel *titulo = new QLabel("Na Escuta?", cabecalho);
    titulo->setObjectName("tituloMarca");
    QLabel *subtitulo = new QLabel("Processamento digital de áudio", cabecalho);
    subtitulo->setObjectName("subtituloMarca");
    QLabel *curso = new QLabel("INF112 · Programação II", cabecalho);
    curso->setObjectName("cursoMarca");
    titulos->addWidget(titulo);
    titulos->addWidget(subtitulo);
    titulos->addWidget(curso);
    linhaMarca->addLayout(titulos, 1);
    estrutura->addWidget(cabecalho);

    QScrollArea *areaRolagem = new QScrollArea(raiz);
    areaRolagem->setObjectName("areaRolagem");
    areaRolagem->setWidgetResizable(true);
    areaRolagem->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    areaRolagem->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *central = new QWidget(areaRolagem);
    central->setObjectName("conteudoPrincipal");
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(22, 18, 22, 22);
    layout->setSpacing(16);
    areaRolagem->setWidget(central);
    estrutura->addWidget(areaRolagem, 1);
    setCentralWidget(raiz);

    // Identidade visual em Qt Widgets; sem alterar o nucleo de processamento.
    setStyleSheet(R"QSS(
        QMainWindow, QWidget#conteudoPrincipal, QWidget#qt_scrollarea_viewport {
            background-color: #101010;
            color: #F5F5F5;
        }
        QFrame#cabecalhoMarca {
            background: #090909;
            border-bottom: 1px solid #343434;
        }
        QLabel#logoMarca { background: transparent; }
        QLabel#tituloMarca { color: #FFFFFF; font-size: 26px; font-weight: bold; }
        QLabel#subtituloMarca { color: #B7B7B7; font-size: 12px; }
        QLabel#cursoMarca { color: #C7C7C7; font-size: 10px; }
        QScrollArea#areaRolagem { border: none; background: #101010; }
        QLabel { color: #E8E8E8; }
        QLabel#textoAjuda { color: #A5A5A5; font-size: 11px; }
        QLabel#estadoAudio { color: #D9D9D9; }
        QGroupBox {
            color: #F0F0F0;
            font-weight: 600;
            background-color: #1E1E1E;
            border: 1px solid #414141;
            border-radius: 12px;
            margin-top: 17px;
            padding: 16px 12px 12px 12px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 14px;
            padding: 0px 7px;
            color: #F5F5F5;
        }
        QLineEdit, QComboBox, QSpinBox, QListWidget {
            color: #F2F2F2;
            background-color: #141414;
            border: 1px solid #4B4B4B;
            border-radius: 7px;
            padding: 7px 9px;
            selection-background-color: #626262;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QListWidget:focus {
            border: 1px solid #C7C7C7;
        }
        QComboBox::drop-down { border: none; width: 25px; }
        QComboBox QAbstractItemView {
            background: #222222; color: #F2F2F2;
            border: 1px solid #505050; selection-background-color: #575757;
        }
        QListWidget::item { padding: 6px 8px; }
        QListWidget::item:selected { background: #575757; border-radius: 4px; }
        QPushButton {
            color: #F0F0F0;
            background: #303030;
            border: 1px solid #505050;
            border-radius: 7px;
            padding: 8px 12px;
            min-height: 20px;
        }
        QPushButton:hover { background: #424242; border-color: #797979; }
        QPushButton:pressed { background: #252525; }
        QPushButton:disabled { color: #888888; background: #292929; border-color: #373737; }
        QPushButton#botaoPrimario {
            color: #111111; background: #E5E5E5; border: 1px solid #E5E5E5;
            font-weight: 700;
        }
        QPushButton#botaoPrimario:hover { background: #FFFFFF; }
        QPushButton#botaoPrimario:disabled {
            color: #858585; background: #333333; border-color: #333333;
        }
        QPushButton#botaoPerigo { color: #D3D3D3; }
        QSlider::groove:horizontal { height: 5px; border-radius: 2px; background: #555555; }
        QSlider::sub-page:horizontal { background: #BDBDBD; border-radius: 2px; }
        QSlider::handle:horizontal {
            background: #F5F5F5; border: 2px solid #B5B5B5;
            width: 14px; margin: -6px 0; border-radius: 8px;
        }
        QScrollBar:vertical { background: #101010; width: 12px; margin: 0; }
        QScrollBar::handle:vertical { background: #626262; border-radius: 5px; min-height: 24px; margin: 2px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QStatusBar { background: #090909; color: #C5C5C5; border-top: 1px solid #343434; }
        QToolTip { color: #F3F3F3; background: #303030; border: 1px solid #666666; }
    )QSS");

    // ----- 01. Selecao do WAV -----
    QGroupBox *grupoEntrada = new QGroupBox("01  ·  Arquivo de entrada", central);
    QVBoxLayout *layoutEntrada = new QVBoxLayout(grupoEntrada);
    QLabel *ajudaEntrada = new QLabel("Selecione um arquivo WAV para carregar e processar.", grupoEntrada);
    ajudaEntrada->setObjectName("textoAjuda");
    layoutEntrada->addWidget(ajudaEntrada);
    QHBoxLayout *linhaEntrada = new QHBoxLayout();
    campoEntrada = new QLineEdit(grupoEntrada);
    campoEntrada->setPlaceholderText("Selecione um arquivo .wav");
    QPushButton *botaoProcurarEntrada = new QPushButton("Procurar...", grupoEntrada);
    QPushButton *botaoCarregar = new QPushButton("Carregar WAV", grupoEntrada);
    botaoCarregar->setObjectName("botaoPrimario");
    linhaEntrada->addWidget(campoEntrada, 1);
    linhaEntrada->addWidget(botaoProcurarEntrada);
    linhaEntrada->addWidget(botaoCarregar);
    layoutEntrada->addLayout(linhaEntrada);
    layout->addWidget(grupoEntrada);

    // Informacoes persistentes, atualizadas ao carregar/processar o WAV.
    QGroupBox *grupoInfo = new QGroupBox("Informações do áudio", central);
    QVBoxLayout *layoutInfo = new QVBoxLayout(grupoInfo);
    rotuloInfoAudio = new QLabel("Nenhum arquivo WAV carregado.", grupoInfo);
    rotuloInfoAudio->setWordWrap(true);
    rotuloEstadoAudio = new QLabel("Estado: aguardando carregamento.", grupoInfo);
    rotuloEstadoAudio->setWordWrap(true);
    rotuloEstadoAudio->setObjectName("estadoAudio");
    layoutInfo->addWidget(rotuloInfoAudio);
    layoutInfo->addWidget(rotuloEstadoAudio);
    layout->addWidget(grupoInfo);


    // ----- Grupo: escolha do filtro -----
    QGroupBox *grupoFiltros = new QGroupBox("03  ·  Configurar filtros", central);
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
    botaoAdicionar->setObjectName("botaoPrimario");
    layoutFiltros->addWidget(botaoAdicionar);
    layout->addWidget(grupoFiltros);

    // ----- Grupo: cadeia de filtros -----
    QGroupBox *grupoCadeia = new QGroupBox("04  ·  Cadeia de filtros (ordem de cima para baixo)", central);
    QVBoxLayout *layoutCadeia = new QVBoxLayout(grupoCadeia);

    listaCadeia = new QListWidget(grupoCadeia);
    listaCadeia->setMinimumHeight(120);
    listaCadeia->setMaximumHeight(160);
    layoutCadeia->addWidget(listaCadeia);

    // Duas linhas evitam que os cinco botoes estourem a largura da janela.
    QGridLayout *botoesCadeia = new QGridLayout();
    botaoSubir   = new QPushButton("Subir", grupoCadeia);
    botaoDescer  = new QPushButton("Descer", grupoCadeia);
    botaoEditar  = new QPushButton("Editar parâmetros", grupoCadeia);
    botaoRemover = new QPushButton("Remover", grupoCadeia);
    botaoLimpar  = new QPushButton("Limpar tudo", grupoCadeia);
    botaoRemover->setObjectName("botaoPerigo");
    botaoLimpar->setObjectName("botaoPerigo");
    botoesCadeia->addWidget(botaoSubir, 0, 0);
    botoesCadeia->addWidget(botaoDescer, 0, 1);
    botoesCadeia->addWidget(botaoEditar, 0, 2);
    botoesCadeia->addWidget(botaoRemover, 1, 0);
    botoesCadeia->addWidget(botaoLimpar, 1, 1, 1, 2);
    layoutCadeia->addLayout(botoesCadeia);

    layout->addWidget(grupoCadeia);

    // ----- 05. Processamento e saida juntos, no ponto natural do fluxo -----
    QGroupBox *grupoFinalizacao = new QGroupBox("05  ·  Processar e exportar", central);
    QVBoxLayout *layoutFinalizacao = new QVBoxLayout(grupoFinalizacao);
    botaoProcessar = new QPushButton("Processar áudio", grupoFinalizacao);
    botaoProcessar->setObjectName("botaoPrimario");
    layoutFinalizacao->addWidget(botaoProcessar);
    QLabel *rotuloSaida = new QLabel("Destino do WAV processado:", grupoFinalizacao);
    layoutFinalizacao->addWidget(rotuloSaida);
    QHBoxLayout *linhaSaida = new QHBoxLayout();
    campoSaida = new QLineEdit(grupoFinalizacao);
    campoSaida->setPlaceholderText("Escolha o arquivo de saída .wav");
    QPushButton *botaoProcurarSaida = new QPushButton("Procurar...", grupoFinalizacao);
    botaoExportar = new QPushButton("Exportar WAV", grupoFinalizacao);
    botaoExportar->setObjectName("botaoPrimario");
    linhaSaida->addWidget(campoSaida, 1);
    linhaSaida->addWidget(botaoProcurarSaida);
    linhaSaida->addWidget(botaoExportar);
    layoutFinalizacao->addLayout(linhaSaida);
    layout->addWidget(grupoFinalizacao);

    // Visualizacao resumida da amplitude antes e depois dos filtros.
    QGroupBox *grupoOndas = new QGroupBox("06  ·  Ondas do áudio (amplitude)", central);
    QVBoxLayout *layoutOndas = new QVBoxLayout(grupoOndas);
    layoutOndas->addWidget(new QLabel("Original", grupoOndas));
    ondaOriginal = new VisualizacaoOnda(QColor(225, 225, 225), grupoOndas);
    layoutOndas->addWidget(ondaOriginal);
    layoutOndas->addWidget(new QLabel("Processado", grupoOndas));
    ondaProcessada = new VisualizacaoOnda(QColor(156, 156, 156), grupoOndas);
    layoutOndas->addWidget(ondaProcessada);
    layout->addWidget(grupoOndas);

    // Comparação auditiva: sempre toca um único áudio por vez.
    QGroupBox *grupoReproducao = new QGroupBox("02  ·  Ouvir e comparar", central);
    QVBoxLayout *layoutReproducao = new QVBoxLayout(grupoReproducao);
    QHBoxLayout *linhaReproducao = new QHBoxLayout();
    botaoOuvirOriginal = new QPushButton("Ouvir original", grupoReproducao);
    botaoOuvirProcessado = new QPushButton("Ouvir processado", grupoReproducao);
    botaoPararAudio = new QPushButton("Parar áudio", grupoReproducao);
    linhaReproducao->addWidget(botaoOuvirOriginal);
    linhaReproducao->addWidget(botaoOuvirProcessado);
    linhaReproducao->addWidget(botaoPararAudio);
    layoutReproducao->addLayout(linhaReproducao);
    QLabel *ajudaReproducao = new QLabel(
        "Carregue um WAV para ouvir o original. Processe-o para comparar o resultado.",
        grupoReproducao);
    ajudaReproducao->setWordWrap(true);
    layoutReproducao->addWidget(ajudaReproducao);
    // Coloca a reproducao imediatamente abaixo das informacoes do WAV.
    layout->insertWidget(2, grupoReproducao);

#if GUI_COM_AUDIO
    reprodutor = new ReprodutorAudio(this);
    reprodutor->aoAlterarEstado([this]() { atualizarEstado(); });
    connect(botaoOuvirOriginal, &QPushButton::clicked, this, [this]() {
        QString erro;
        if (!reprodutor->tocarOriginal(erro))
            QMessageBox::warning(this, "Reprodução indisponível", erro);
        else
            statusBar()->showMessage("Reproduzindo áudio original.");
        atualizarEstado();
    });
    connect(botaoOuvirProcessado, &QPushButton::clicked, this, [this]() {
        QString erro;
        if (!audioCarregado || !audioProcessado || !reprodutor->tocarProcessado(*audioCarregado, erro))
            QMessageBox::warning(this, "Reprodução indisponível",
                                 erro.isEmpty() ? "Processe o áudio antes de reproduzi-lo." : erro);
        else
            statusBar()->showMessage("Reproduzindo áudio processado.");
        atualizarEstado();
    });
    connect(botaoPararAudio, &QPushButton::clicked, this, [this]() {
        reprodutor->parar();
        statusBar()->showMessage("Reprodução interrompida.");
        atualizarEstado();
    });
#else
    grupoReproducao->setToolTip("Instale o módulo Qt Multimedia para habilitar a reprodução.");
    QLabel *avisoMultimedia = new QLabel(
        "Reprodução desativada: módulo Qt Multimedia não encontrado. "
        "Instale-o na mesma versão do Qt e configure o projeto novamente.", grupoReproducao);
    avisoMultimedia->setWordWrap(true);
    layoutReproducao->addWidget(avisoMultimedia);
#endif

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
#if GUI_COM_AUDIO
        reprodutor->limpar();
#endif
        audioCarregado.reset();
        arquivoCarregado = false;
        audioProcessado = false;
        ondaOriginal->limpar();
        ondaProcessada->limpar();
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
#if GUI_COM_AUDIO
    reprodutor->limpar();
#endif
    // Assim, uma tentativa frustrada de carregamento nao deixa
    // um resultado antigo disponivel para processamento/exportacao.
    audioCarregado.reset();
    arquivoCarregado = false;
    audioProcessado = false;
    ondaOriginal->limpar();
    ondaProcessada->limpar();
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

#if GUI_COM_AUDIO
        // Guarda o som original antes que os filtros alterem as amostras em memória.
        reprodutor->definirOriginal(*novoAudio);
#endif
        audioCarregado = std::move(novoAudio);
        ondaOriginal->definirAudio(*audioCarregado);
        ondaProcessada->limpar();
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
        // So autoriza sobrescrever quando o usuario confirmou na caixa de dialogo.
        GerenciadorArquivoWav::exportar(caminho.toStdString(), *audioCarregado, info.exists());
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

#if GUI_COM_AUDIO
    reprodutor->limpar();
#endif
    audioCarregado.reset();
    arquivoCarregado = false;
    audioProcessado = false;
    ondaOriginal->limpar();
    ondaProcessada->limpar();
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
    botaoExportar->setEnabled(arquivoCarregado && audioCarregado && audioProcessado);
#if GUI_COM_AUDIO
    botaoOuvirOriginal->setEnabled(reprodutor && reprodutor->temOriginal());
    botaoOuvirProcessado->setEnabled(arquivoCarregado && audioCarregado && audioProcessado);
    botaoPararAudio->setEnabled(reprodutor && reprodutor->tocando());
#else
    botaoOuvirOriginal->setEnabled(false);
    botaoOuvirProcessado->setEnabled(false);
    botaoPararAudio->setEnabled(false);
#endif
    botaoExportar->setToolTip(audioProcessado
        ? QString() : "Processe um arquivo WAV antes de exportar.");

    if (!arquivoCarregado || !audioCarregado) {
        rotuloInfoAudio->setText("Nenhum arquivo WAV carregado.");
        rotuloEstadoAudio->setText("Estado: aguardando carregamento.");
    } else {
        const uint16_t canais = audioCarregado->get_num_canais();
        const uint32_t frequencia = audioCarregado->get_sample_rate();
        const uint16_t bits = audioCarregado->get_bits_per_sample();
        const uint32_t amostras = audioCarregado->get_num_amostras();

        QString duracao = "indisponível";
        if (canais > 0 && frequencia > 0) {
            const double segundos = static_cast<double>(amostras)
                / (static_cast<double>(canais) * frequencia);
            duracao = QString::number(segundos, 'f', 2) + " s";
        }

        rotuloInfoAudio->setText(
            QString("Canais: %1  |  Taxa: %2 Hz  |  Resolução: %3 bits  |  "
                    "Amostras: %4  |  Duração: %5")
                .arg(canais).arg(frequencia).arg(bits).arg(amostras).arg(duracao));
        rotuloEstadoAudio->setText(audioProcessado
            ? "Estado: áudio processado; pronto para exportar."
            : "Estado: WAV carregado; aguardando processamento.");
    }

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

#if GUI_COM_AUDIO
    reprodutor->parar();  // não manter ponteiros de amostras durante processamento in-place
#endif
    definirOcupado(true);
    audioProcessado = false;

    try {
        montada.cadeia->processar(*audioCarregado);
        ondaProcessada->definirAudio(*audioCarregado);
        audioProcessado = true;
        statusBar()->showMessage(
            QString("Áudio processado com %1 filtro(s); %2 ignorado(s). Pronto para exportar.")
                .arg(montada.cadeia->get_quantidade()).arg(montada.ignorados.size()));
    } catch (const std::exception &erro) {
        // O núcleo pode ter alterado parte das amostras antes de lançar a exceção.
        // Invalida o áudio para impedir a exportação de um resultado incompleto.
#if GUI_COM_AUDIO
        reprodutor->limpar();
#endif
        audioCarregado.reset();
        arquivoCarregado = false;
        ondaOriginal->limpar();
        ondaProcessada->limpar();
        QMessageBox::critical(this, "Erro ao processar áudio",
                              QString::fromUtf8(erro.what()) +
                              "\nCarregue o arquivo novamente antes de processar.");
    } catch (...) {
#if GUI_COM_AUDIO
        reprodutor->limpar();
#endif
        audioCarregado.reset();
        arquivoCarregado = false;
        ondaOriginal->limpar();
        ondaProcessada->limpar();
        QMessageBox::critical(this, "Erro ao processar áudio",
                              "Falha inesperada. Carregue o arquivo novamente antes de processar.");
    }

    definirOcupado(false);
}
