#include "util.h"
#include "settings.h"

#include <QRegularExpression>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QWidget>

namespace Util {

bool DimLightsSupported()
{
    return true;
}

void SetAlwaysOnTop(WId wid, bool ontop)
{
    QWidget *window = QWidget::find(wid);
    if(window == nullptr || window->windowFlags().testFlag(Qt::WindowStaysOnTopHint) == ontop)
        return;
    window->setWindowFlag(Qt::WindowStaysOnTopHint, ontop);
    window->show(); // changing window flags hides the window
}

QString SettingsLocation()
{
    // saves to ~/Library/Preferences/${SETTINGS_FILE}.ini
    return QString("%0/%1.ini").arg(
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation),
            SETTINGS_FILE);
}

bool IsValidFile(QString path)
{
    static const QRegularExpression rx("^\\.{1,2}|/", QRegularExpression::CaseInsensitiveOption); // relative path, network location, drive
    return rx.match(path).hasMatch();
}

bool IsValidLocation(QString loc)
{
    static const QRegularExpression rx("^([a-z]{2,}://|\\.{1,2}|/)", QRegularExpression::CaseInsensitiveOption); // url, relative path, absolute path
    return rx.match(loc).hasMatch();
}

void ShowInFolder(QString path, QString)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QString MonospaceFont()
{
    return "Menlo";
}

}
