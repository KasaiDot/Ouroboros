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

#ifndef RECOGNITIONENGINE_H
#define RECOGNITIONENGINE_H

//*********************************************************************************************************
#define RECOGNITION_KEYWORD_AUDIO             "2CH, 5.1CH, 5.1, AAC, AC3, DTS, DTS5.1, DTS-ES, DUALAUDIO, DUAL AUDIO, FLAC, MP3, OGG, TRUEHD5.1, VORBIS"
#define RECOGNITION_KEYWORD_EXTRA             "ASS, BATCH, BD, BLURAY, BLU-RAY, COMPLETE, DIRECTOR'S CUT, DVD, DVD5, DVD9, DVD-R2J, DVDRIP, ENG, ENGLISH, HARDSUB, PS3, R2DVD, R2J, R2JDVD, RAW, REMASTERED, SOFTSUB, SUBBED, SUB, UNCENSORED, UNCUT, VOSTFR, WEBCAST, WIDESCREEN, WS"
#define RECOGNITION_KEYWORD_EXTRA_UNSAFE      "END, FINAL, OAV, ONA, OVA"
#define RECOGNITION_KEYWORD_VERSION           "V0, V2, V3, V4"
#define RECOGNITION_KEYWORD_EXTENSION         "MKV, AVI, MP4, OGM, RM, RMVB, WMV, DIVX, MOV, FLV, MPG, 3GP"
#define RECOGNITION_KEYWORD_VIDEO             "8BIT, 10BIT, AVI, DIVX, H264, H.264, HD, HDTV, HI10P, HQ, LQ, RMVB, SD, TS, VFR, WMV, X264, X.264, XVID"
#define RECOGNITION_KEYWORD_EPISODE           "EPISODE, EP., EP, VOLUME, VOL., VOL, EPS., EPS"
#define RECOGNITION_KEYWORD_EPISODE_PREFIX    "EP., EP, E, VOL., VOL, EPS."
//*********************************************************************************************************

#include <QString>
#include <QChar>
#include <QVector>

#include "library/animedatabase.h"
#include "library/animeentity.h"
#include "library/animeepisode.h"
#include "ouroboros/common.h"

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

    bool IsAlphanumeric(QChar C);
    bool IsAlphanumeric(const QString &String);

    //checks if string or character is hex
    bool IsHex(QChar c);
    bool IsHex(QString String);
    //*****************************************************************************
    //resoulution functions
    bool IsResolution(QString &String);

    //Returns video resoultion
    int TranslateResolution(QString& String, bool ReturnValidity);
    //*****************************************************************************
    //Token functions
    inline void RemoveWordFromToken(Token &CurToken,QString Word,bool CaseInsensitive);

    bool IsNumeric(QChar c);
    bool IsNumeric(QString String);

    bool IsCountingWord(QString String);

    bool IsEpisodeFormat(QString &str, Anime::AnimeEpisode &episode,char separator = ' ');
    bool ValidateEpisodeNumber(Anime::AnimeEpisode &Episode);

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
