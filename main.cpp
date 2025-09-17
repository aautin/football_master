#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[], char *envp[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.show();

    return app.exec();
}
