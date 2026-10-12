#include "interfacegrafica.hpp"
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

InterfaceGrafica::InterfaceGrafica(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Processador de Áudio");
    resize(650, 600);

    QWidget *central = new QWidget(this);   // o pai (this) destrói o filho
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // (os próximos commits adicionam os widgets aqui, neste layout)
    layout->addStretch();

    statusBar()->showMessage("Pronto.");
}
