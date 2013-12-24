#ifndef UPDATEAVAILIABLEDIALOG_H
#define UPDATEAVAILIABLEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateAvailiableDialog;
}

class UpdateAvailiableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateAvailiableDialog(bool AppUpdate, bool UpdaterUpdate, QString AppChangeLog,QString UpdaterChangeLog);
    ~UpdateAvailiableDialog();

private:
    Ui::UpdateAvailiableDialog *ui;
    bool AppUpdate;
    bool UpdaterUpdate;
    QString AppChangeLog;
    QString UpdaterChangeLog;

signals:
    void Update(bool UpdateApp,bool UpdateUpdater);
private slots:
    void on_buttonBox_accepted();
};

#endif // UPDATEAVAILIABLEDIALOG_H
