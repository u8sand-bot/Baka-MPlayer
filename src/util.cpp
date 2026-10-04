#include "util.h"

#include <QTime>
#include <QRegularExpression>
#include <QStringList>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>

namespace Util {

QString VersionFileUrl()
{
    return "https://api.github.com/repos/u8sand/Baka-MPlayer/releases/latest";
}

QString DownloadFileUrl()
{
    return "https://github.com/u8sand/Baka-MPlayer/releases/latest";
}

QString FindYtdl()
{
    // GUI apps don't necessarily inherit the user's shell PATH (e.g. apps
    // launched from Finder on macOS don't see Homebrew), so also look in the
    // usual install locations, and finally next to our own executable where
    // the release builds bundle a copy.
    const QString home = QDir::homePath();
    QStringList extraPaths = {
        "/opt/homebrew/bin",
        "/usr/local/bin",
        "/opt/local/bin",
        home + "/.local/bin",
        home + "/bin"
    };
    // pip install --user on macOS
    const QDir userPython(home + "/Library/Python");
    for(const QString &version : userPython.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed))
        extraPaths << userPython.filePath(version + "/bin");
    const QStringList names = {"yt-dlp", "yt-dlp_macos", "youtube-dl"};
    for(const QString &name : names)
    {
        QString path = QStandardPaths::findExecutable(name);
        if(path.isEmpty())
            path = QStandardPaths::findExecutable(name, extraPaths);
        if(!path.isEmpty())
            return path;
    }
    for(const QString &name : names)
    {
        QString path = QStandardPaths::findExecutable(name, {QCoreApplication::applicationDirPath()});
        if(!path.isEmpty())
            return path;
    }
    return QString();
}

bool IsValidUrl(QString url)
{
    static const QRegularExpression rx("^[a-z]{2,}://", QRegularExpression::CaseInsensitiveOption); // url
    return rx.match(url).hasMatch();
}

QString FormatTime(int _time, int _totalTime)
{
    QTime time = QTime::fromMSecsSinceStartOfDay(_time * 1000);
    if(_totalTime >= 3600) // hours
        return time.toString("h:mm:ss");
    if(_totalTime >= 60)   // minutes
        return time.toString("mm:ss");
    return time.toString("0:ss");   // seconds
}

QString FormatRelativeTime(int _time)
{
    QString prefix;
    if(_time < 0)
    {
        prefix = "-";
        _time = -_time;
    }
    else
        prefix = "+";
    QTime time = QTime::fromMSecsSinceStartOfDay(_time * 1000);
    if(_time >= 3600) // hours
        return prefix+time.toString("h:mm:ss");
    if(_time >= 60)   // minutes
        return prefix+time.toString("mm:ss");
    return prefix+time.toString("0:ss");   // seconds
}

QString FormatNumber(int val, int length)
{
    if(length < 10)
        return QString::number(val);
    else if(length < 100)
        return QString("%1").arg(val, 2, 10, QChar('0'));
    else
        return QString("%1").arg(val, 3, 10, QChar('0'));
}

QString FormatNumberWithAmpersand(int val, int length)
{
    if(length < 10)
        return "&"+QString::number(val);
    else if(length < 100)
    {
        if(val < 10)
            return "0&"+QString::number(val);
        return QString("%1").arg(val, 2, 10, QChar('0'));
    }
    else
    {
        if(val < 10)
            return "00&"+QString::number(val);
        return QString("%1").arg(val, 3, 10, QChar('0'));
    }
}

QString HumanSize(qint64 size)
{
    // taken from http://comments.gmane.org/gmane.comp.lib.qt.general/34914
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

QString ShortenPathToParent(const Recent &recent)
{
    const int long_name = 100;
    if(recent.title != QString())
        return QString("%0 (%1)").arg(recent.title, recent.path);
    QString p = QDir::fromNativeSeparators(recent.path);
    int i = p.lastIndexOf('/');
    if(i != -1)
    {
        int j = p.lastIndexOf('/', i-1);
        if(j != -1)
        {
            QString parent = p.mid(j+1, i-j-1),
                    file = p.mid(i+1);
            // todo: smarter trimming
            if(parent.length() > long_name)
            {
                parent.truncate(long_name);
                parent += "..";
            }
            if(file.length() > long_name)
            {
                file.truncate(long_name);
                i = p.lastIndexOf('.');
                file += "..";
                if(i != -1)
                {
                    QString ext = p.mid(i);
                    file.truncate(file.length()-ext.length());
                    file += ext; // add the extension back
                }
            }
            return QDir::toNativeSeparators(parent+"/"+file);
        }
    }
    return QDir::toNativeSeparators(recent.path);
}

QStringList ToNativeSeparators(QStringList list)
{
    QStringList ret;
    for(auto element : list)
    {
        if(Util::IsValidLocation(element))
            ret.push_back(element);
        else
            ret.push_back(QDir::toNativeSeparators(element));
    }
    return ret;
}

QStringList FromNativeSeparators(QStringList list)
{
    QStringList ret;
    for(auto element : list)
        ret.push_back(QDir::fromNativeSeparators(element));
    return ret;
}

int GCD(int u, int v)
{
    int shift;
    if(u == 0) return v;
    if(v == 0) return u;
    for(shift = 0; ((u | v) & 1) == 0; ++shift)
    {
       u >>= 1;
       v >>= 1;
    }
    while((u & 1) == 0)
        u >>= 1;
    do
    {
        while ((v & 1) == 0)
            v >>= 1;
        if (u > v)
        {
            unsigned int t = v;
            v = u;
            u = t;
        }
        v = v - u;
    } while (v != 0);
    return u << shift;
}

QString Ratio(int w, int h)
{
    int gcd=GCD(w, h);
    if(gcd == 0)
        return "0:0";
    return QString("%0:%1").arg(QString::number(w/gcd), QString::number(h/gcd));
}

}
