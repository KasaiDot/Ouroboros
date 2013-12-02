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
#include "guimanager.h"
#include "filemanager.h"

using namespace Recognition;
MediaManager Media_Manager;

/*********************************************************************
 * Credits to erengy(Creator of Taiga) for the media class
 ********************************************************************/

MediaManager::MediaManager(QObject *parent) :
    QObject(parent),
    Index(-1),
    OldIndex(-1),
    MediaListLoaded(false),
    MediaTicker(-1),
    TitleChanged(false)
{
}

/********************************************
 * Loads in all media information from file
 ********************************************/
bool MediaManager::Load(QByteArray &Data)
{
    MediaListLoaded = false;
    QJsonDocument Doc = QJsonDocument::fromJson(Data);
    QVariantList MediaArray = Doc.toVariant().toList();

    //iterate through list
    foreach(QVariant Variant,MediaArray)
    {
        QVariantMap MediaObject = Variant.toMap();

        //Construct the media
        Media NewMedia;
        NewMedia.Name = MediaObject.value("Name","").toString();
        NewMedia.Enabled = MediaObject.value("Enabled",false).toBool();
        NewMedia.Mode = MediaObject.value("RecognitionMode",0).toInt();

        QVariantList FileList = MediaObject.value("Files").toList();
        foreach(QVariant File,FileList)
            NewMedia.Files.append(File.toString());

        QVariantList FolderList = MediaObject.value("Folders").toList();
        foreach(QVariant Folder,FolderList)
            NewMedia.Folders.append(Folder.toString());

        QVariantList KeywordList = MediaObject.value("RemoveKeywords").toList();
        foreach(QVariant Keyword,KeywordList)
            NewMedia.Keywords.append(Keyword.toString());

        //check if it is valid
        if(NewMedia.Name.isEmpty() || NewMedia.Files.isEmpty()) continue;
        else MediaList.append(NewMedia);

    }

    if(MediaList.size() > 0)
        MediaListLoaded = true;

    return true;
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
                        NewTitle = GetWindowTitleFromHandle(Handle).trimmed();
                        RemoveKeywords(NewTitle,Index);
                        if(NewTitle.trimmed().toLower() != CurrentTitle.trimmed().toLower()) TitleChanged = true;
                        CurrentTitle = NewTitle.trimmed();
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
        if(CurrentEpisode.Slug == ANIMEEPISODE_UNKNOWN_SLUG)
        {
            if(Settings.Recognition.Enabled)
            {
                //Check if we are not watching an episode
                if(Recognition_Engine.ExamineTitle(CurrentTitle,CurrentEpisode))
                {
                    QString AnimeSlug = Anime_Database.GetAnimeSlug(CurrentEpisode.CleanTitle,true,false);
                    if(AnimeSlug != ANIMEDATABASE_UKNOWN_SLUG)
                    {
                        Entity = Anime_Database.GetAnime(AnimeSlug);
                        if(Entity)
                        {
                            SetTitleChanged(false);
                            CurrentEpisode.Set(Entity->GetAnimeSlug());
                            GUI_Manager.StartWatching(CurrentEpisode,Entity);
                            return;
                        }
                    }
                    //Not recognised
                } else {
                    CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
                    if(!CurrentEpisode.Title.isEmpty())
                    {
                        SetTitleChanged(false);
                        if(Settings.Recognition.NotifyEpisodeNotRecognised)
                        {
                            QString Message = QString("Cannot recognise %1 \n").arg(CurrentEpisode.Title);
                            Message.append("If you think this is a bug, please report it to the Ouroboros hummingbird thread.");
                            emit ShowTrayMessage("Unknown Title", Message);
                        }
                    }
                }
            }
            // Already watching or not recognized before
        } else {
            if (MediaTicker > -1 && MediaTicker <= Settings.Recognition.Delay)
            {
                if (MediaTicker == Settings.Recognition.Delay)
                {
                    // Disable ticker
                    MediaTicker = -1;
                    // Update anime
                    if(!Settings.Recognition.WaitForMPClose)
                        if (Entity)
                            Anime_Database.UpdateEntity(CurrentEpisode,Entity);
                    return;
                }

                if (!Settings.Recognition.MPFocus || MediaList[MediaPlayerIndex].WindowHandle == GetForegroundWindow())
                    MediaTicker++;
            }
            // Caption changed?
            if (DidTitleChange())
            {
                SetTitleChanged(false);
                bool Processed = CurrentEpisode.Processed;
                CurrentEpisode.Set(ANIMEEPISODE_UNKNOWN_SLUG);
                if (Entity)
                {
                    GUI_Manager.FinishWatching(CurrentEpisode,Entity);
                    CurrentEpisode.Slug = Entity->GetAnimeSlug();
                    CurrentEpisode.Processed = Processed;
                    Anime_Database.UpdateEntity(CurrentEpisode,Entity);
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
            GUI_Manager.FinishWatching(CurrentEpisode,Entity);
            if(Settings.Recognition.WaitForMPClose)
            {
                CurrentEpisode.Slug = Entity->GetAnimeSlug();
                CurrentEpisode.Processed = Processed;
                Anime_Database.UpdateEntity(CurrentEpisode,Entity);
                CurrentEpisode.Slug = ANIMEEPISODE_UNKNOWN_SLUG;
            }
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
