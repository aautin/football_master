// Entry point for Calculator application without .ui (programmatic UI)
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.show();
    window.setWindowTitle("Football Master");

    return app.exec();
}
