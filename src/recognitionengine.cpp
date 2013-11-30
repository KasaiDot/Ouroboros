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

#include "recognitionengine.h"

#include <QFileInfo>
#include <QPair>

using namespace Recognition;
RecognitionEngine Recognition_Engine;

//********************************************************************************
// Credits to erengy(creator of Taiga) for the recognition class
//********************************************************************************

RecognitionEngine::RecognitionEngine()
{
    CommonCharTable = ",_ -+;.&|~";

    AudioKeywords = QString(RECOGNITION_KEYWORD_AUDIO).split(",").toVector();
    VideoKeywords = QString(RECOGNITION_KEYWORD_VIDEO).split(",").toVector();
    ExtraKeywords = QString(RECOGNITION_KEYWORD_EXTRA).split(",").toVector();
    ExtraUnsafeKeywords = QString(RECOGNITION_KEYWORD_EXTRA_UNSAFE).split(",").toVector();
    VersionKeywords = QString(RECOGNITION_KEYWORD_VERSION).split(",").toVector();
    EpisodeKeywords = QString(RECOGNITION_KEYWORD_EPISODE).split(",").toVector();
    EpisodePrefixes = QString(RECOGNITION_KEYWORD_EPISODE_PREFIX).split(",").toVector();
}

/******************************************************
 * Constructs an anime episode from the given title
 ******************************************************/
