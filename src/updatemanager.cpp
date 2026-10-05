#include "updatemanager.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVersionNumber>
#include <QUrl>

#if defined(Q_OS_WIN)
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryFile>
#include <windows.h>
#endif

#include "bakaengine.h"
#include "util.h"

UpdateManager::UpdateManager(QObject *parent) :
    QObject(parent),
    baka(static_cast<BakaEngine*>(parent)),
    manager(new QNetworkAccessManager(this)),
    busy(false)
{

}

UpdateManager::~UpdateManager()
{
    delete manager;
}

QString UpdateManager::AssetName()
{
#if defined(Q_OS_WIN)
    return "Baka-MPlayer-windows-x86_64.zip";
#elif defined(Q_OS_MACOS)
    return "Baka-MPlayer-macos-arm64.dmg";
#else
    return QString(); // Linux builds come from distribution packages
#endif
}

bool UpdateManager::IsUpdateAvailable() const
{
    QVersionNumber latest = QVersionNumber::fromString(info.value("version"));
    return !latest.isNull() &&
           latest > QVersionNumber::fromString(BAKA_MPLAYER_VERSION);
}

bool UpdateManager::CanInstall() const
{
#if defined(Q_OS_WIN)
    return IsUpdateAvailable() && !info.value("asset").isEmpty();
#else
    return false;
#endif
}

bool UpdateManager::CheckForUpdates()
{
    if(busy)
        return false;
    busy = true;
    emit messageSignal(tr("Checking for updates..."));
    emit progressSignal(0);
    QNetworkRequest request{QUrl{Util::VersionFileUrl()}};
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setHeader(QNetworkRequest::UserAgentHeader, QString("Baka-MPlayer/%0").arg(BAKA_MPLAYER_VERSION));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::downloadProgress,
            [=](qint64 received, qint64 total)
            {
                if(total > 0)
                    emit progressSignal((int)(99.0*received/total));
            });

    connect(reply, &QNetworkReply::finished,
            [=]
            {
                if(reply->error())
                    emit messageSignal(reply->errorString());
                else
                {
                    QJsonObject release = QJsonDocument::fromJson(reply->readAll()).object();
                    QString version = release["tag_name"].toString();
                    if(version.startsWith('v'))
                        version.remove(0, 1);
                    info["version"] = version;
                    info["bugfixes"] = release["body"].toString();
                    info["url"] = release["html_url"].toString(Util::DownloadFileUrl());
                    info.remove("asset");
                    for(const QJsonValue &asset : release["assets"].toArray())
                        if(asset["name"].toString() == AssetName())
                            info["asset"] = asset["browser_download_url"].toString();
                }
                busy = false;
                emit progressSignal(100);
                emit checkFinished(IsUpdateAvailable());
                reply->deleteLater();
            });
    return true;
}

#if defined(Q_OS_WIN)

bool UpdateManager::InstallUpdate()
{
    if(busy || !CanInstall())
        return false;

    // the update replaces the files next to the executable
    const QString appDir = QCoreApplication::applicationDirPath();
    QTemporaryFile probe(appDir + "/.baka-update-XXXXXX");
    if(!probe.open())
    {
        emit messageSignal(tr("Cannot write to %0; please download the update manually.").arg(QDir::toNativeSeparators(appDir)));
        return false;
    }
    probe.close();

    const QString dir = QDir::temp().filePath("baka-mplayer-update");
    QDir(dir).removeRecursively();
    QDir().mkpath(dir);
    const QString zip = QDir(dir).filePath(AssetName());
    QFile *file = new QFile(zip, this);
    if(!file->open(QFile::WriteOnly | QFile::Truncate))
    {
        emit messageSignal(tr("Could not write %0").arg(QDir::toNativeSeparators(zip)));
        delete file;
        return false;
    }

    busy = true;
    emit messageSignal(tr("Downloading update..."));
    emit progressSignal(0);
    QNetworkRequest request{QUrl{info["asset"]}};
    request.setHeader(QNetworkRequest::UserAgentHeader, QString("Baka-MPlayer/%0").arg(BAKA_MPLAYER_VERSION));
    QNetworkReply *reply = manager->get(request); // GitHub redirects downloads; Qt follows safe redirects

    connect(reply, &QNetworkReply::readyRead,
            [=] { file->write(reply->readAll()); });
    connect(reply, &QNetworkReply::downloadProgress,
            [=](qint64 received, qint64 total)
            {
                if(total > 0)
                    emit progressSignal((int)(90.0*received/total));
            });
    connect(reply, &QNetworkReply::finished,
            [=]
            {
                file->write(reply->readAll());
                file->close();
                file->deleteLater();
                busy = false;
                if(reply->error())
                    emit messageSignal(reply->errorString());
                else
                    ExtractAndRestart(zip, dir);
                reply->deleteLater();
            });
    return true;
}

void UpdateManager::ExtractAndRestart(const QString &zip, const QString &dir)
{
    emit messageSignal(tr("Extracting..."));
    const QString files = QDir(dir).filePath("files");
    QDir().mkpath(files);
    // tar.exe ships with Windows 10 and later and understands zip archives
    QProcess tar;
    tar.start("tar", {"-xf", zip, "-C", files});
    const QString exeName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    if(!tar.waitForFinished(120000) || tar.exitCode() != 0 ||
       !QFile::exists(QDir(files).filePath(exeName)))
    {
        emit messageSignal(tr("Could not extract the update; please download it manually."));
        return;
    }

    // the running executable can't be replaced, so a small script waits for
    // us to exit, copies the new files over and starts the new version
    const QString appDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    const QString script = QDir(dir).filePath("update.bat");
    QFile bat(script);
    if(!bat.open(QFile::WriteOnly | QFile::Truncate))
    {
        emit messageSignal(tr("Could not create the updater script."));
        return;
    }
    const QString pid = QString::number(QCoreApplication::applicationPid());
    bat.write(QString(
        "@echo off\r\n"
        ":wait\r\n"
        "tasklist /FI \"PID eq %1\" 2>NUL | find \" %1 \" >NUL && (timeout /t 1 /nobreak >NUL & goto wait)\r\n"
        "robocopy \"%2\" \"%3\" /E /NFL /NDL /NJH /NJS /NP >NUL\r\n"
        "start \"\" \"%3\\%5\"\r\n"
        "rmdir /S /Q \"%2\"\r\n"
        "del \"%4\"\r\n"
        "(goto) 2>NUL & del \"%~f0\"\r\n").arg(
            pid,
            QDir::toNativeSeparators(files),
            appDir,
            QDir::toNativeSeparators(zip),
            exeName).toLocal8Bit());
    bat.close();

    QProcess updater;
    updater.setProgram("cmd.exe");
    updater.setArguments({"/C", QDir::toNativeSeparators(script)});
    updater.setCreateProcessArgumentsModifier(
        [](QProcess::CreateProcessArguments *args) { args->flags |= CREATE_NO_WINDOW; });
    if(!updater.startDetached())
    {
        emit messageSignal(tr("Could not start the updater."));
        return;
    }
    emit progressSignal(100);
    emit messageSignal(tr("Restarting..."));
    baka->Quit();
}

#else

bool UpdateManager::InstallUpdate()
{
    return false;
}

#endif
