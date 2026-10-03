#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>


namespace Ui {
class UpdateDialog;
}

class BakaEngine;

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(BakaEngine *baka, QWidget *parent = nullptr);
    ~UpdateDialog();

    static void CheckForUpdates(BakaEngine *baka, QWidget *parent = nullptr);

protected slots:
    void ShowInfo();

private:
    Ui::UpdateDialog *ui;
    BakaEngine *baka;
};

#endif // UPDATEDIALOG_H
