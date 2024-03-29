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

#include "ouroboros.h"
#include "ui_ouroboros.h"

#include <QTimer>
#include <QDebug>

#include "api/queuemanager.h"
#include "library/animeentity.h"
#include "library/historymanager.h"
#include "manager/filemanager.h"
#include "ui/guimanager.h"
#include "ui/thememanager.h"
#include "recognition/recognitionengine.h"
#include "recognition/mediamanager.h"
#include "ouroboros/appinfo.h"
#include "ouroboros/settings.h"
#include "ui/dialog_about.h"
#include "ui/dialog_settings.h"

//Class which contains a definition APP_DEBUG to indicate whether we are debugging or not
//this is in a seperate header to minimize build times
#include "ouroboros/ouroborosdebug.h"

Ouroboros::Ouroboros(QWidget *parent) :
    QMainWindow(parent),
    PlayStatus(PLAYSTATUS_STOPPED),
    AnimeProgressStyle(this), //assign this as parent so that stylesheet get inherited
    ui(new Ui::Ouroboros)
{
    ui->setupUi(this);\
    setMouseTracking(true);

    //setup main window
    QString Title = QString(APP_NAME) + " " + QString::number(APP_MAJOR_VERSION) + "." + QString::number(APP_MINOR_VERSION);
    if(APP_DEBUG) Title.append(" Debug");
    this->setWindowTitle(Title);

    //Generate a random seed for other classes
    QTime CurrentTime = QTime::currentTime();
    qsrand((uint)CurrentTime.msec());

    //setup views
    SetViewLayouts();

    //setup tray icon
    SetupTrayIcon();

    //Load settings
    Settings.Load();

    //Load the style
    Theme_Manager.LoadThemeList();
    Theme_Manager.LoadTheme(Settings.Application.Stylesheet);
    this->setStyleSheet(QString(Theme_Manager.GetTheme()));

    //Set the ui for the manager
    GUI_Manager.SetMainWindow(this);

    //Setup Detection
    //Only detects on windows currently
#ifdef WIN32
    File_Manager.SaveMedia();
    //Load Media
    File_Manager.LoadMedia();
    if(Media_Manager.MediaListLoaded)
    {
        DetectionTimer.setInterval(RECOGNITION_TIMEDELAY);
        connect(&DetectionTimer,SIGNAL(timeout()),&Media_Manager,SLOT(DetectAnime()));
        connect(this,SIGNAL(StopDetectionTimer()),&DetectionTimer,SLOT(stop()));
        DetectionTimer.start();
    }
#endif

    //Setup a timer to run the queue every 5 minutes
    QTimer *RunTimer = new QTimer(this);
    connect(RunTimer,SIGNAL(timeout()),&Queue_Manager,SLOT(Run()));
    connect(RunTimer,SIGNAL(timeout()),&Theme_Manager,SLOT(LoadThemeList()));
    RunTimer->start(30000);

    //Connect signals and slots
    connect(RunTimer,SIGNAL(timeout()),&Queue_Manager,SLOT(StartRunning()));
    connect(&Api_Manager,SIGNAL(ChangeStatus(QString,int)),this,SLOT(ChangeStatus(QString,int)));
    connect(&GUI_Manager,SIGNAL(ShowTrayMessage(QString,QString,int)),this,SLOT(ShowTrayMessage(QString,QString,int)));
    connect(&Media_Manager,SIGNAL(ShowTrayMessage(QString,QString,int)),this,SLOT(ShowTrayMessage(QString,QString,int)));
    connect(&Theme_Manager,SIGNAL(ThemeChanged(QString)),this,SLOT(setStyleSheet(QString)));

    //Load user info
    File_Manager.LoadUserInformation();

    //Load history
    File_Manager.LoadHistory();

    //Load local database
    File_Manager.LoadAnimeDatabase();
    GUI_Manager.PopulateModel();

    //Sync anime
    if(CurrentUser.isValid())
    {
        emit ChangeStatus("Syncing ...", 3000);
        Queue_Manager.Sync(true);
    }
}

