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

#ifndef GLOBALS_H
#define GLOBALS_H

//*********************************************************************************************************
#define RECOGNITION_TIMEDELAY 1000

//********************************************** Messages ***************************************************
#define APP_MESSAGE_SHOWAPP "ShowApp"

//********************************************* All statuses ******************************************************
#define STATUS_COMPLETED "completed"
#define STATUS_CURRENTLY_WATCHING "currently-watching"
#define STATUS_ON_HOLD "on-hold"
#define STATUS_DROPPED "dropped"
#define STATUS_PLAN_TO_WATCH "plan-to-watch"

#define ANIME_STATUS_CURRENTLY_AIRING "Currently Airing"
#define ANIME_STATUS_FINISHED_AIRING "Finished Airing"

//********************************************* Roles **************************************************************
#define ROLE_DEFAULT 0x0100 //Since we can't use Qt::UserRole in the macro, we have to manually assign it
#define ROLE_ANIME_SLUG (ROLE_DEFAULT + 1)
#define ROLE_ANIME_EPISODES (ROLE_DEFAULT + 2)
#define ROLE_USER_STATUS (ROLE_DEFAULT + 3)
#define ROLE_USER_EPISODES 0//Make this equal to the display role so we can sort the progress

#endif // GLOBALS_H
