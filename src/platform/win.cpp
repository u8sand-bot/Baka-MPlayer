#include "util.h"

#include <QApplication>
#include <QRegularExpression>
#include <QProcess>
#include <QDir>

#include <windows.h>

#include "settings.h"


namespace Util {

bool DimLightsSupported()
{
    return true;
}

void SetAlwaysOnTop(WId wid, bool ontop)
{
    SetWindowPos((HWND)wid,
                 ontop ? HWND_TOPMOST : HWND_NOTOPMOST,
                 0, 0, 0, 0,
                 SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}

QString SettingsLocation()
{
    // saves to $(application directory)\${SETTINGS_FILE}.ini
    return QString("%0\\%1.ini").arg(QApplication::applicationDirPath(), SETTINGS_FILE);
}

bool IsValidFile(QString path)
{
    static const QRegularExpression rx("^(\\.{1,2}|[a-z]:|\\\\\\\\)", QRegularExpression::CaseInsensitiveOption); // relative path, network location, drive
    return rx.match(path).hasMatch();
}

bool IsValidLocation(QString loc)
{
    static const QRegularExpression rx("^([a-z]{2,}://|\\.{1,2}|[a-z]:|\\\\\\\\)", QRegularExpression::CaseInsensitiveOption); // url, relative path, network location, drive
    return rx.match(loc).hasMatch();
}

void ShowInFolder(QString path, QString file)
{
    QProcess::startDetached("explorer.exe", QStringList{"/select,", QDir::toNativeSeparators(path+file)});
}

QString MonospaceFont()
{
    return "Lucida Console";
}

}
