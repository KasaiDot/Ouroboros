#include "ouroboros.h"
#include "ui_ouroboros.h"

#include <QTime>
#include <QDebug>

#include "animeentity.h"

Ouroboros::Ouroboros(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Ouroboros)
{
    ui->setupUi(this);

    //Generate a random seed
    QTime CurrentTime = QTime::currentTime();
    qsrand((uint)CurrentTime.msec());

    Anime::AnimeEntity *a = new Anime::AnimeEntity();
    a->SetAnimeTitle("test");
    a->SetAnimeSlug("slug");

    Anime_Database.AddAnime(a);
    qDebug() << Anime_Database.GetDatabaseSize();

}

Ouroboros::~Ouroboros()
{
    delete ui;
}
