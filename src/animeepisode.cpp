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

#include "animeepisode.h"

using namespace Anime;
AnimeEpisode CurrentEpisode;

AnimeEpisode::AnimeEpisode() :
    Processed(false)
{
    Slug = ANIMEEPISODE_UNKNOWN_SLUG;
}

/**********************
 * Resets the episode
 *********************/
void AnimeEpisode::Clear()
{
    Slug = ANIMEEPISODE_UNKNOWN_SLUG;
    AudioType.clear();
    Checksum.clear();
    Extras.clear();
    File.clear();
    FileExtension.clear();
    Folder.clear();
    Format.clear();
    Group.clear();
    Name.clear();
    Number.clear();
    Resolution.clear();
    Title.clear();
    CleanTitle.clear();
    Version.clear();
    VideoType.clear();
    Year.clear();
    Processed = false;
}

/*********************************
 * Sets the current episode slug
 *********************************/
void AnimeEpisode::Set(QString Slug)
{
    this->Slug = Slug;
    Processed = false;
}

/************************************************************
 * Gets episode number, if format is: low-high, E.g 01-02
 ***********************************************************/
int AnimeEpisode::GetEpisodeNumberHigh() { return GetEpisodeNumberHigh(Number); }
int AnimeEpisode::GetEpisodeNumberHigh(QString Number)
{
    int Value = 1;
    int Pos = Number.lastIndexOf("-");
    if (Pos == Number.length() - 1)
    {
      Value = Number.mid(0, Pos).toInt();
    } else if (Pos > -1) {
      Value = Number.mid(Pos + 1).toInt();
    } else {
      Value = Number.toInt();
    }
    return Value;
}
int AnimeEpisode::GetEpisodeNumberLow() { return GetEpisodeNumberLow(Number); }
int AnimeEpisode::GetEpisodeNumberLow(QString Number)
{
    int Pos = Number.indexOf("-");
    if(Pos < 0)
        return Number.toInt();
    return Number.mid(0,Pos).toInt();
}