Ouroboros::~Ouroboros()
{
    //Delete detection timer
    emit StopDetectionTimer();

    //Save settings
    Settings.Save();

    //Save
    File_Manager.SaveUserInformation();
    File_Manager.SaveAnimeDatabase();
    File_Manager.SaveQueue();
    File_Manager.SaveHistory();

    TrayIcon->hide();

    delete ui;
}

/*****************************************************
 * Function sets the view so it fills up the tab page
 *****************************************************/
void Ouroboros::SetViewLayouts()
{
    this->setCentralWidget(ui->MainTabWidget);
    ui->MainTabWidget->widget(TAB_CURRENTLY_WATCHING)->setLayout(ui->Layout_CurrentlyWatching);
    ui->MainTabWidget->widget(TAB_COMPLETED)->setLayout(ui->Layout_Completed);
    ui->MainTabWidget->widget(TAB_DROPPED)->setLayout(ui->Layout_Dropped);
    ui->MainTabWidget->widget(TAB_ON_HOLD)->setLayout(ui->Layout_OnHold);
    ui->MainTabWidget->widget(TAB_PLAN_TO_WATCH)->setLayout(ui->Layout_PlanToWatch);
    ui->MainTabWidget->widget(TAB_SEARCH)->setLayout(ui->Layout_Search);

    ui->View_Completed->header()->setSectionsMovable(false);
    ui->View_CurrentlyWatching->header()->setSectionsMovable(false);
    ui->View_Dropped->header()->setSectionsMovable(false);
    ui->View_OnHold->header()->setSectionsMovable(false);
    ui->View_PlanToWatch->header()->setSectionsMovable(false);
    ui->View_Search->header()->setSectionsMovable(false);

    ui->View_Completed->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_CurrentlyWatching->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_Dropped->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_OnHold->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_PlanToWatch->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_Search->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
}

/*************************
 * Creates a tray icon
 ************************/
void Ouroboros::SetupTrayIcon()
{
    TrayIcon = new QSystemTrayIcon(QIcon(":/Resources/Icon.png"),this);
    QString ToolTip = QString("%1 %2.%3").arg(APP_NAME).arg(QString::number(APP_MAJOR_VERSION)).arg(QString::number(APP_MINOR_VERSION));
    if(APP_DEBUG) ToolTip.append(" DEBUG");
    TrayIcon->setToolTip(ToolTip);

    //create the menu
    TrayMenu = new QMenu(this);
    TrayMenu->setObjectName("TrayMenu");
    TrayMenu->addAction("Show Ouroboros")->setData(Tray_ShowOuroboros);
    TrayMenu->addSeparator();
    TrayMenu->addAction("Sync anime")->setData(Tray_Sync);
    TrayMenu->addAction("Settings")->setData(Tray_Settings);
    TrayMenu->addSeparator();
    TrayMenu->addAction("Exit")->setData(Tray_Exit);

    TrayIcon->setContextMenu(TrayMenu);

    //connect signals and slots
    connect(TrayMenu,SIGNAL(triggered(QAction*)),this,SLOT(TrayMenuItemClicked(QAction*)));
    connect(TrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(TrayIconTriggered(QSystemTrayIcon::ActivationReason)));

    TrayIcon->show();
}

/********************************************************************
 * Handles main tray interaction such as double click, right click
 ********************************************************************/
void Ouroboros::TrayIconTriggered(QSystemTrayIcon::ActivationReason Reason)
{
    switch(Reason)
    {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
        if(this->isHidden())
        {
            this->showNormal();
            this->setFocus();
        }
        break;

    default:
        break;
    }
}

/*********************************
 * Handles tray menu clicks
 ********************************/
void Ouroboros::TrayMenuItemClicked(QAction *Action)
{
    int ActionNumber = Action->data().toInt();

    switch (ActionNumber)
    {
    case Tray_ShowOuroboros:
        if(this->isHidden())
        {
            this->showNormal();
            this->setFocus();
        }
        break;

    case Tray_Exit:
        QApplication::instance()->quit();
        break;

    case Tray_Sync:
        Queue_Manager.Sync();
        ShowTrayMessage("Ouroboros Syncing","Syncing anime list ...");
        break;

    case Tray_Settings:
        //show settings dialog
        on_Action_ChangeSettings_triggered();

        break;

    default:
        break;
    }
}