bool RecognitionEngine::ExamineTitle(QString Title, Anime::AnimeEpisode &Episode, bool ExamineInside, bool ExamineOutside, bool ExamineNumber, bool CheckExtras)
{

    //Clear old data
    Episode.Clear();
    if(Title.isEmpty()) return false;

    //Remove zero width space characters
    Title.replace("\u200B","");

    // Retrieve file name from full path and remove the extension
    if (Title.length() > 2 && Title.at(1) == ':' && Title.at(2) == '\\')
    {
        QFileInfo TitleFile(Title);
        Episode.Folder = TitleFile.path();
        Title = TitleFile.completeBaseName();
        Episode.FileExtension = TitleFile.suffix();
    }
    Episode.File = Title;

    //****************************************************************************************
    // Here we are, entering the world of tokens. Each token has four properties:
    // * Content: Self-explanatory
    // * Encloser: Can be empty or one of these characters: [](){}
    // * Seperator: The most common non-alphanumeric character - usually a space
    //   or an underscore
    // * Virgin: All tokens start out as virgins but lose this property when some
    //   keyword within is recognized and erased.
    //****************************************************************************************

    QVector<Token> Tokens;
    Tokens.reserve(4);
    TokenizeTitle(Title,QString("[](){}"), Tokens);
    if(Tokens.empty()) return false;

    // Examine tokens
    for (int i = 0; i < Tokens.size(); ++i)
    {
        if (Tokens[i].isEnclosed())
        {
            if (ExamineInside) ExamineToken(Tokens[i], Episode, CheckExtras);
        } else {
            if (ExamineOutside) ExamineToken(Tokens[i], Episode, CheckExtras);
        }
    }

    // Tidy up tokens
    for (int i = 1; i < Tokens.size() - 1; i++)
    {
        // Combine remaining Tokens that are enclosed with parentheses - this is
        // especially useful for titles that include a year value and some other cases
        if (Tokens[i - 1].Virgin == false || Tokens[i].Virgin == false ||
                Tokens[i - 1].isEnclosed() || Tokens[i].Encloser != '(' ||
                Tokens[i - 1].Content.length() < 2)
        {
            continue;
        }

        Tokens[i - 1].Content += "(" + Tokens[i].Content + ")";
        if (!Tokens[i + 1].isEnclosed())
        {
            Tokens[i - 1].Content += Tokens[i + 1].Content;
            if (Tokens[i - 1].Separator == '\0')
                Tokens[i - 1].Separator = Tokens[i + 1].Separator;
            Tokens.erase(Tokens.begin() + i + 1);
        }
        Tokens.erase(Tokens.begin() + i);
        i = 0;
    }

    for (int i = 0; i < Tokens.size(); i++)
    {
        // Trim Separator character from each side of the token
        if(Tokens[i].Content.at(0) == Tokens[i].Separator || '\0')
            Tokens[i].Content.remove(0,1);
        if(Tokens[i].Content.at(Tokens[i].Content.length() - 1) == Tokens[i].Separator || '\0')
            Tokens[i].Content.remove(Tokens[i].Content.length() - 1,1);

        // Tokens that are too short are now garbage, so we take them out
        QRegExp Number("\\d*");
        if (Tokens[i].Content.length() < 2 && !Tokens[i].Content.contains(Number))
        {
            Tokens.erase(Tokens.begin() + i);
            i--;
        }
    }

    //************************************************************************
    // Now we apply some logic to decide on the title and the group name
    //************************************************************************

    int GroupIndex = -1;
    int TitleIndex = -1;
    QVector<int> GroupVector, TitleVector;
    for (int i = 0; i < Tokens.size(); i++)
    {
        if (Tokens[i].isEnclosed())
        {
            GroupVector.push_back(i);
        } else {
            TitleVector.push_back(i);
        }
    }

    // Choose the first free token as the title, if there is one
    if (TitleVector.size() > 0)
    {
        TitleIndex = TitleVector[0];
        TitleVector.erase(TitleVector.begin());
    } else {
        // Choose the second enclosed token as the title, if there is more than one
        // (it is more probable that the group name comes before the title)
        if (GroupVector.size() > 1)
        {
            TitleIndex = GroupVector[1];
            GroupVector.erase(GroupVector.begin() + 1);
            // Choose the first enclosed token as the title, if there is one
            // (which means that there is no group name available)
        } else if (GroupVector.size() > 0) {
            TitleIndex = GroupVector[0];
            GroupVector.erase(GroupVector.begin());
        }
    }

    // Choose the first enclosed virgin token as the group name
    for (int i = 0; i < GroupVector.size(); i++)
    {
        // Here we assume that group names are never enclosed with other keywords
        if (Tokens[GroupVector[i]].Virgin) {
            GroupIndex = GroupVector[i];
            GroupVector.erase(GroupVector.begin() + i);
            break;
        }
    }
    // Group name might not be enclosed at all
    // This is a special case for THORA releases, where the group name is at the end
    if (GroupIndex == -1)
    {
        if (TitleVector.size() > 0)
        {
            GroupIndex = TitleVector.back();
            TitleVector.erase(TitleVector.end() - 1);
        }
    }

    // Do we have a title?
    if (TitleIndex > -1)
    {
        // Replace the separator with a space character
        Tokens[TitleIndex].Content.replace(Tokens[TitleIndex].Separator, ' ');
        // Do some clean-up
        if(Tokens[TitleIndex].Content.at(0) == " " && Tokens[TitleIndex].Content.at(1) == "-")
            Tokens[TitleIndex].Content.remove(0,2);

        if(Tokens[TitleIndex].Content.at(Tokens[TitleIndex].Content.length() - 2) == " " && Tokens[TitleIndex].Content.at(Tokens[TitleIndex].Content.length() - 1) == "-")
            Tokens[TitleIndex].Content.remove(Tokens[TitleIndex].Content.length() - 2,2);
        // Set the title
        Title = Tokens[TitleIndex].Content;
        Tokens[TitleIndex].Content.clear();
    }

    // Do we have a group name?
    if (GroupIndex > -1)
    {
        // We don't want to lose any character if the token is enclosed, because
        // those characters can be a part of the group name itself
        if (!Tokens[GroupIndex].isEnclosed())
        {
            // Replace the separator with a space character
            Tokens[GroupIndex].Content.replace(Tokens[GroupIndex].separator, ' ');
            // Do some clean-up
            if(Tokens[GroupIndex].Content.at(0) == " " && Tokens[GroupIndex].Content.at(1) == "-")
                Tokens[GroupIndex].Content.remove(0,2);

            if(Tokens[GroupIndex].Content.at(Tokens[GroupIndex].Content.length() - 2) == " " && Tokens[GroupIndex].Content.at(Tokens[GroupIndex].Content.length() - 1) == "-")
                Tokens[GroupIndex].Content.remove(Tokens[GroupIndex].Content.length() - 2,2);
        }
        // Set the group name
        Episode.Group = Tokens[GroupIndex].Content;
        // We don't clear token content here, becuse we'll be checking it for
        // episode number later on
    }

}

/*********************************
 * Converts title into a tokens
 ********************************/
size_t RecognitionEngine::TokenizeTitle(const QString &Title, const QString &Delimiters, QVector<Token> &Tokens)
{
    size_t IndexBegin = Title.toStdWString().find_first_not_of(Delimiters.toStdWString());
    while (IndexBegin != std::wstring::npos)
    {
        size_t IndexEnd = Title.toStdWString().find_first_of(Delimiters.toStdWString(), IndexBegin + 1);
        Tokens.resize(Tokens.size() + 1);
        if (IndexEnd == std::wstring::npos)
        {
            Tokens.back().Content = Title.mid(IndexBegin);
            break;
        } else {
            Tokens.back().Content = Title.mid(IndexBegin, IndexEnd - IndexBegin);
            if (IndexBegin > 0) Tokens.back().Encloser = Title.at(IndexBegin - 1).toLatin1();
            IndexBegin = Title.toStdWString().find_first_not_of(Delimiters.toStdWString(), IndexEnd + 1);
        }
    }
    return Tokens.size();
}

