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

#include "singleapplication.h"

#include <QTimer>
#include <QByteArray>

SingleApplication::SingleApplication(int &argc, char *argv[], const QString UniqueKey) :
    QApplication(argc, argv)
{
    SharedMemory.setKey(UniqueKey);

    // we can create it only if it doesn't exist
    if (SharedMemory.create(5000))
    {
        SharedMemory.lock();
        *(char*)SharedMemory.data() = '\0';
        SharedMemory.unlock();

        bAlreadyExists = false;

        // start checking for messages of other instances.
        QTimer *Timer = new QTimer(this);
        connect(Timer, SIGNAL(timeout()), this, SLOT(CheckForMessage()));
        Timer->start(200);

    } else if (SharedMemory.attach()) {
        bAlreadyExists = true;
    }
}

/***********************************************************
 * Checks for any messages recieved from other instances
 **********************************************************/
void SingleApplication::CheckForMessage()
{
    QStringList Arguments;

    SharedMemory.lock();
    char *From = (char*)SharedMemory.data();

    while(*From != '\0')
    {
        int SizeToRead = int(*From);
        ++From;

        QByteArray ByteArray = QByteArray(From, SizeToRead);
        ByteArray[SizeToRead] = '\0';
        From += SizeToRead;

        Arguments << QString::fromUtf8(ByteArray.constData());
    }

    *(char*)SharedMemory.data() = '\0';
    SharedMemory.unlock();

    if(Arguments.size()) emit MessageAvailable( Arguments );
}

/**************************************
 * Send message to the main instance
 **************************************/
bool SingleApplication::SendInstanceMessage(const QString &message)
{
    //we cannot send message if we are master process
    if (isMasterApp()) return false;

    QByteArray ByteArray;
    ByteArray.append(char(message.size()));
    ByteArray.append(message.toUtf8());
    ByteArray.append('\0');

    SharedMemory.lock();
    char *To = (char*)SharedMemory.data();
    while(*To != '\0')
    {
        int SizeToRead = int(*To);
        To += SizeToRead + 1;
    }

    const char *From = ByteArray.data();
    memcpy(To, From, qMin(SharedMemory.size(), ByteArray.size()));
    SharedMemory.unlock();

    return true;
}
