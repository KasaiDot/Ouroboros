#include "updateavailiabledialog.h"
#include "ui_updateavailiabledialog.h"

UpdateAvailiableDialog::UpdateAvailiableDialog(bool AppUpdate, bool UpdaterUpdate, QString AppChangeLog,QString UpdaterChangeLog) :
    ui(new Ui::UpdateAvailiableDialog)
{
    ui->setupUi(this);
    this->AppUpdate = AppUpdate;
    this->UpdaterUpdate = UpdaterUpdate;
    this->AppChangeLog = AppChangeLog;
    this->UpdaterChangeLog = UpdaterChangeLog;

    //hide items if no update available
    if(!AppUpdate)
    {
        ui->listWidget->item(0)->setHidden(true);
    } else {
        QString AppTitle = "<b>Ouroboros</b>";
        ui->ChangeLog->append(AppTitle);
        ui->ChangeLog->append(AppChangeLog);
        ui->ChangeLog->append("\n");
    }
    if(!UpdaterUpdate)
    {
        ui->listWidget->item(1)->setHidden(true);
    } else {
        QString UpdaterTitle = "<b>Updater</b>";
        ui->ChangeLog->append(UpdaterTitle);
        ui->ChangeLog->append(UpdaterChangeLog);
        ui->ChangeLog->append("\n");
    }
}

UpdateAvailiableDialog::~UpdateAvailiableDialog()
{
    delete ui;
}

//OK pressed
void UpdateAvailiableDialog::on_buttonBox_accepted()
{
    QListWidgetItem *AppItem = ui->listWidget->item(0);
    QListWidgetItem *UpdateItem = ui->listWidget->item(1);
    //check if any updates are selected
    bool UpdateApp = (!AppItem->isHidden() && AppItem->checkState() == Qt::Checked);
    bool UpdateUpdater = (!UpdateItem->isHidden() && UpdateItem->checkState() == Qt::Checked);

    emit Update(UpdateApp,UpdateUpdater);
}
