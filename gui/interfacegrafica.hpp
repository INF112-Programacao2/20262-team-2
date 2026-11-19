#pragma once
#include <QMainWindow>
#include <memory>

class DadosAudio;

class QLineEdit;
class QComboBox;
class QSlider;
class QLabel;
class QListWidget;
class QPushButton;
class VisualizacaoOnda;

class InterfaceGrafica : public QMainWindow {
    Q_OBJECT

public:
    explicit InterfaceGrafica(QWidget *parent = nullptr);
    ~InterfaceGrafica() override;

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

    // Arquivos
    QLineEdit *campoEntrada;
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
    std::unique_ptr<DadosAudio> audioCarregado;
    bool arquivoCarregado = false;
    bool audioProcessado = false;
};
