#include "customguielements.h"


/*************************************************** Progress bar ****************************************************************/

CustomGui::ProgressDelegate::ProgressDelegate(QObject *parent, AnimeProgressBar *Style) :
    QStyledItemDelegate(parent)
{
    this->Style = Style;
}

//Paint the actual progress bar
void CustomGui::ProgressDelegate::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
    QStyledItemDelegate::paint(Painter,Option,Index);

    //First we check if mouse is hovering above the item
    bool isHovering = ((Option.state & QStyle::State_MouseOver) == QStyle::State_MouseOver);

    //progress bar values
    int MinValue = 0;
    int CurrentValue = 0;
    int MaxValue = 500;

    //Get the user episode count and anime episode count from the index
    int UserEpisodeCount = Index.data(ROLE_USER_EPISODES).toInt();
    int AnimeEpisodeCount = Index.data(ROLE_ANIME_EPISODES).toInt();

    QString AnimeEpisodeText = QString::number(AnimeEpisodeCount);

    //Check if we have got the anime episode count
    if(AnimeEpisodeCount > ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
    {
        if(UserEpisodeCount > AnimeEpisodeCount)
            UserEpisodeCount = AnimeEpisodeCount;

        MaxValue = AnimeEpisodeCount;
    } else {
        AnimeEpisodeText = "-";
        MaxValue = UserEpisodeCount + 50;
    }

    //Set Current value
    if(UserEpisodeCount < MinValue)
    {
        UserEpisodeCount = MinValue;
    }

    //Do this so we don't divide by zero when displaying completed list
    CurrentValue = UserEpisodeCount;

    //Fill in whole bar if unknown anime episode and completed
    if(Index.data(ROLE_USER_STATUS).toString() == STATUS_COMPLETED)
    {
        if(AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        {
            CurrentValue = 1;
            MaxValue = CurrentValue;
        }
    }

    QString UserEpisodeText = QString::number(UserEpisodeCount);
    QString ProgressText = UserEpisodeText + "/" + AnimeEpisodeText;

    //progressbar Colors
    QColor ProgressBarColor = Qt::gray;
    QString CurrentStatus = Index.data(ROLE_USER_STATUS).toString();
    if(CurrentStatus == STATUS_CURRENTLY_WATCHING || STATUS_PLAN_TO_WATCH) ProgressBarColor = Style->GetProgressbar_Current();
    if(CurrentStatus == STATUS_COMPLETED) ProgressBarColor = Style->GetProgressbar_Completed();
    if(CurrentStatus == STATUS_ON_HOLD) ProgressBarColor = Style->GetProgressbar_OnHold();
    if(CurrentStatus == STATUS_DROPPED) ProgressBarColor = Style->GetProgressbar_Dropped();


    //create the text rect
    int MaxTextPixelSize = Option.fontMetrics.size(Qt::TextSingleLine,"999/999").width() + 8;
    QRect TextRect(Option.rect.left() + Option.rect.width() - MaxTextPixelSize + 3,Option.rect.top() + 2,MaxTextPixelSize,Option.rect.height());

    //Paint the string
    Painter->save();
    Painter->setRenderHint(QPainter::Antialiasing);
    Painter->setBrush(Qt::transparent);
    Painter->setPen(Style->GetTextColor());
    Painter->drawText(TextRect,ProgressText);
    Painter->restore();

    //Paint the progressbar outline
    //note if editing progressbaroutline then you have to update the one in editor events
    QRect ProgressBarOutlineRect(Option.rect.left() + 3,Option.rect.top() + 4,Option.rect.width() - MaxTextPixelSize - 6,Option.rect.height() - 8);
    Painter->save();
    Painter->setRenderHint(QPainter::Antialiasing);
    Painter->setBrush(Style->GetOutlineColor());
    Painter->setPen(QColor(Qt::transparent));
    Painter->drawRoundedRect(ProgressBarOutlineRect,Style->GetOutlineRadius(),Style->GetOutlineRadius());
    Painter->restore();

    //Paint the progressbar background
    QRect ProgressBarBackgroundRect(ProgressBarOutlineRect.left() + 1, ProgressBarOutlineRect.top() + 1,ProgressBarOutlineRect.width() - 2, ProgressBarOutlineRect.height() - 2);
    Painter->save();
    Painter->setRenderHint(QPainter::Antialiasing);
    Painter->setBrush(Style->GetBackgroundColor());
    Painter->setPen(QColor(Qt::transparent));
    Painter->drawRoundedRect(ProgressBarBackgroundRect,Style->GetOutlineRadius(),Style->GetOutlineRadius());
    Painter->restore();

    //Paint the progressbar
    int ProgressBarWidth = float((ProgressBarOutlineRect.width() * CurrentValue)/ MaxValue);
    if(ProgressBarWidth > 1 && CurrentValue >= MinValue)
    {
        QRect ProgressBarRect(ProgressBarBackgroundRect.left(),ProgressBarBackgroundRect.top(),ProgressBarWidth - 2,ProgressBarBackgroundRect.height());
        Painter->save();
        Painter->setRenderHint(QPainter::Antialiasing);
        Painter->setBrush(ProgressBarColor);
        Painter->setPen(QColor(Qt::transparent));
        Painter->drawRoundedRect(ProgressBarRect,Style->GetOutlineRadius(),Style->GetOutlineRadius());
        Painter->restore();

    }


    //Draw the plus and minus boxes if mouse is hovering
    if(isHovering)
    {
        //Global values
        QColor BoxColor = QColor(96,96,96);
        //We draw the + and - boxes around the progress bar
        QStyleOptionGraphicsItem Minus;
        Minus.state = QStyle::State_Enabled;
        Minus.rect = ProgressBarOutlineRect;
        Minus.rect.setWidth(ProgressBarOutlineRect.height());

        Painter->fillRect(Minus.rect,QBrush(BoxColor));
        Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
        Painter->drawLine(Minus.rect.x() + 3,Minus.rect.y() + (Minus.rect.height() / 2),Minus.rect.x() + Minus.rect.width() - 3,Minus.rect.y() + (Minus.rect.height() / 2));

        QStyleOptionGraphicsItem Plus;
        Plus.state = QStyle::State_Enabled;
        Plus.rect = ProgressBarOutlineRect;
        Plus.rect.setX((ProgressBarOutlineRect.x() + ProgressBarOutlineRect.width() - ProgressBarOutlineRect.height()));
        Plus.rect.setWidth(ProgressBarOutlineRect.height());

        Painter->fillRect(Plus.rect,QBrush(BoxColor));
        Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
        Painter->drawLine(Plus.rect.x() + 3,Plus.rect.y() + (Plus.rect.height() / 2),Plus.rect.x() + Plus.rect.width() - 3,Plus.rect.y() + (Plus.rect.height() / 2)); //- line
        Painter->drawLine(Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + 3,Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + Plus.rect.height() - 3); // | line


        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Minus,Painter);
        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Plus,Painter);

    }
}

