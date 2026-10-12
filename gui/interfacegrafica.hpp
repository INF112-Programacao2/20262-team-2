#pragma once
#include <QMainWindow>

class QLineEdit;

class InterfaceGrafica : public QMainWindow {
    Q_OBJECT

public:
    explicit InterfaceGrafica(QWidget *parent = nullptr);

private slots:
    void escolherEntrada();
    void escolherSaida();
    void carregar();
    void exportar();

private:
    QLineEdit *campoEntrada;
    QLineEdit *campoSaida;
};
