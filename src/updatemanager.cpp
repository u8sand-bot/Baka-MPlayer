#include "updatemanager.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

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
                }
                busy = false;
                emit progressSignal(100);
                reply->deleteLater();
            });
    return true;
}
