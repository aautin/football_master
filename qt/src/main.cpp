#include <QApplication>
#include <QWidget>
#include "MainWindow.hpp"

int main(int argc, char *argv[], char *envp[]) {
    QApplication app(argc, argv);

    MainWindow window(envp);
    window.show();

    return app.exec();
}
