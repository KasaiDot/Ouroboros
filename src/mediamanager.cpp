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

#include "mediamanager.h"
#include "animeepisode.h"
#include "animeentity.h"
#include "animedatabase.h"
#include "recognitionengine.h"

using namespace Recognition;
MediaManager Media_Manager;

/*********************************************************************
 * Credits to erengy(Creator of Taiga) for the media class
 ********************************************************************/

MediaManager::MediaManager(QObject *parent) :
    QObject(parent),
    Index(-1),
    OldIndex(-1),
    TitleChanged(false),
    MediaTicker(-1)
{
}

/********************************************
 * Loads in all media information from file
 ********************************************/
void MediaManager::Load()
{

}

/**************************************************************
 * Cleans up title by removing keywords provided in the item
 *************************************************************/
void MediaManager::RemoveKeywords(QString &Title, int MediaIndex)
{
    const int KeywordsSize = MediaList.at(MediaIndex).Keywords.size();
    if(Title.isEmpty() || KeywordsSize <= 0) return;

    //iterate through keywords and replace them with ""
    for(int i = 0; i < KeywordsSize; ++i)
    {
        Title.replace(MediaList.at(MediaIndex).Keywords.at(i),"");
    }
}

/**********************************************
 * Gets the process name of the window handle
 **********************************************/
QString MediaManager::GetProcessNameFromHandle(HWND Handle)
{
    QString ProcessName = "";

    DWORD ProcessId;
    GetWindowThreadProcessId(Handle,&ProcessId);
    HANDLE HProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessId);

    if(HProcess != nullptr)
    {
        HMODULE HModule;
        DWORD CBNeeded;
        wchar_t ProcessNameSize[MAX_PATH];

        if (EnumProcessModules(HProcess, &HModule, sizeof(HModule), &CBNeeded))
        {
            if(GetModuleBaseNameW(HProcess, HModule, ProcessNameSize, sizeof(ProcessNameSize)/sizeof(TCHAR) ) > 0)
            {
                ProcessName = QString::fromUtf16((unsigned short*) ProcessNameSize);
            }
        }

        CloseHandle(HProcess);
    }
    return ProcessName;
}

/****************************************************
 * Retrieves Winodw title from given window handle
 ****************************************************/
QString MediaManager::GetWindowTitleFromHandle(HWND Handle)
{
    if(Handle != nullptr)
    {
        int MaxCount = 200;
        wchar_t Title[MAX_PATH];

        if(GetWindowTextW(Handle,Title,MaxCount) > 0)
        {
            return QString::fromWCharArray(Title);
        }
    }
    return QString("");
}
//***************************************************************************************************************************

/***************************************************************
 * Credit to erengy (creator of taiga) for parts of this code
 ***************************************************************/

/******************************************************************************
 * Loops through all handles and checks if one of them is in the media list
 ******************************************************************************/
int MediaManager::DetectMediaPlayer()
{
    Index = -1;
    bool Recognized = false;

    //Iterate through handles, starting from the top most handle
    HWND Handle = GetWindow(GetTopWindow(nullptr), GW_HWNDFIRST);
    while (Handle != nullptr)
    {
        //Iterate thorugh all our media players
        const int ListSize = MediaList.size();
        for(int i = 0; i < ListSize; ++i)
        {
            Media &Item = MediaList[i];
            if(!Item.Enabled) continue;
            if(!IsWindowVisible(Handle)) continue; //We only want the gui processes
            //Compare the file names
            const int FileListSize = Item.Files.size();
            for(int j = 0; j < FileListSize; ++j)
            {
                QString File = Item.Files.at(j);
                if(File.isEmpty()) continue;
                if((File == GetProcessNameFromHandle(Handle)) && !GetWindowTitleFromHandle(Handle).isEmpty())
                {
                    // Stick with the previously recognized window, if there is one
                    if(!Recognized || Item.WindowHandle == Handle)
                    {
                        Index = OldIndex = i;
                        NewTitle = GetWindowTitleFromHandle(Handle);
                        RemoveKeywords(NewTitle,Index);
                        if(NewTitle != CurrentTitle) TitleChanged = true;
                        CurrentTitle = NewTitle;
                        Item.WindowHandle = Handle;
                        return Index;
                    }
                }
            }

        }
        //check next handle
        Handle = GetWindow(Handle,GW_HWNDNEXT);
    }
    return MEDIAMANAGER_MEDIANOTFOUND; //No anime detected
}

