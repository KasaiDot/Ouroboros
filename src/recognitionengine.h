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

#ifndef RECOGNITIONENGINE_H
#define RECOGNITIONENGINE_H

#define RECOGNITION_KEYWORD_AUDIO             "2CH, 5.1CH, 5.1, AAC, AC3, DTS, DTS5.1, DTS-ES, DUALAUDIO, DUAL AUDIO, FLAC, MP3, OGG, TRUEHD5.1, VORBIS"
#define RECOGNITION_KEYWORD_EXTRA             "ASS, BATCH, BD, BLURAY, BLU-RAY, COMPLETE, DIRECTOR'S CUT, DVD, DVD5, DVD9, DVD-R2J, DVDRIP, ENG, ENGLISH, HARDSUB, PS3, R2DVD, R2J, R2JDVD, RAW, REMASTERED, SOFTSUB, SUBBED, SUB, UNCENSORED, UNCUT, VOSTFR, WEBCAST, WIDESCREEN, WS"
#define RECOGNITION_KEYWORD_EXTRA_UNSAFE      "END, FINAL, OAV, ONA, OVA"
#define RECOGNITION_KEYWORD_VERSION           "V0, V2, V3, V4"
#define RECOGNITION_KEYWORD_EXTENSION         "MKV, AVI, MP4, OGM, RM, RMVB, WMV, DIVX, MOV, FLV, MPG, 3GP"
#define RECOGNITION_KEYWORD_VIDEO             "8BIT, 10BIT, AVI, DIVX, H264, H.264, HD, HDTV, HI10P, HQ, LQ, RMVB, SD, TS, VFR, WMV, X264, X.264, XVID"
#define RECOGNITION_KEYWORD_EPISODE           "EPISODE, EP., EP, VOLUME, VOL., VOL, EPS., EPS"
#define RECOGNITION_KEYWORD_EPISODE_PREFIX    "EP., EP, E, VOL., VOL, EPS."

#include <QString>
#include <QChar>
#include <QVector>

#include "animedatabase.h"
#include "animeentity.h"
#include "animeepisode.h"
#include "common.h"

/*********************************************************************
 * Credits to erengy(Creator of Taiga) for the recognition class
 ********************************************************************/
namespace Recognition
{

class Token
{
public:
    Token();
    char Encloser;
    char Separator;
    QString Content;
    bool Virgin;
    inline bool isEnclosed() { return (Encloser == '[' || Encloser == '(' || Encloser == '{'); }
};

class RecognitionEngine
{
public:
    RecognitionEngine();
    QStringList TrimStrings(QStringList StringList);


    bool ExamineTitle(QString Title,
                      Anime::AnimeEpisode& Episode,
                      bool ExamineInside = true,
                      bool ExamineOutside = true,
                      bool ExamineNumber = true,
                      bool CheckExtras = true);
    size_t TokenizeTitle(const QString &Title, const QString &Delimiters, QVector<Token> &Tokens);
    void ExamineToken(Token &CurToken, Anime::AnimeEpisode& Episode, bool CompareExtras);
    size_t Tokenize(QString &String, QString Delimiters, QVector<QString> &Tokens);

    //********************************************************* Helper functions ***********************************************************
    //Character functions
    int GetCommonCharIndex(QChar C);
    QChar GetMostCommonCharacter(QString &String);

