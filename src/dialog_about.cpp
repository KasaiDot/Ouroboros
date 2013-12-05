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

#include "dialog_about.h"
#include "ui_dialog_about.h"

#include "appinfo.h"

Dialog_About::Dialog_About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_About)
{
    ui->setupUi(this);
    ui->VersionLabel->setText(QString("Version: %1.%2").arg(APP_MAJOR_VERSION).arg(APP_MINOR_VERSION));
    setWindowFlags(this->windowFlags() |= Qt::MSWindowsFixedSizeDialogHint);
}

Dialog_About::~Dialog_About()
{
    delete ui;
}
