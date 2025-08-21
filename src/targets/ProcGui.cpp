#include "ProcGuiHandler.hpp"
#include <QApplication>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ProcGuiHandler window;
    window.show();
    return app.exec();
}