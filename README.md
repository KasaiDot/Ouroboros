Ouroboros
=========

Hummingbird.me anime tracker

Index
==========================
* [Folders](#folders-found-in-src)
* [Classes](#classes)  
* [Notes](#notes)  
* [Src Requirements](#src-requirements)  
* [Building Src](#building-src)  
* [Copyright Info](#copyright-info)  

Folders (found in src)
============================
**api** - Classes which are related to the retrieval/sending of data between the client and the server.  
**library** - Classes that are related to the anime library/list.  
**manager** - Managers which do not belong to any other folder.  
**ouroboros** - Classes which do not belong to any other folder that are required by ouroboros.  
**recognition** - Classes related to the recognition of anime.  
**ui** - Classes which are related to the GUI.  


Classes
============================

**AnimeDatabase** - Main database which contains all functions to access single entities.  
**AnimeEntity** - Hold all anime data and user data for a specific anime.  
**AnimeEpisode** - Holds all the information about the video file user is currently watching.  
**ApiManager** - Class which links Ouroboros with the Hummingbird Api.  
**Common** - Class which contains functions which are common to two or more classes.  
**CustomGui(Header)** - Contains all modified and custom gui elements, such as Textbox with clear button  
**FileManager** - Class which handles all loading and saving in Ouroboros.  
**Globals** - Holds global values which are tweakable.  
**GUIManager** - Class which acts like an interface between the ui and the data.  
**HistoryManager** - Manages user anime history, and passes it to Dialog_History.  
**MediaManager** - Class which handles anime detection (credits erengy).  
**Ouroboros** - Main entry point into the program.  
**QueueItem** - Single queue entity which is used by QueueManager.  
**QueueManager** - Class that handles queueing of user actions such as authenticating user, getting library etc...  
**RecogntionEngine** - Class which handles recognition of anime (credits erengy).  
**Settings** - Holds all settings of Ouroboros.  
**SingleApplication** - Class derived from QApplication that handles application instances.  
**ThreadManager** - Class which handles creation and deletion of threads within the application.  
**User** - Class which holds user information such as username and password.

## Dialogs  

**Dialog_Settings** - Settings dialog.  
**Dialog_AnimeInformation** - Displays anime information.  
**Dialog_History** - Displays user anime history.  
**Dialog_Search** - Searches and displays anime from hummingbird.  
**Dialog_About** - Basic ouroboros about dialog.  

## Classes not included  
  
**Private** - Header file which holds private information such as the Ma-shape key.  
  
Notes  
===========================

Ouroboros uses extern classes, meaning that some classes are global such as the api manager and they won't need to be initialised manually.  
  
Here is the list of classes that are global and their equivalent variable:  
  
* **AnimeDatabase** - Anime_Database  
* **ApiManager** - Api_Manager  
* **QueueManager** - Queue_Manager  
* **ThreadManager** - Thread_Manager  
* **FileManager** - File_Manager
* **User** - CurrentUser  
* **GUIManager** - GUI_Manager  
* **HistoryManager** - History_Manager
* **OuroborosSettings** - Settings  
* **AnimeEpisode** - CurrentEpisode  
* **MediaManager** - Media_Manager  
* **RecognitionEngine** - Recognition_Engine  
  
You may also notice that classes have been put into namespaces. This is used to group classes based on what they are part of or what they do, E.g ApiManager is part of managers  
  
Here are the list of current namespaces:  

* Anime  
* Queue  
* Manager  
* CustomGui  
* Recognition  
* Ui - Default Qt namespace 

You may also notice that sometimes objects are created (A *a = new A) but they aren't deleted.  
The reason is because Qt uses a parent-child hierarchy whereby if an object (must be derived from QObject) has a parent, that object will automatically be deleted upon deletion of the parent object.  
Any objects that don't have a parent or those which do not derive from QObject must be manually deleted.  

Src Requirements
=======================================

Ouroboros is programmed in QT, so you will need to donwload it.
You can download it from here: https://qt-project.org/

<b>Note:</b> This src is built using Windows 7, If you would like to build it for Mac or Linux, you are free to do so,
but some code might need to be ported (Most of it should work as intended since Qt is cross-platfrom)

Building Src
============================================

If you would like to build a custom version of Ouroboros you can do it two ways.

Firstly, you can use the default Qt build which builds using shared libraries, or
Secondly, you can build using the static version of the Qt Library.

It is recommended to build Qt static in another directory than the default Qt one.
E.G: Qt-[version]-static folder

The Ouroboros source was built using the static libraries of Qt.  

Make sure you change the <code>QMAKE_LFLAGS</code> (found in [path to qt folder]/qtbase/mkspecs/[your compiler]/qmake.conf) so that it compiles the libraries required.  
E.g for gcc, you go into [path to qt folder]/qtbase/mkspecs/win32-gcc++/qmake.conf and in <code>QMAKE_LFLAGS</code> add <code>-static -static-libgcc </code>.  


The build configuration was as follows for Windows 7:

<code>configure.bat -static -release -prefix [Path to qt folder]/qtbase -nomake test -nomake examples </code>
<code> -nomake demos -opengl desktop -platform win32-g++ -openssl -L \<openssl folder path\>\lib </code>
<code> -I \<openssl folder path\>\include </code>
  
This configuration will allow you to statically build release versions of Ouroboros.
Arguments such as the nomake ones are optional.

The -opengl argument is also optional.
The platform argument varies depending on the platform and the compiler.

OpenSSL Libraries must be downloaded (get it from https://www.openssl.org/).  
**Note:** The libeay33 and the ssleay32 dlls will need to be copied to the application folder

For more detail on static building visit https://qt-project.org/doc/qt-5.0/qtdoc/deployment.html.

Copyright info
==================================================================

Copyright (C) 2010-2014 Mikunj Varsani

Ouroboros is free software. This means that the source code is available to public, 
anyone is welcome to research how the application works, participate in its development, 
freely distribute the application and spread the word!