void RecognitionEngine::ExamineToken(Token &CurToken, Anime::AnimeEpisode &Episode, bool CompareExtras)
{
    // Split into words. The most common non-alphanumeric character is the
    // separator.
    CurToken.Separator = GetMostCommonCharacter(CurToken.Content).toLatin1();
    QVector<QString> Words = CurToken.Content.split(CurToken.Encloser).toVector();

    // Revert if there are words that are too short. This prevents splitting some
    // group names (e.g. "m.3.3.w") and keywords (e.g. "H.264").
    if (CurToken.isEnclosed())
    {
        for (int i = 0; i < Words.size(); ++i)
        {
            if (Words[i].length() == 1)
            {
                Words.clear();
                Words.push_back(CurToken.Content);
                break;
            }
        }
    }

    // Compare with keywords
    for (int i = 0; i < Words.size(); ++i)
    {
        Words[i] = Words[i].trimmed();
        if (Words[i].isEmpty()) continue;

        // Checksum
        if (Episode.Checksum.isEmpty() && Words[i].length() == 8 && IsHex(Words[i])) {
            Episode.Checksum = Words[i];
            RemoveWordFromToken(CurToken,Words[i],false);
            // Video resolution
        } else if (Episode.Resolution.isEmpty() && IsResolution(Words[i])) {
            Episode.Resolution = Words[i];
            RemoveWordFromToken(CurToken,Words[i],false);
            // Video info
        } else if (VideoKeywords.contains(Words[i])) {
            Episode.VideoType.append(Words[i]);
            RemoveWordFromToken(CurToken,Words[i],true);
            // Audio info
        } else if (Words[i], AudioKeywords.contains(Words[i])) {
            Episode.AudioType.append(Words[i]);
            RemoveWordFromToken(CurToken,Words[i],true);
            // Version
        } else if (Episode.Version.isEmpty() && VersionKeywords.contains(Words[i])) {
            Episode.Version.push_back(Words[i].at(Words[i].length() - 1));
            RemoveWordFromToken(CurToken,Words[i],true);
            // Extras
        } else if (CompareExtras && ExtraKeywords.contains(Words[i])) {
            Episode.Extras.append(Words[i]);
            RemoveWordFromToken(CurToken,Words[i],true);
        } else if (CompareExtras && ExtraUnsafeKeywords.contains(Words[i])) {
            Episode.Extras.append(Words[i]);
            if (CurToken.isEnclosed())
                RemoveWordFromToken(CurToken,Words[i],true);
        }
    }
}

/***********************************************************************
 * Gets the most common non alpha-numeric character  from the string
 **********************************************************************/
int RecognitionEngine::GetCommonCharIndex(QChar C)
{
    for (int i = 0; i < CommonCharTable.size(); ++i)
        if (CommonCharTable.at(i) == C)
            return i;
    return -1;
}

QChar RecognitionEngine::GetMostCommonCharacter(QString &String)
{
    QVector<QPair<QChar,int>> CharCount;
    int CharIndex = -1;

    //iterate through all characters
    for(int i = 0; i < String.size(); ++i)
    {
        QChar Char = String.at(i);
        if(!IsAlphanumeric(Char))
        {
            CharIndex = GetCommonCharIndex(Char);
            for (auto It = CharCount.begin(); It != CharCount.end(); ++It)
            {
                if (It->first == String.at(i))
                {
                    It->second++;
                    CharIndex = -1;
                    break;
                }
            }
            if (CharIndex > -1) {
                CharCount.push_back(qMakePair(String.at(i), 1));
            }
        }
    }

    //get the most common char from char count based on the commonchartable order
    CharIndex = 0;
    for (auto It = CharCount.begin(); It != CharCount.end(); ++It)
    {
        if (It->second * 1.8f >= CharCount.at(CharIndex).second && GetCommonCharIndex(It->first) < GetCommonCharIndex(CharCount.at(CharIndex).first))
        {
            CharIndex = It - CharCount.begin();
        }
    }
    return CharCount.empty() ? '\0' : CharCount.at(CharIndex).first;
}

//*****************************************************************************
Token::Token() :
    Encloser('\0'),
    Separator('\0'),
    Virgin(true)
{

}
