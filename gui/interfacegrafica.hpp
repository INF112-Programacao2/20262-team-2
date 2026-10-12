#pragma once
#include <QMainWindow>

class QLineEdit;
class QComboBox;
class QSlider;
class QLabel;

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

private:
    void atualizarValor(int i);

    // Arquivos
    QLineEdit *campoEntrada;
    QLineEdit *campoSaida;

    // Escolha de filtro e parâmetros (até 2 por filtro)
    QComboBox *comboFiltros;
    QLabel *rotuloParam[2];
    QSlider *slider[2];
    QLabel *valorParam[2];
};
