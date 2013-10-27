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

#ifndef USER_H
#define USER_H

#include <QObject>

class User
{
public:
    User();
    void SetUserDetails(QString Username, QString Base64Password);
    inline void SetAuthenticated(bool isAuthenticated) { Authenticated = isAuthenticated; }
    inline void SetAuthKey(QString Key) { AuthKey = Key; }


    bool isValid() const { return (!(Username.isEmpty() && Password.isEmpty())); }
    bool isAuthenticated() const { return Authenticated; }
    QString GetUsername() const { return Username; }
    QString GetBase64Password() const { return Password; }
    QString GetAuthKey() const { return AuthKey; }

private:

    QString Username;//Email or Username
    QString Password; //Password in base64 encoding

    QString AuthKey;
    bool Authenticated;

};

extern User CurrentUser;

#endif // USER_H
