#include "ui/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QString>
#include <QSurfaceFormat>

#include <locale.h>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    FreeConsole();
#endif
#if defined(Q_OS_MACOS)
    // mpv needs a modern OpenGL context; macOS defaults to legacy 2.1
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
#endif
    QApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C"); // for mpv

    MainWindow w;
    w.show();

    // parse command line
    QStringList args = QApplication::arguments();
    QStringList::iterator arg = args.begin();
    if(++arg != args.end())
        w.Load(*arg);
    else
        w.Load();

    return a.exec();
}
