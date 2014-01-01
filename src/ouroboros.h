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

#ifndef OUROBOROS_H
#define OUROBOROS_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTreeView>
#include <QTimer>
#include <QSystemTrayIcon>

#include "threadmanager.h"
#include "apimanager.h"
#include "animedatabase.h"
#include "globals.h"

namespace Ui {
class Ouroboros;
}


class Ouroboros : public QMainWindow
{
    Q_OBJECT

public:
    explicit Ouroboros(QWidget *parent = 0);
    ~Ouroboros();

    /******************************************* Recognition *******************************************/
    enum Play_Status
    {
      PLAYSTATUS_STOPPED,
      PLAYSTATUS_PLAYING,
      PLAYSTATUS_UPDATED
    }PlayStatus;

    /******************************************** UI ***********************************************/
    enum Views
    {
        CurrentlyWatching,
        OnHold,
        Dropped,
        Completed,
        PlanToWatch,
        Search
    };

    enum Actions
    {
        EditUserEpisodes
    };

    //tray
    enum TrayActions
    {
        Tray_ShowOuroboros,
        Tray_Sync,
        Tray_Settings,
        Tray_Exit
    };

    /********************************************* Functions *******************************************/
    //Fills the tabs with the view
    void SetViewLayouts();

    //Creates tray icon
    void SetupTrayIcon();

    //Get Functions
    QTabWidget* GetMainTabWidget();
    QTreeView* GetView(Ouroboros::Views Type);
    QAction* GetAction(Ouroboros::Actions Type);
    QToolBar* GetMainToolBar();

private:
    Ui::Ouroboros *ui;

    //System tray
    QSystemTrayIcon *TrayIcon;
    QMenu *TrayMenu;

    //detection
    QTimer DetectionTimer;

protected:

    //application events
    void closeEvent(QCloseEvent *Event);
    void changeEvent(QEvent *Event);


public slots:
    void ChangeStatus(QString Status, int Timeout = 0);
    void RecievedMessageFromInstance(QStringList Messages);

private slots:
    void on_Action_Synchronize_Anime_triggered();
    void on_Action_ChangeSettings_triggered();
    void on_Action_ViewHistory_triggered();
    void on_Action_ViewSearch_triggered();

    //Tray
    void TrayIconTriggered(QSystemTrayIcon::ActivationReason Reason);
    void TrayMenuItemClicked(QAction *Action);
    void ShowTrayMessage(QString Title, QString Message, int msecs = 10000);

    void on_Action_About_triggered();

signals:
    void StopDetectionTimer();

};

#endif // OUROBOROS_H
