/*
**  Ouroboros, Hummingbird.me Desktop App
**  Copyright (C) 2014, Mikunj Varsani
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dialog_settings.h"
#include "ui_dialog_settings.h"

#include <QMessageBox>
#include <QPainter>
#include <QColorDialog>

#include "api/apimanager.h"
#include "api/queuemanager.h"
#include "library/animedatabase.h"
#include "manager/filemanager.h"
#include "manager/guimanager.h"
#include "manager/stylemanager.h"
#include "ouroboros/settings.h"

Dialog_Settings::Dialog_Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Settings)
{
    ui->setupUi(this);

    // Set default values
    if(CurrentUser.isValid())
    {
        ui->UsernameField->setText(CurrentUser.GetUsername());
        ui->PasswordField->setText(QString(QByteArray::fromBase64(CurrentUser.GetBase64Password())));
    }

    SetSettings();

    this->setStyleSheet(QString(Style_Manager.GetStyle()));

    //Connect signals and slots
    connect(&Style_Manager,&Manager::StyleManager::StyleChanged,[=](QByteArray NewStyle){
        this->setStyleSheet(QString(NewStyle));
    });
}

Dialog_Settings::~Dialog_Settings()
{
    delete ui;
}

/***********************************
 * Sets all the settings in the ui
 ***********************************/
void Dialog_Settings::SetSettings()
{
    //application settings
    ui->Application_TrayClose->setChecked(Settings.Application.CloseToTray);
    ui->Application_TrayMinimize->setChecked(Settings.Application.MinimizeToTray);

    //recognition
    ui->Recognition_Enabled->setChecked(Settings.Recognition.Enabled);
    ui->Recognition_DelaySpinBox->setValue(Settings.Recognition.Delay);
    ui->Recogntion_MPClose->setChecked(Settings.Recognition.WaitForMPClose);
    ui->Recogntion_MPFocus->setChecked(Settings.Recognition.MPFocus);
    ui->Recogntion_NotifyRecognised->setChecked(Settings.Recognition.NotifyEpisodeRecognised);
    ui->Recogntion_NotifyNotRecognised->setChecked(Settings.Recognition.NotifyEpisodeNotRecognised);
}

/************************
 * Test button clicked
 ***********************/
void Dialog_Settings::on_AccountTestButton_clicked()
{
    //Set current user details
    CurrentUserCopy.SetUserDetails(CurrentUser.GetUsername(),CurrentUser.GetBase64Password());
    CurrentUserCopy.SetAuthenticated(CurrentUser.isAuthenticated());
    CurrentUserCopy.SetAuthKey(CurrentUser.GetAuthKey());

    CurrentUser.SetUserDetails(ui->UsernameField->text(),ui->PasswordField->text().toUtf8().toBase64());

    int Code = Api_Manager.Authenticate();

    if(Code == Manager::ApiManager::Api_InvalidCredentials)
    {
        QMessageBox::critical(this,"Invalid Credentials","Please check that your username (not email) and password are correct",QMessageBox::Ok);

    } else if(Code == Manager::ApiManager::Api_Success) {

        QMessageBox::information(this,"Correct Credentials","Successfully Authenticated",QMessageBox::Ok);

    } else {

        QMessageBox::critical(this,"Error Occured","Oops! An error occured, please try again later",QMessageBox::Ok);

    }

    //Replace previous details
    CurrentUser.SetUserDetails(CurrentUserCopy.GetUsername(),CurrentUserCopy.GetBase64Password());
    CurrentUser.SetAuthenticated(CurrentUserCopy.isAuthenticated());
    CurrentUser.SetAuthKey(CurrentUserCopy.GetAuthKey());
}

/******************************
 * Ok button on dialog pressed
 ******************************/
void Dialog_Settings::on_buttonBox_accepted()
{
    //Check if username and password fields are not empty and that the username are different from current user
    QString Username = ui->UsernameField->text();
    QString Password = ui->PasswordField->text();
    if(!(Username.isEmpty() && Password.isEmpty()) && ((Username.toLower() != CurrentUser.GetUsername()) || (Password.toUtf8().toBase64() != CurrentUser.GetBase64Password())))
    {
        QString OldUsername = CurrentUser.GetUsername();

        //Save old user anime files if different username
        if(Username.toLower() != CurrentUser.GetUsername())
        {
            File_Manager.SaveUserInformation();
            File_Manager.SaveAnimeDatabase();
            File_Manager.SaveQueue();
        }

        //Set current user as the new user
        CurrentUser.SetUserDetails(ui->UsernameField->text(),ui->PasswordField->text().toUtf8().toBase64());

        File_Manager.SaveUserInformation();

        //if it's a different username clear the database and the views
        //Note: if changing username while another valid user is syncing, it can intefere and may cause the anime to mix up, Be warned
        if(OldUsername != CurrentUser.GetUsername())
        {
            Anime_Database.ClearDatabase();
            GUI_Manager.ClearModel();
            File_Manager.LoadAnimeDatabase();
            GUI_Manager.PopulateModel();
            Queue_Manager.ClearQueue(true);
        } else {
            Queue_Manager.Sync(true);
        }

    }

    //save application settings
    Settings.Application.CloseToTray = ui->Application_TrayClose->isChecked();
    Settings.Application.MinimizeToTray = ui->Application_TrayMinimize->isChecked();

    //recognition
    Settings.Recognition.Enabled = ui->Recognition_Enabled->isChecked();
    Settings.Recognition.Delay = ui->Recognition_DelaySpinBox->value();
    Settings.Recognition.MPFocus = ui->Recogntion_MPFocus->isChecked();
    Settings.Recognition.NotifyEpisodeNotRecognised = ui->Recogntion_NotifyNotRecognised->isChecked();
    Settings.Recognition.NotifyEpisodeRecognised = ui->Recogntion_NotifyRecognised->isChecked();
    Settings.Recognition.WaitForMPClose = ui->Recogntion_MPClose->isChecked();

    Settings.Save();
}

/***********************************
 * Default settings button clicked
 **********************************/
void Dialog_Settings::on_DefaultSettingsButton_clicked()
{
    bool Allowed = (QMessageBox::warning(this,"Restore default settings","Are you sure you want to restore default settings?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes);

    if(Allowed)
    {
        Settings.ResetSettings();
        SetSettings();
    }
}

