/*
**  Ouroboros, Hummingbird.me Desktop App
**  Copyright (C) 2013, Mikunj Varsani
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

#include "apimanager.h"
#include "queuemanager.h"
#include "filemanager.h"
#include "settings.h"
#include "animedatabase.h"
#include "guimanager.h"

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

    //set progress button colors
    SetButtonColor(ui->ProgressBar_BackgroundColorButton,Settings.ProgressDelegate.Background);
    SetButtonColor(ui->ProgressBar_OutlineColorButton,Settings.ProgressDelegate.Outline);
    SetButtonColor(ui->ProgressBar_TextColorButton,Settings.ProgressDelegate.TextColor);
    SetButtonColor(ui->ProgressBar_CurrentlyWatchingColorButton,Settings.ProgressDelegate.CurrentlyWatching);
    SetButtonColor(ui->ProgressBar_CompletedColorButton,Settings.ProgressDelegate.Completed);
    SetButtonColor(ui->ProgressBar_OnHoldColorButton,Settings.ProgressDelegate.OnHold);
    SetButtonColor(ui->ProgressBar_DroppedColorButton,Settings.ProgressDelegate.Dropped);

}

/***********************************
 * Changes the color of the button
 **********************************/
void Dialog_Settings::ChangeColor(QPushButton *ColorButton)
{
    QColorDialog ColorDialog;

    QColor SelectedColor = ColorDialog.getColor(ColorButton->palette().background().color(),this);
    if(!SelectedColor.isValid()) return;

    SetButtonColor(ColorButton,SelectedColor);
}

/***********************************************************
 * Sets the color of the button
 * where ColorVar is a variable that you set the color of
 ***********************************************************/
void Dialog_Settings::SetButtonColor(QPushButton *ColorButton,QColor &ColorVar)
{
    QPalette NewPalette = ColorButton->palette();
    NewPalette.setColor(QPalette::Background,ColorVar);

    ColorButton->setPalette(NewPalette);

    //Draw color
    ColorButton->setStyleSheet("border:1px solid #000000; background-color:" + ColorVar.name() + ";");
}

/******************************
 * Saves button color globally
 ******************************/
void Dialog_Settings::SaveButtonColor(QPushButton *ColorButton, QColor &ColorVar)
{
    QColor ButtonColor = ColorButton->palette().background().color();
    ColorVar = ButtonColor;
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

    //save progress colors
    SaveButtonColor(ui->ProgressBar_TextColorButton,Settings.ProgressDelegate.TextColor);
    SaveButtonColor(ui->ProgressBar_OutlineColorButton,Settings.ProgressDelegate.Outline);
    SaveButtonColor(ui->ProgressBar_BackgroundColorButton,Settings.ProgressDelegate.Background);
    SaveButtonColor(ui->ProgressBar_CurrentlyWatchingColorButton,Settings.ProgressDelegate.CurrentlyWatching);
    SaveButtonColor(ui->ProgressBar_CompletedColorButton,Settings.ProgressDelegate.Completed);
    SaveButtonColor(ui->ProgressBar_OnHoldColorButton,Settings.ProgressDelegate.OnHold);
    SaveButtonColor(ui->ProgressBar_DroppedColorButton,Settings.ProgressDelegate.Dropped);

    //save application settings

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

/************************** Progress bar color buttons clicked ************************************/
void Dialog_Settings::on_ProgressBar_TextColorButton_clicked() { ChangeColor(ui->ProgressBar_TextColorButton); }
void Dialog_Settings::on_ProgressBar_OutlineColorButton_clicked() { ChangeColor(ui->ProgressBar_OutlineColorButton); }
void Dialog_Settings::on_ProgressBar_BackgroundColorButton_clicked() { ChangeColor(ui->ProgressBar_BackgroundColorButton); }
void Dialog_Settings::on_ProgressBar_CurrentlyWatchingColorButton_clicked() { ChangeColor(ui->ProgressBar_CurrentlyWatchingColorButton); }
void Dialog_Settings::on_ProgressBar_CompletedColorButton_clicked() { ChangeColor(ui->ProgressBar_CompletedColorButton); }
void Dialog_Settings::on_ProgressBar_OnHoldColorButton_clicked() { ChangeColor(ui->ProgressBar_OnHoldColorButton); }
void Dialog_Settings::on_ProgressBar_DroppedColorButton_clicked() { ChangeColor(ui->ProgressBar_DroppedColorButton); }
