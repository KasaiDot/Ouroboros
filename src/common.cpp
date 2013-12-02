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

#include "common.h"

//**************************************** Recogntion title cleaning ******************************************/

/***************************************************
 * Rids title of uneccesary words and punctuation
 ***************************************************/
void CleanTitle(QString &Title)
{
    if (Title.isEmpty()) return;
    EraseUnnecessary(Title);
    TransliterateSpecial(Title);
    ErasePunctuation(Title, true);
}

void TransliterateSpecial(QString &String)
{
  // Character equivalencies
  String.replace("\u00E9", "e"); // small e acute accent
  String.replace("\uFF0F", "/"); // unicode slash
  String.replace("\uFF5E", "~"); // unicode tilde
  String.replace("\u223C", "~"); // unicode tilde 2
  String.replace("\u301C", "~"); // unicode tilde 3
  String.replace("\uFF1F", "?"); // unicode question mark
  String.replace("\uFF01", "!"); // unicode exclamation point
  String.replace("\u00D7", "x"); // multiplication symbol
  String.replace("\u2715", "x"); // multiplication symbol 2
  String.replace("\u2605", ""); //unicode star (useful for anime such as Puella Magi Madoka Magica

  // A few common always-equivalent romanizations
  String.replace("\u014C", "Ou"); // O macron
  String.replace("\u014D", "ou"); // o macron
  String.replace("\u016B", "uu"); // u macron
  String.replace(" wa ", " ha "); // hepburn to wapuro
  String.replace(" e ", " he "); // hepburn to wapuro
  String.replace(" o ", " wo "); // hepburn to wapuro

  // Abbreviations
  String.replace(" & ", " and ");
}

void EraseUnnecessary(QString &String)
{
  EraseLeft(String, "the ", true);
  String.replace(" the ", " ");
  String.replace("episode "," ");
  String.replace(" ep.", " ");
  String.replace(" specials", " special");
}

void ErasePunctuation(QString &String, bool KeepTrailing)
{
  std::wstring StdString = String.toStdWString();
  auto RLast = StdString.rbegin();
  if (KeepTrailing)
  {
    RLast = std::find_if(StdString.rbegin(), StdString.rend(), [](wchar_t c) -> bool {
      return !(c == L'!' || // "Hayate no Gotoku!", "K-ON!"...
               c == L'+' || // "Needless+"
               c == L'\''); // "Gintama'"
    });
  }

  auto It = std::remove_if(StdString.begin(), RLast.base(),[](int c) -> bool {
      // Control codes, white-space and punctuation characters
      if (c <= 255 && !isalnum(c)) return true;
      // Unicode stars, hearts, notes, etc. (0x2000-0x2767)
      if (c > 8192 && c < 10087) return true;
      // Valid character
      return false;
    });

  if (KeepTrailing) std::copy(RLast.base(), StdString.end(), It);
  StdString.resize(StdString.size() - (RLast.base() - It));

  String = QString::fromStdWString(StdString);
}

bool IsCharsEqual(QChar c1,QChar c2)
{
    return c1.toLower() == c2.toLower();
}

//**************************************** Erase Functions *******************************************/
void EraseLeft(QString &String1, const QString String2, bool CaseInsensitive)
{
    std::wstring Str1 = String1.toStdWString();
    std::wstring Str2 = String2.toStdWString();

    if (Str1.length() < Str2.length()) return;
    if (CaseInsensitive) {
        if (!std::equal(Str2.begin(), Str2.end(), Str1.begin(), &IsCharsEqual)) return;
    } else {
        if (!std::equal(Str2.begin(), Str2.end(), Str1.begin())) return;
    }
    Str1.erase(Str1.begin(), Str1.begin() + Str2.length());

    String1 = QString::fromStdWString(Str1);
}

void EraseRight(QString &String1, const QString String2, bool CaseInsensitive)
{
  if (String1.length() < String2.length()) return;
  if (CaseInsensitive) {
    if (!std::equal(String2.begin(), String2.end(), String1.end() - String2.length(), &IsCharsEqual)) return;
  } else {
    if (!std::equal(String2.begin(), String2.end(), String1.end() - String2.length())) return;
  }
  String1.resize(String1.length() - String2.length());
}