/************************************************************
 * Main loop called by the detection timer to detect anime
 ***********************************************************/
void MediaManager::DetectAnime()
{
    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(CurrentEpisode.Slug);
    int MediaPlayerIndex = DetectMediaPlayer();

    //Media player is running
    if(MediaPlayerIndex > MEDIAMANAGER_MEDIANOTFOUND)
    {
        //Check if we are not watching an episode
        if(CurrentEpisode.Slug == ANIMEEPISODE_UNKNOWN_SLUG)
        {
            if(Recognition_Engine.ExamineTitle(CurrentTitle,CurrentEpisode))
            {
                QString AnimeSlug = Anime_Database.GetAnimeSlug(CurrentEpisode.CleanTitle,false);
                if(AnimeSlug != ANIMEDATABASE_UKNOWN_SLUG)
                {
                    Entity = Anime_Database.GetAnime(AnimeSlug);
                    if(Entity)
                    {
                        CurrentEpisode.Set(Entity->GetAnimeSlug());
                        return;
                    }
                }
                //Not recognised
            } else {
                CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
                if(!CurrentEpisode.Title.isEmpty())
                {
                    SetTitleChanged(false);
                    //Unknown title
                }
            }
            // Already watching or not recognized before
        } else {
            if (MediaTicker > -1 && MediaTicker <= MEDIAMANAGER_UPDATEDELAY)
            {
                if (MediaTicker == MEDIAMANAGER_UPDATEDELAY)
                {
                    // Disable ticker
                    MediaTicker = -1;
                    // Update anime
                    if (Entity); //TODO: Add an update function based on currentepisode
                    //anime_item->UpdateList(CurrentEpisode);
                    return;
                }

                if (MediaList[MediaPlayerIndex].WindowHandle == GetForegroundWindow())
                    MediaTicker++;
            }
            // Caption changed?
            if (TitleChanged)
            {
                SetTitleChanged(false);
                bool Processed = CurrentEpisode.Processed;
                CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
                if (Entity)
                {
                    //TODO: ADD START AND END WATCHING FUNCTIONS
                    //anime_item->EndWatching(CurrentEpisode);
                    CurrentEpisode.Slug = Entity->GetAnimeSlug();
                    CurrentEpisode.Processed = Processed;
                    //anime_item->UpdateList(CurrentEpisode);
                    CurrentEpisode.Slug = ANIMEEPISODE_UNKNOWN_SLUG;
                }
                MediaTicker = 0;
            }
        }
     // Media player is NOT running
    } else {
        // Was running, but not watching
        if (!Entity)
        {
            if (OldIndex > 0)
            {
                CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
                OldIndex = 0;
            }

            // Was running and watching
        } else {
            bool Processed = CurrentEpisode.Processed;
            CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
            //anime_item->EndWatching(CurrentEpisode);
            CurrentEpisode.Slug = Entity->GetAnimeSlug();
            CurrentEpisode.Processed = Processed;
            //anime_item->UpdateList(CurrentEpisode);
            CurrentEpisode.Slug = ANIMEEPISODE_UNKNOWN_SLUG;
            MediaTicker = 0;
        }
    }

}

//*************************************************************************************************************
Media::Media():
    Name(""),
    Enabled(false),
    Mode(0),
    WindowHandle(nullptr)
{

}
