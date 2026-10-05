#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>
#include <QString>

class BakaEngine;

class UpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit UpdateManager(QObject *parent = nullptr);
    ~UpdateManager();

    // keys: version, bugfixes, url (release page), asset (this platform's download)
    const QMap<QString, QString> &getInfo() { return info; }

    // a newer version than the running one has been found
    bool IsUpdateAvailable() const;
    // the update can be downloaded and installed automatically (Windows)
    bool CanInstall() const;

    // file name of this platform's download in a GitHub release
    static QString AssetName();

public slots:
    bool CheckForUpdates();
    bool InstallUpdate();

signals:
    void progressSignal(int percent);
    void messageSignal(QString msg);
    void checkFinished(bool updateAvailable);

private:
#if defined(Q_OS_WIN)
    void ExtractAndRestart(const QString &zip, const QString &dir);
#endif

    BakaEngine *baka;

    QNetworkAccessManager *manager;
    QMap<QString, QString> info;
    bool busy;
};

#endif // UPDATEMANAGER_H