/***************************
 * Displays a tray message
 **************************/
void Ouroboros::ShowTrayMessage(QString Title, QString Message, int msecs)
{
    TrayIcon->showMessage(Title,Message,QSystemTrayIcon::Information,msecs);
}

/************************************
 * Custom application event handlers
 ***********************************/
void Ouroboros::closeEvent(QCloseEvent *Event)
{
    if(Settings.Application.CloseToTray)
    {
        this->hide();
        ShowTrayMessage("Ouroboros minimised to tray","Ouroboros has been minimised to tray, you can show it again by right clicking the tray icon");
        Event->ignore();
    } else {
        Event->accept();
    }
}

void Ouroboros::changeEvent(QEvent *Event)
{
    if(Event->type() == QEvent::WindowStateChange)
    {
        if(isMinimized() && Settings.Application.MinimizeToTray)
        {
            this->hide();
            Event->ignore();
        }
    }
    QMainWindow::changeEvent(Event);
}

/**************************************************
 * Handles message recieving from other instances
 **************************************************/
void Ouroboros::RecievedMessageFromInstance(QStringList Messages)
{
    foreach (QString Message, Messages)
    {
        //Show app
        if(Message == QString(APP_MESSAGE_SHOWAPP))
        {
            this->showNormal();
            this->setFocus();
            this->activateWindow();
        }

    }
}

/************************************ Getter functions ****************************************/
QTabWidget* Ouroboros::GetMainTabWidget()
{
    return ui->MainTabWidget;
}

/*************************
 * Returns main views
 ************************/
QTreeView* Ouroboros::GetView(Ouroboros::Views Type)
{
    switch(Type)
    {
    case CurrentlyWatching:
        return ui->View_CurrentlyWatching;
        break;

    case OnHold:
        return ui->View_OnHold;
        break;

    case PlanToWatch:
        return ui->View_PlanToWatch;
        break;

    case Dropped:
        return ui->View_Dropped;
        break;

    case Completed:
        return ui->View_Completed;
        break;

    case Search:
        return ui->View_Search;
        break;
    }

    return  nullptr;
}

/************************
 * Returns Actions
 ************************/
QAction *Ouroboros::GetAction(Ouroboros::Actions Type)
{
    switch (Type)
    {
    case EditUserEpisodes:
        return ui->Action_EditUserEpisodes;
        break;
    }

    return nullptr;
}

/**************************
 * Returns main tool bar
 *************************/
QToolBar* Ouroboros::GetMainToolBar()
{
    return ui->MainToolBar;
}


/**************************************************
 * Changes the status on the bottom of the screen
 **************************************************/
void Ouroboros::ChangeStatus(QString Status, int Timeout)
{
    ui->MainStatusBar->showMessage(Status,Timeout);
}


/****************** Action Triggers ********************************/
void Ouroboros::on_Action_Synchronize_Anime_triggered()
{
    Queue_Manager.Sync();
}

/*********************************
 * Displays the settings dialog
 *********************************/
void Ouroboros::on_Action_ChangeSettings_triggered()
{
    Dialog_Settings SettingsDialog;
    SettingsDialog.setModal(true);

    SettingsDialog.exec();
}

/**************************
 * Displays history dialog
 *************************/
void Ouroboros::on_Action_ViewHistory_triggered()
{
    History_Manager.ShowHistoryDialog();
}

/****************************
 * Displays search dialog
 ***************************/
void Ouroboros::on_Action_ViewSearch_triggered()
{
    GUI_Manager.ShowSearchDialog();
}

/************************
 * Display about dialog
 ************************/
void Ouroboros::on_Action_About_triggered()
{
    Dialog_About Dialog;
    Dialog.exec();
}

/**********************
 * Exports anime data
 **********************/
void Ouroboros::on_Action_Export_Anime_triggered()
{
    File_Manager.ExportAnime();
}

/**********************
 * Imports anime data
 **********************/
void Ouroboros::on_Action_Import_Anime_triggered()
{
    File_Manager.ImportAnime();
}
