Ouroboros
=========

Hummingbird.me Desktop App

Index
==========================
* [Classes](#classes)  
* [Notes](#notes)  
* [Src Requirements](#src-requirements)  
* [Building Src](#building-src)  
* [Copyright Info](#copyright-info)  


Classes
============================

**AnimeDatabase** - Main database which contains all functions to access single entities  
**AnimeEntity** - Hold all anime data and user data for a specific anime  
**ApiManager** - Class which links Ouroboros with the Hummingbird Api  
**Globals** - Holds global values which are tweakable  
**Ouroboros** - Main entry point into the program  
**QueueItem** - Single queue entity which is used by QueueManager  
**QueueManager** - Class that handles queueing of user actions such as authenticating user, getting library etc...  
**QueueWorker** - A Worker method which is used in conjunction with a thread to run QueueItems asynchronously  
**ThreadManager** - Class which handles creation and deletion of threads within the application  

## Classes not included  
  
**Private** - Header file which holds private information such as the Ma-shape key  
  
Notes  
===========================

Ouroboros uses extern classes, meaning that some classes are global such as the apimanager and they won't need to be initialised manually.  
  
Here is the list of classes that are global and their equivilent variable:  
  
* **AnimeDatabase** - Anime_Database  
* **ApiManager** - Api_Manager  
* **QueueManager** - Queue_Manager  
* **ThreadManager** - Thread_Manager  
  
You may also notice that classes have been put into namespaces. This is used to group classes based on what they are part of or what they do, E.g apimanager is part of managers  
  
Here are the list of current namespaces:  

* Anime  
* Queue  
* Manager  
* Ui - Default Qt namespace  

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

It is reccommended to build Qt static in another directory than the default Qt one.
E.G: Qt-[version]-static folder

The Ouroboros source was built using the static libraries of Qt.

The build configuration was as follows for Windows 7:

<code>configure.bat -static -release -prefix [Path to qt folder]/qtbase -nomake test -nomake examples </code>
<code> -nomake demos -opengl desktop -platform win32-g++ -openssl -L \<openssl folder path\>\lib </code>
<code> -I \<openssl folder path\>\include </code>
  
This configuration will allow you to statically build release versions of Ouroboros.
Arguments such as the nomake ones are optional.

The -opengl argument is also optional.
The platfrom argument varies depending on the platform and the compiler.

OpenSSL Libraries must be downloaded (get it from https://www.openssl.org/).  
**Note:** The libeay33 and the ssleay32 dlls will need to be copied to the application folder

For more detail on static building visit https://qt-project.org/doc/qt-5.0/qtdoc/deployment.html.

Copyright info
==================================================================

Copyright (C) 2010-2013 Mikunj Varsani

Ouroboros is free software. This means that the source code is available to public, 
anyone is welcome to research how the application works, participate in its development, 
freely distribute the application and spread the word!

