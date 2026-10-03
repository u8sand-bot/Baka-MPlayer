#include "updatedialog.h"
#include "ui_updatedialog.h"

#include "bakaengine.h"
#include "updatemanager.h"
#include "util.h"

#include <QDesktopServices>

UpdateDialog::UpdateDialog(BakaEngine *baka, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog),
    baka(baka)
{
    ui->setupUi(this);

    ui->updateButton->setText(tr("&DOWNLOAD"));
    ui->timeRemainingLabel->setVisible(false);

    connect(baka->update, &UpdateManager::progressSignal, this,
            [=](int percent)
            {
                ui->progressBar->setValue(percent);
                if(percent == 100)
                    ShowInfo();
                else
                    ui->progressBar->setVisible(true);
            });

    connect(baka->update, &UpdateManager::messageSignal, this,
            [=](QString msg)
            {
                ui->plainTextEdit->appendPlainText(msg+"\n");
            });

    connect(ui->updateButton, &QPushButton::clicked, this,
            [=]
            {
                QDesktopServices::openUrl(QUrl(baka->update->getInfo().value("url", Util::DownloadFileUrl())));
            });

    connect(ui->cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    if(baka->update->getInfo().empty())
        baka->update->CheckForUpdates();
    else
        ShowInfo();
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::CheckForUpdates(BakaEngine *baka, QWidget *parent)
{
    UpdateDialog dialog(baka, parent);
    dialog.exec();
}

void UpdateDialog::ShowInfo()
{
    auto &info = baka->update->getInfo();
    ui->progressBar->setVisible(false);
    if(info["version"].isEmpty())
    {
        ui->updateLabel->setText(tr("Could not check for updates."));
        ui->updateButton->setEnabled(false);
        return;
    }
    ui->plainTextEdit->setPlainText(info["bugfixes"]);
    if(info["version"].trimmed() == BAKA_MPLAYER_VERSION)
    {
        ui->updateButton->setEnabled(false);
        ui->updateLabel->setText(tr("You have the latest version!"));
    }
    else
    {
        ui->updateLabel->setText(tr("Update Available!\nVersion: %0").arg(info["version"]));
        ui->updateButton->setEnabled(true);
    }
}
