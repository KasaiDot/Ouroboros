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

#include "dialog_search.h"
#include "ui_dialog_search.h"

#include <QDebug>

#include "user.h"

Dialog_Search::Dialog_Search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Search),
    Searching(false)
{
    ui->setupUi(this);

    SetupTreeWidget();
    SetupSearchBox();

    //connect signals and slots
    connect(ui->SearchButton,SIGNAL(clicked()),this,SLOT(SearchAnime()));
    connect(SearchBox,SIGNAL(returnPressed()),this,SLOT(SearchAnime()));
}

Dialog_Search::~Dialog_Search()
{
    delete ui;
}

/******************************************************
 * Adds a custom search box to the search window
 ******************************************************/
void Dialog_Search::SetupSearchBox()
{
    SearchBox = new CustomGui::EraseableLineEdit(this);
    SearchBox->setPlaceholderText("Search anime");
    SearchBox->installEventFilter(this);
    ui->SearchFieldLayout->insertWidget(0,SearchBox);
    SearchBox->setFocus();
}

/**************************************
 * Sets up headers of the tree widget
 **************************************/
void Dialog_Search::SetupTreeWidget()
{
    QHeaderView *Header = ui->SearchTreeWidget->header();
    QFontMetrics Font = ui->SearchTreeWidget->fontMetrics();

    Header->setDefaultAlignment(Qt::AlignCenter);
    ui->SearchTreeWidget->headerItem()->setTextAlignment(SEARCH_HEADER_TITLE,Qt::AlignLeft);

    Header->resizeSection(SEARCH_HEADER_TITLE,Font.width("------------------- abcdefghijklmnopqrstuvwxyz1234567890 ----------------"));
    Header->resizeSection(SEARCH_HEADER_EPISODE, Font.width("--- Episodes ---"));
    Header->resizeSection(SEARCH_HEADER_TYPE, Font.width("--- Special ---"));
}

/*********************************
 * Searches for the given string
 *********************************/
void Dialog_Search::SetSearchString(QString String)
{
    SearchBox->setText(String);
}

/*********************************
 * Adds anime to the anime table
 ********************************/
void Dialog_Search::ParseAnime(QByteArray &Data)
{
    QJsonDocument MainDoc = QJsonDocument::fromJson(Data);
    QVariantList AnimeVariantList = MainDoc.toVariant().toList();

    foreach(QVariant Variant,AnimeVariantList)
    {
       QJsonDocument Doc = QJsonDocument::fromVariant(Variant);
       Anime::AnimeEntity Entity;
       Entity.ParseAnimeJson(Doc.toJson());
       if(Entity.GetAnimeSlug().isEmpty() || Entity.GetAnimeSlug().isNull()) continue; //go to next iteration

        AnimeList.append(Entity);
    }

    PopulateWidget();
}

/*************************************************
 * Adds items to the widget from the hash table
 ************************************************/
void Dialog_Search::PopulateWidget()
{
    //clear old items first
    ui->SearchTreeWidget->clear();

    if(AnimeList.isEmpty()) return;

    //iterate through anime and add it to the widget
    foreach(Anime::AnimeEntity Entity,AnimeList)
    {
        //if anime is in our list then we apply the green icon to it
        bool AlreadyInList = Anime_Database.Contains(Entity.GetAnimeSlug());

        QString IconLocation = (AlreadyInList) ? ":/Resources/circle-green.png" : ":/Resources/circle-blue.png";
        QIcon ListIcon(IconLocation);

        QString EpisodeCount = (Entity.GetAnimeEpisodeCount() <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE) ? "-" : QString::number(Entity.GetAnimeEpisodeCount());

        QStringList ItemStrings;
        ItemStrings << Entity.GetAnimeTitle();
        ItemStrings << EpisodeCount;
        ItemStrings << Entity.GetAnimeShowType();

        QTreeWidgetItem *Item = new QTreeWidgetItem(ItemStrings);
        Item->setData(SEARCH_HEADER_TITLE,Qt::DecorationRole,ListIcon);
        Item->setTextAlignment(SEARCH_HEADER_EPISODE,Qt::AlignCenter);
        Item->setTextAlignment(SEARCH_HEADER_TYPE,Qt::AlignCenter);

        ui->SearchTreeWidget->addTopLevelItem(Item);
    }

}

/***********************************************
 * Searches for the given text in hummingbird
 **********************************************/
void Dialog_Search::SearchAnime()
{
    if(!CurrentUser.isAuthenticated()) { Searching = false; return; }
    if(SearchBox->text().isEmpty() || Searching) return;

    //clear hash table
    AnimeList.clear();
    PopulateWidget();

    Searching = true;
    ui->SearchButton->setEnabled(false);
    QString SearchText = SearchBox->text();

    QByteArray ReplyData = Api_Manager.Search(SearchText);
    if(ReplyData.isEmpty())
    {
        Searching = false;
        ui->SearchButton->setEnabled(true);
        return;
    }

    ParseAnime(ReplyData);

     ui->SearchButton->setEnabled(true);
     Searching = false;

}

/***************************************************
 * Stops dialog from closing when enter is pressed
 **************************************************/
bool Dialog_Search::eventFilter(QObject *Object, QEvent *Event)
{

    if(Event->type() == QEvent::KeyPress)
    {
        QKeyEvent *KeyEvent = static_cast<QKeyEvent*>(Event);

        /* We want to stop dialog from closing after pressing enter key */
        if(KeyEvent->key() == Qt::Key_Enter || KeyEvent->key() == Qt::Key_Return)
        {
            SearchAnime();
            return true;
        }
    }

    return QWidget::eventFilter(Object,Event);
}

