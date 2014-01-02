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

#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include <QApplication>
#include <QSharedMemory>
#include <QStringList>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int &argc, char *argv[], const QString UniqueKey);

    bool AlreadyExists() const { return bAlreadyExists; }
    bool isMasterApp() const { return !AlreadyExists(); }
    bool SendInstanceMessage(const QString &Message);

public slots:
    void CheckForMessage();

signals:
    void MessageAvailable(const QStringList& Messages);

private:
    bool bAlreadyExists;
    QSharedMemory SharedMemory;

};
#endif // SINGLE_APPLICATION_H
