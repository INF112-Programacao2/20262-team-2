#pragma once
#include <QMainWindow>
#include <QString>
#include <memory>

class DadosAudio;

class QLineEdit;
class QDragEnterEvent;
class QDropEvent;
class QMenu;
class QTimer;
class QComboBox;
class QSlider;
class QLabel;
class QListWidget;
class QPushButton;
class VisualizacaoOnda;
class ReprodutorAudio;

class InterfaceGrafica : public QMainWindow {
    Q_OBJECT

public:
    explicit InterfaceGrafica(QWidget *parent = nullptr);
    ~InterfaceGrafica() override;

protected:
    void dragEnterEvent(QDragEnterEvent *evento) override;
    void dropEvent(QDropEvent *evento) override;

private slots:
    void escolherEntrada();
    void escolherSaida();
    void carregar();
    void exportar();

    void aoMudarFiltro(int indice);
    void aoAdicionarFiltro();
    void aoEditarFiltro();
    void aoRemoverFiltro();
    void aoLimparCadeia();

    void aoProcessar();

private:
    void atualizarValor(int i);
    void moverFiltro(int delta);
    void atualizarBotoesCadeia();
    void atualizarEstado();
    void definirOcupado(bool ocupado);
    bool invalidarAudioAposAlterarCadeia();
    void atualizarMenuRecentes();
    void registrarArquivoRecente(const QString &caminho);
    void atualizarProgressoReproducao();

    // Arquivos
    QLineEdit *campoEntrada;
    QMenu *menuRecentes;
    QString ultimaSaidaAutomatica;
    QLineEdit *campoSaida;
    QLabel *rotuloInfoAudio;
    QLabel *rotuloEstadoAudio;
    QPushButton *botaoExportar;

    // Escolha de filtro e parâmetros (até 2 por filtro)
    QComboBox *comboFiltros;
    QPushButton *botaoAdicionar;
    QLabel *rotuloParam[2];
    QSlider *slider[2];
    QLabel *valorParam[2];

    // Cadeia de filtros
    QListWidget *listaCadeia;
    QPushButton *botaoSubir;
    QPushButton *botaoDescer;
    QPushButton *botaoEditar;
    QPushButton *botaoRemover;
    QPushButton *botaoLimpar;

    // Processamento
    QPushButton *botaoProcessar;
    VisualizacaoOnda *ondaOriginal;
    VisualizacaoOnda *ondaProcessada;
    QPushButton *botaoOuvirOriginal;
    QPushButton *botaoOuvirProcessado;
    QPushButton *botaoPausarAudio;
    QPushButton *botaoContinuarAudio;
    QPushButton *botaoPararAudio;
    QSlider *sliderProgresso;
    QLabel *rotuloTempo;
    QTimer *timerProgresso;
    ReprodutorAudio *reprodutor = nullptr;  // criado apenas com Qt Multimedia disponível
    std::unique_ptr<DadosAudio> audioCarregado;
    std::unique_ptr<DadosAudio> audioResultado;
    bool arquivoCarregado = false;
    bool ocupado = false;
    bool audioProcessado = false;
};
