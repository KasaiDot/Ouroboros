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

using namespace Recognition;
MediaManager Media_Manager;

MediaManager::MediaManager(QObject *parent) :
    QObject(parent),
    Index(-1),
    OldIndex(-1),
    TitleChanged(false)
{
}

/********************************************
 * Loads in all media information from file
 ********************************************/
void MediaManager::Load(HWND Handle)
{
    GetProcessNameFromHandle(Handle);
    GetWindowTitleFromHandle(Handle);
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

/******************************************************************************
 * Loops through all handles and checks if one of them is in the media list
 * If it is, then we pass it to the recognition class
 ******************************************************************************/
int MediaManager::DetectAnime()
{
    qDebug() << "Detecting anime";
    /***************************************************************
     * Credit to erengy (creator of taiga) for parts of this code
     ***************************************************************/
    Index = -1;
    bool Recognized = false;

    //Iterate through handles, starting from the top most handle
    HWND Handle = GetWindow(GetTopWindow(nullptr), GW_HWNDFIRST);
    while (Handle != nullptr)
    {
        QLinkedList<Media>::iterator Item;
        //Iterate thorugh all our media players
        for(Item = MediaList.begin(); Item != MediaList.end(); ++Item)
        {
            if(!Item->Enabled) continue;
            if(!IsWindowVisible(Handle)) continue;
            //Compare the process names
            for(QStringList::iterator File = Item->Files.begin(); File != Item->Files.end(); ++File)
            {
                if(File->isEmpty()) continue;
                if((*File == GetProcessNameFromHandle(Handle)) && !GetWindowTitleFromHandle(Handle).isEmpty())
                {
                    // Stick with the previously recognized window, if there is one
                    if(!Recognized || Item->WindowHandle == Handle)
                    {
                        //Index = OldIndex = Item - MediaList.begin();
                        NewTitle = GetWindowTitleFromHandle(Handle);
                        CurrentTitle = NewTitle;
                        Item->WindowHandle = Handle;
                        return Index;
                    }
                }
            }

        }
        //check next handle
        Handle = GetWindow(Handle,GW_HWNDNEXT);
    }
    return -1; //No anime detected
}

//*************************************************************************************************************
Media::Media():
    Name(""),
    Enabled(false),
    Mode(0),
    WindowHandle(nullptr)
{

}
