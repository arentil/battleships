#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <QTextBrowser>
#include <QTimer>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow main_window;
    main_window.show();


    return app.exec();
}
