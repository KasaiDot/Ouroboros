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

#ifndef ANIMEEPISODE_H
#define ANIMEEPISODE_H

//*********************************************************************************************************
#define ANIMEEPISODE_UNKNOWN_SLUG "Unknown Slug"
//*********************************************************************************************************

#include <QString>

namespace Anime
{

class AnimeEpisode
{
public:
    AnimeEpisode();

    void Clear();
    void Set(QString Slug);

    int GetEpisodeNumberHigh();
    int GetEpisodeNumberHigh(QString Number);

    int GetEpisodeNumberLow();
    int GetEpisodeNumberLow(QString Number);

    QString Slug;
    QString File;
    QString FileExtension;
    QString Folder;
    QString Format;
    QString Title;
    QString CleanTitle;
    QString Name;
    QString Group;
    QString Number;
    QString Version;
    QString Resolution;
    QString AudioType;
    QString VideoType;
    QString Checksum;
    QString Extras;
    QString Year;
    bool Processed;

};

}
//global value
extern Anime::AnimeEpisode CurrentEpisode;

#endif // ANIMEEPISODE_H