    inline bool IsAlphanumeric(QChar C) { return (C >= '0' && C <= '9') || (C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z'); }
    bool IsAlphanumeric(const QString &String)
    {
        if (String.isEmpty()) return false;
        for (int i = 0; i < String.length(); ++i)
            if (!IsAlphanumeric(String.at(i))) return false;
        return true;
    }

    //checks if string or character is hex
    bool IsHex(QChar c)
    {
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    }
    bool IsHex(QString String)
    {
        if (String.isEmpty()) return false;
        for (int i = 0; i < String.length(); i++)
            if (!IsHex(String.at(i))) return false;
        return true;
    }
    //*****************************************************************************
    //resoulution functions
    bool IsResolution(QString &String)
    {
        return TranslateResolution(String, true) > 0;
    }

    //Returns video resoultion
    int TranslateResolution(QString& String, bool ReturnValidity)
    {
        // *###x###*
        if (String.length() > 6) {
            int Pos = String.indexOf("x");
            if (Pos > -1)
            {
                for (int i = 0; i < String.length(); i++)
                {
                    if (i != Pos && !String.at(i).isNumber()) return 0;
                }
                return ReturnValidity ? 1 : (String.mid(Pos + 1)).toInt();
            }

            // *###p
        } else if (String.length() > 3) {
            if (String.at(String.length() - 1) == 'p')
            {
                for (int i = 0; i < String.length() - 1; i++)
                {
                    if (!String.at(i).isNumber()) return 0;
                }
                return ReturnValidity ? 1 : (String.mid(0, String.length() - 1)).toInt();
            }
        }

        return 0;
    }
    //*****************************************************************************
    //Token functions
    inline void RemoveWordFromToken(Token &CurToken,QString Word,bool CaseInsensitive)
    {
        if(CaseInsensitive)
            CurToken.Content.replace(Word,"",Qt::CaseInsensitive);
        else
            CurToken.Content.replace(Word,"",Qt::CaseSensitive);

        CurToken.Virgin = false;
    }

    inline bool IsNumeric(QChar c)  { return c >= '0' && c <= '9'; }
    inline bool IsNumeric(QString String)
    {
        if (String.isEmpty()) return false;
        for (int i = 0; i < String.length(); i++)
            if (!IsNumeric(String.at(i))) return false;
        return true;
    }

    inline bool IsCountingWord(QString String)
    {
        if (String.length() > 2) {
            if (String.endsWith("th") || String.endsWith("nd") || String.endsWith("rd") || String.endsWith("st") ||
                    String.endsWith("TH") || String.endsWith("ND") || String.endsWith("RD") || String.endsWith("ST"))
            {
                if (IsNumeric(String.mid(0, String.length() - 2)) ||
                        String == "FIRST" ||
                        String == "SECOND" ||
                        String == "THIRD" ||
                        String == "FOURTH" ||
                        String == "FIFTH")
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool IsEpisodeFormat(QString &str, Anime::AnimeEpisode &episode,char separator = ' ');

    inline void Trim(QString &String, QString TrimChars, bool TrimLeft = true, bool TrimRight = true)
    {
        if (String.isEmpty()) return;
        const size_t IndexBegin = TrimLeft ? String.toStdWString().find_first_not_of(TrimChars.toStdWString()) : 0;
        const size_t IndexEnd = TrimRight ? String.toStdWString().find_last_not_of(TrimChars.toStdWString()) : String.length() - 1;
        if (IndexBegin == std::wstring::npos || IndexEnd == std::wstring::npos) {
            String.clear();
            return;
        }
        if (TrimRight) String.remove(IndexEnd + 1, String.length() - IndexEnd + 1);
        if (TrimLeft) String.remove(0, IndexBegin);
    }

    inline void TrimLeft(QString &String, QString TrimChars)
    {
        Trim(String, TrimChars, true, false);
    }

    inline void TrimRight(QString &String, QString TrimChars)
    {
        Trim(String, TrimChars, false, true);
    }

    inline bool ValidateEpisodeNumber(Anime::AnimeEpisode &Episode) {
        int Number = Episode.Number.toInt();
        if (Number <= 0 || Number > 1000) {
            if (Number > 1950 && Number < 2050) {
                Episode.Year = Episode.Number;
            }
            Episode.Number.clear();
            return false;
        }
        return true;
    }

    //*****************************************************************************

private:
    QString CommonCharTable;

    QVector<QString> AudioKeywords;
    QVector<QString> VideoKeywords;
    QVector<QString> ExtentionKeywords;
    QVector<QString> ExtraKeywords;
    QVector<QString> ExtraUnsafeKeywords;
    QVector<QString> VersionKeywords;
    QVector<QString> EpisodeKeywords;
    QVector<QString> EpisodePrefixes;
};

}

extern Recognition::RecognitionEngine Recognition_Engine;

#endif // RECOGNITIONENGINE_H
