#include "ui/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QString>
#include <QSurfaceFormat>

#include <locale.h>
#include <cstdio>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <io.h>

// Baka MPlayer is a GUI app, so it has no console of its own. Send its
// output (mpv's log, Qt warnings, crash reports) to the console it was
// started from, or else to baka-mplayer.log next to the executable.
static void SetupWindowsLogging()
{
    if(AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    else
    {
        wchar_t path[MAX_PATH];
        DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
        while(n > 0 && path[n-1] != L'\\')
            --n;
        path[n] = L'\0';
        wcsncat(path, L"baka-mplayer.log", MAX_PATH - n - 1);
        if(_wfreopen(path, L"w", stdout))
            _dup2(_fileno(stdout), _fileno(stderr));
    }
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
}

static LONG WINAPI CrashHandler(EXCEPTION_POINTERS *info)
{
    void *address = info->ExceptionRecord->ExceptionAddress;
    HMODULE module = nullptr;
    char name[MAX_PATH] = "unknown module";
    if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          static_cast<LPCSTR>(address), &module))
        GetModuleFileNameA(module, name, MAX_PATH);
    fprintf(stderr, "[baka]: crashed: exception 0x%08lx at %p (%s+0x%llx)\n",
            info->ExceptionRecord->ExceptionCode, address, name,
            static_cast<unsigned long long>(static_cast<char*>(address) - reinterpret_cast<char*>(module)));
    fflush(stderr);
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

static void MessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    static const char *levels[] = {"debug", "warning", "critical", "fatal", "info"};
    fprintf(stderr, "[qt %s]: %s\n", levels[type < 5 ? type : 0], msg.toLocal8Bit().constData());
    fflush(stderr);
}

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    SetupWindowsLogging();
    SetUnhandledExceptionFilter(CrashHandler);
#endif
    qInstallMessageHandler(MessageHandler);
#if defined(Q_OS_MACOS)
    // mpv needs a modern OpenGL context; macOS defaults to legacy 2.1
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
#endif
    QApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C"); // for mpv
    fprintf(stderr, "[baka]: Baka MPlayer %s, Qt %s, platform %s\n", BAKA_MPLAYER_VERSION,
            qVersion(), QGuiApplication::platformName().toLocal8Bit().constData());

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
