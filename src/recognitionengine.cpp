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

    //Trim all the keywords
    AudioKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_AUDIO).split(",")).toVector();
    VideoKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_VIDEO).split(",")).toVector();
    ExtraKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_EXTRA).split(",")).toVector();
    ExtraUnsafeKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_EXTRA_UNSAFE).split(",")).toVector();
    VersionKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_VERSION).split(",")).toVector();
    EpisodeKeywords = TrimStrings(QString(RECOGNITION_KEYWORD_EPISODE).split(",")).toVector();
    EpisodePrefixes = TrimStrings(QString(RECOGNITION_KEYWORD_EPISODE_PREFIX).split(",")).toVector();


}
/***********************************************************
 * Removes all spaces and uneeded character from string
 **********************************************************/
QStringList RecognitionEngine::TrimStrings(QStringList StringList)
{
    QStringList TrimmedList;
    foreach(QString String,StringList)
    {
        TrimmedList << String.trimmed();
    }

    return TrimmedList;
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
    Title.replace(QString::fromUtf8("\u200B"),"");

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
        QString TrimChar = QString(Tokens[i].Separator) + QString::fromLatin1('\0');
        Trim(Tokens[i].Content, TrimChar);

        // Tokens that are too short are now garbage, so we take them out
        if (Tokens[i].Content.length() < 2 && !IsNumeric(Tokens[i].Content))
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
        QString TrimChar = " -";
        Trim(Tokens[TitleIndex].Content, TrimChar);
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
            Tokens[GroupIndex].Content.replace(Tokens[GroupIndex].Separator, ' ');
            // Do some clean-up
            QString TrimChar = " -";
            Trim(Tokens[GroupIndex].Content, TrimChar);
        }
        // Set the group name
        Episode.Group = Tokens[GroupIndex].Content;
        // We don't clear token content here, becuse we'll be checking it for
        // episode number later on
    }

    //************************************************************
    // Get episode number and version, if available
    //*************************************************************
    if (ExamineNumber)
    {
        // Check remaining Tokens first
        for (int i = 0; i < Tokens.size(); i++)
        {
            if (IsEpisodeFormat(Tokens[i].Content, Episode, Tokens[i].Separator))
            {
                Tokens[i].Virgin = false;
                break;
            }
        }

        // Check title
        if (Episode.Number.isEmpty())
        {
            // Split into words
            QVector<QString> Words;
            Words.reserve(4);
            Tokenize(Title, QString(" "), Words);
            if (Words.isEmpty()) return false;
            Title.clear();
            int NumberIndex = -1;

            // Check for Episode number format, starting with the second word
            for (int i = 1; i < Words.size(); i++)
            {
                if (IsEpisodeFormat(Words[i], Episode)) {
                    NumberIndex = i;
                    break;
                }
            }

            // Set the first valid numeric token as Episode number
            if (Episode.Number.isEmpty()) {
                for (int i = 0; i < Tokens.size(); i++)
                {
                    if (IsNumeric(Tokens[i].Content))
                    {
                        Episode.Number = Tokens[i].Content;
                        if (ValidateEpisodeNumber(Episode))
                        {
                            Tokens[i].Virgin = false;
                            break;
                        }
                    }
                }
            }

            // Set the lastmost number that follows a '-'
            if (Episode.Number.isEmpty() && Words.size() > 2)
            {
                for (int i = Words.size() - 2; i > 0; i--)
                {
                    if (Words[i] == "-" && IsNumeric(Words[i + 1]))
                    {
                        Episode.Number = Words[i + 1];
                        if (ValidateEpisodeNumber(Episode))
                        {
                            NumberIndex = i + 1;
                            break;
                        }
                    }
                }
            }

            // Set the lastmost number as a last resort
            if (Episode.Number.isEmpty())
            {
                for (int i = Words.size() - 1; i > 0; i--)
                {
                    if (IsNumeric(Words[i]))
                    {
                        Episode.Number = Words[i];
                        if (ValidateEpisodeNumber(Episode))
                        {
                            // Discard and give up if movie or season number (Episode numbers cannot precede them)
                            if (i > 1 && (Words[i - 1] == "Season" || Words[i - 1] == "Movie") && !IsCountingWord(Words[i - 2]))
                            {
                                Episode.Number.clear();
                                NumberIndex = -1;
                                break;
                            }
                            NumberIndex = i;
                            break;
                        }
                    }
                }
            }

            // Build title and name
            for (int i = 0; i < Words.size(); i++)
            {
                if (i < NumberIndex)
                {
                    if (i == NumberIndex - 1 && EpisodeKeywords.contains(Words[i])) continue;
                    Title.append(Words[i]);
                } else if (i > NumberIndex) {
                    Episode.Name.append(Words[i]);
                }
            }

            // Clean up
            TrimRight(Title, QString(" -"));
            TrimLeft(Episode.Name, QString(" -"));
            if (Episode.Name.startsWith("'") && Episode.Name.endsWith("'"))
                Trim(Episode.Name, "'");
            TrimLeft(Episode.Name, QString("\u300C")); // Japanese left quotation mark
            TrimRight(Episode.Name, QString("\u300D")); // Japanese right quotation mark
        }
    }
    //*********************************************
    // Check if the group token is still a virgin
    //********************************************
    if (GroupIndex > -1 && !Tokens[GroupIndex].Virgin)
    {
        Episode.Group.clear();
        for ( int i = 0; i < Tokens.size(); i++)
        {
            // Set the first available virgin token as group name
            if (!Tokens[i].Content.isEmpty() && Tokens[i].Virgin)
            {
                Episode.Group = Tokens[i].Content;
                break;
            }
        }
    }
    // Set Episode name as group name if necessary
    if (Episode.Group.isEmpty() && Episode.Name.length() > 2)
    {
        if (Episode.Name.startsWith("(") && Episode.Name.endsWith(")"))
        {
            Episode.Group = Episode.Name.mid(1, Episode.Name.length() - 2);
            Episode.Name.clear();
        }
    }

    // Examine remaining Tokens once more
    for (int i = 0; i < Tokens.size(); i++)
    {
        if (!Tokens[i].Content.isEmpty()) {
            ExamineToken(Tokens[i], Episode, true);
        }
    }

    //*************************************************************
    // Set the final title, hopefully name of the anime
     //************************************************************
    Episode.Title = Title;
    Episode.CleanTitle = Title;
    CleanTitle(Episode.CleanTitle);

    return !Title.isEmpty();
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

