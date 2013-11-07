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

#include "apimanager.h"
#include "queuemanager.h"
#include "filemanager.h"

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
}

Dialog_Settings::~Dialog_Settings()
{
    delete ui;
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
    //Check if username and password fields are not empty and that the username and passwords are different from current user
    QString Username = ui->UsernameField->text();
    QString Password = ui->PasswordField->text();
    if(!(Username.isEmpty() && Password.isEmpty()) && (Username.toLower() != CurrentUser.GetUsername()) && (Password.toUtf8().toBase64() != CurrentUser.GetBase64Password()))
    {
        CurrentUser.SetUserDetails(ui->UsernameField->text(),ui->PasswordField->text().toUtf8().toBase64());
        Queue_Manager.AuthenticateUser();
        Queue_Manager.GetAnimeLibrary();
        File_Manager.SaveUserInformation();
    }
}
