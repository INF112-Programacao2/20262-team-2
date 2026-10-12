#include <QApplication>
#include "interfacegrafica.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    InterfaceGrafica janela;
    janela.show();

    return app.exec();
}
