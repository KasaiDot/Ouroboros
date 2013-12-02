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

#ifndef COMMON_H
#define COMMON_H

#include <QString>

/*******************************************************************************
 * This class contains functions which are common to two or more classes
 *******************************************************************************/

void CleanTitle(QString &Title);
void EraseUnnecessary(QString &String);
void TransliterateSpecial(QString &String);
void ErasePunctuation(QString &String, bool KeepTrailing);
void EraseLeft(QString &String1, const QString String2, bool CaseInsensitive);
void EraseRight(QString &String1, const QString String2, bool CaseInsensitive);


#endif // COMMON_H
