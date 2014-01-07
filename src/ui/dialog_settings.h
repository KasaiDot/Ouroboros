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

#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

#include <QDialog>
#include <QPushButton>

#include "library/user.h"

namespace Ui {
class Dialog_Settings;
}

class Dialog_Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Settings(QWidget *parent = 0);
    ~Dialog_Settings();

    void SetSettings();

private slots:
    //Test button clicked
    void on_AccountTestButton_clicked();

    //Ok button pressed
    void on_buttonBox_accepted();

    //Cancel pressed
    void on_buttonBox_rejected();

    void on_DefaultSettingsButton_clicked();

    //Theme buttons
    void on_RefreshThemeListButton_clicked();
    void on_RefreshThemeButton_clicked();
    void on_ApplyThemeButton_clicked();


private:
    Ui::Dialog_Settings *ui;

    //used for the test button
    User CurrentUserCopy;

};

#endif // DIALOG_SETTINGS_H