size_t RecognitionEngine::Tokenize(QString &String, QString Delimiters, QVector<QString> &Tokens)
{
    Tokens.clear();
    size_t IndexBegin = String.toStdWString().find_first_not_of(Delimiters.toStdWString());
    while (IndexBegin != std::wstring::npos)
    {
        size_t IndexEnd = String.toStdWString().find_first_of(Delimiters.toStdWString(), IndexBegin + 1);
        if (IndexEnd == std::wstring::npos)
        {
            Tokens.push_back(String.mid(IndexBegin));
            break;
        } else {
            Tokens.push_back(String.mid(IndexBegin, IndexEnd - IndexBegin));
            IndexBegin = String.toStdWString().find_first_not_of(Delimiters.toStdWString(), IndexEnd + 1);
        }
    }
    return Tokens.size();
}

/***************************************************
 * Rids title of uneccesary words and punctuation
 ***************************************************/
void RecognitionEngine::CleanTitle(QString &Title)
{
    if (Title.isEmpty()) return;
    EraseUnnecessary(Title);
    TransliterateSpecial(Title);
    ErasePunctuation(Title, true);
}

void RecognitionEngine::EraseUnnecessary(QString &String)
{
  EraseLeft(String, "the ", true);
  String.replace(" the ", " ");
  String.replace("episode "," ");
  String.replace(" ep.", " ");
  String.replace(" specials", " special");
}
void RecognitionEngine::TransliterateSpecial(QString &String)
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

void RecognitionEngine::ErasePunctuation(QString &String, bool KeepTrailing)
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

void RecognitionEngine::EraseLeft(QString &String1, const QString String2, bool CaseInsensitive)
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

void RecognitionEngine::EraseRight(QString &String1, const QString String2, bool CaseInsensitive)
{
  if (String1.length() < String2.length()) return;
  if (CaseInsensitive) {
    if (!std::equal(String2.begin(), String2.end(), String1.end() - String2.length(), &IsCharsEqual)) return;
  } else {
    if (!std::equal(String2.begin(), String2.end(), String1.end() - String2.length())) return;
  }
  String1.resize(String1.length() - String2.length());
}

/**************************************************
 * Checks whether the string is in episode format
 *************************************************/
bool RecognitionEngine::IsEpisodeFormat(QString &String, Anime::AnimeEpisode &Episode, char Separator)
{
    int NumStart, i, j;

    // Find first number
    for (NumStart = 0; NumStart < String.length() && !IsNumeric(String.at(NumStart)); NumStart++);
    if (NumStart == String.length()) return false;

    // Check for Episode prefix
    if (NumStart > 0)
        if (!EpisodePrefixes.contains(String.mid(0, NumStart))) return false;

    for (i = NumStart + 1; i < String.length(); i++)
    {
        if (!IsNumeric(String.at(i)))
        {
            // *#-#*
            if (String.at(i) == '-' || String.at(i) == '&')
            {
                if (i == String.length() - 1 || !IsNumeric(String.at(i + 1))) return false;
                for (j = i + 1; j < String.length() && IsNumeric(String.at(j)); j++);
                Episode.Number = String.mid(NumStart, j - NumStart);
                // *#-#*v#
                if (j < String.length() - 1 && (String.at(j) == 'v' || String.at(j) =='V'))
                {
                    NumStart = i + 1;
                    continue;
                } else {
                    return true;
                }

                // v#
            } else if (String.at(i) == 'v' || String.at(i) == 'V') {
                if (Episode.Number.isEmpty()) {
                    if (i == String.length() - 1 || !IsNumeric(String.at(i + 1))) return false;
                    Episode.Number = String.mid(NumStart, i - NumStart);
                }
                Episode.Version = String.mid(i + 1);
                return true;

                // *# of #*
            } else if (String.at(i) == Separator) {
                if (String.length() < i + 5) return false;
                if (String.at(i + 1) == 'o' &&
                        String.at(i + 2) == 'f' &&
                        String.at(i + 3) == Separator) {
                    Episode.Number = String.mid(NumStart, i - NumStart);
                    return true;
                }

                // *#x#*
            } else if (String.at(i) == 'x') {
                if (i == String.length() - 1 || !IsNumeric(String.at(i + 1))) return false;
                for (j = i + 1; j < String.length() && IsNumeric(String.at(j)); j++);
                Episode.Number = String.mid(i + 1, j - i - 1);
                if (Episode.Number.toInt() < 100)
                {
                    return true;
                } else {
                    Episode.Number.clear();
                    return false;
                }

                // Japanese counter
            } else if (String.at(i) == QString::fromUtf8("\u8A71").at(0)) {
                Episode.Number = String.mid(NumStart, i - 1);
                return true;
            } else {
                Episode.Number.clear();
                return false;
            }
        }
    }

    // [prefix]#*
    if (NumStart > 0 && Episode.Number.isEmpty()) {
        Episode.Number = String.mid(NumStart, String.length() - NumStart);
        if (!ValidateEpisodeNumber(Episode)) return false;
        return true;
    }

    // *#
    return false;
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





