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

#include "user.h"

User CurrentUser;

User::User():
    Username(""),
    Password(""),
    AuthKey(""),
    Authenticated(false)
{
}

/*****************************************************************************
 * Sets user details, where user is either the email or username of the user
 * and Base64Password is user password encoded in Base64
 ****************************************************************************/
void User::SetUserDetails(QString Username, QByteArray Base64Password)
{
    this->Username = Username.toLower();
    this->Password = Base64Password;
    SetAuthenticated(false);
    SetAuthKey("");
}

/**********************************************
 * Builds the JSON required for authentication
 **********************************************/
QJsonDocument User::BuildAuthJsonDocument()
{
    QJsonObject Object;
    Object.insert("username",Username);
    Object.insert("password",QString(QByteArray::fromBase64(Password)));

    QJsonDocument Doc(Object);
    return Doc;
}
