#pragma once
#include <QMainWindow>

class QLineEdit;
class QComboBox;
class QSlider;
class QLabel;
class QListWidget;
class QPushButton;

class InterfaceGrafica : public QMainWindow {
    Q_OBJECT

public:
    explicit InterfaceGrafica(QWidget *parent = nullptr);

private slots:
    void escolherEntrada();
    void escolherSaida();
    void carregar();
    void exportar();

    void aoMudarFiltro(int indice);
    void aoAdicionarFiltro();
    void aoRemoverFiltro();
    void aoLimparCadeia();

private:
    void atualizarValor(int i);
    void moverFiltro(int delta);
    void atualizarBotoesCadeia();

    // Arquivos
    QLineEdit *campoEntrada;
    QLineEdit *campoSaida;

    // Escolha de filtro e parâmetros (até 2 por filtro)
    QComboBox *comboFiltros;
    QLabel *rotuloParam[2];
    QSlider *slider[2];
    QLabel *valorParam[2];

    // Cadeia de filtros
    QListWidget *listaCadeia;
    QPushButton *botaoSubir;
    QPushButton *botaoDescer;
    QPushButton *botaoRemover;
    QPushButton *botaoLimpar;
};