//Check for click events
bool CustomGui::ProgressDelegate::editorEvent(QEvent *Event, QAbstractItemModel *Model, const QStyleOptionViewItem &Option, const QModelIndex &Index)
{
    Q_UNUSED(Model)

    //Make the the progressbar rect
    int MaxTextPixelSize = QApplication::fontMetrics().size(Qt::TextSingleLine,"999/999").width() + 8;
    QRect ProgressBarOutlineRect(Option.rect.left() + 3,Option.rect.top() + 4,Option.rect.width() - MaxTextPixelSize - 6,Option.rect.height() - 8);

    //Make the hitboxes for the plus and minus buttons
    QRect Minus(ProgressBarOutlineRect);
    Minus.setWidth(Minus.height());

    QRect Plus(ProgressBarOutlineRect);
    Plus.setX(ProgressBarOutlineRect.x() + ProgressBarOutlineRect.width() - ProgressBarOutlineRect.height());
    Plus.setWidth(Plus.height());

    QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);

    //Check for clicks
    if( Event->type() == QEvent::MouseButtonPress)
    {
        //we want to select the row
        emit SelectRow(Index);
    } else if( Event->type() == QEvent::MouseButtonRelease) {

        //User Clicked the button, so we send the signal for GUI_Manager to handle
        if(Minus.contains( MouseEvent->pos()))
            emit ButtonClicked(Index.data(ROLE_ANIME_SLUG).toString(),Button::Minus);
        else if(Plus.contains( MouseEvent->pos()))
            emit ButtonClicked(Index.data(ROLE_ANIME_SLUG).toString(),Button::Plus);
    }
    return true;
}

/*********************************************************** Erasable LineEdit **************************************************************/

CustomGui::EraseableLineEdit::EraseableLineEdit(QWidget *parent, bool PaintCross):
    QLineEdit(parent),
    PaintCross(PaintCross)
{
    QIcon CrossIcon(":/Resources/cross.png");
    CrossWidth = 16;
    CrossHeight = 16;

    ClearButton = new QToolButton(this);
    ClearButton->setObjectName("ClearButton");
    ClearButton->setIcon(CrossIcon);
    ClearButton->setIconSize(QSize(CrossWidth,CrossHeight));
    ClearButton->setCursor(Qt::PointingHandCursor);
    ClearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    ClearButton->hide();

    //Connect signals and slots
    connect(ClearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(UpdateCloseButton(const QString&)));

    QMetaObject::invokeMethod( this, "SetupClearButtonRect", Qt::QueuedConnection );
}

//Event handler for when text is changed
void CustomGui::EraseableLineEdit::UpdateCloseButton(const QString &Text)
{
    ClearButton->setVisible(!Text.isEmpty());
}

//Sets up the clear button
void CustomGui::EraseableLineEdit::SetupClearButtonRect()
{
    //Setup the button
    QStyleOptionFrame StyleOption;
    initStyleOption(&StyleOption);;
    QRect TextBoxRect = style()->subElementRect(QStyle::SE_LineEditContents, &StyleOption, this);
    ClearButton->setGeometry(TextBoxRect.x() + (TextBoxRect.width() - CrossWidth),TextBoxRect.y() + ((TextBoxRect.height() - CrossHeight)/2),CrossWidth,CrossHeight);
}

