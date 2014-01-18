#ifndef ANIMEPROGRESSBAR_H
#define ANIMEPROGRESSBAR_H

#include <QWidget>
#include <QColor>

/*********************************************************************************************
 *  This class makes it possible for us to style the progress bars inside the tree view
 *  as QStyledItemDelegates do not support stylesheet assignment
 * ********************************************************************************************/
namespace CustomGui
{

class AnimeProgressBar : public QWidget
{
    Q_OBJECT

    //Properties for progressdelegate (trying to stick to css standards)
    /****************************************************************************************
     *  How to use in stylesheet:
     *
     *  CustomGui--AnimeProgressBar
     *  {
     *      //every property creates must be started with a qproperty-<property name>
     *       qproperty-borderRadius: 3; //Note do not use px at the end
     *       qproperty-color: rgb(255,255,255);
     *      //... etc
     *
     *  }
     *
     ***************************************************************************************/
    Q_PROPERTY(float borderRadius READ GetOutlineRadius WRITE SetOutlineRadius DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(float progressBarRadius READ GetProgressbarRadius WRITE SetProgressbarRadius DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor color READ GetTextColor WRITE SetTextColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor borderColor READ GetOutlineColor WRITE SetOutlineColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor background READ GetBackgroundColor WRITE SetBackgroundColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor currentlyWatching READ GetProgressbar_Current WRITE SetProgressbar_Current DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor completed READ GetProgressbar_Completed WRITE SetProgressbar_Completed DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor onHold READ GetProgressbar_OnHold WRITE SetProgressbar_OnHold DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor dropped READ GetProgressbar_Dropped WRITE SetProgressbar_Dropped DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor priorityHigh READ GetPriorityHigh WRITE SetPriorityHigh DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor priorityMedium READ GetPriorityMedium WRITE SetPriorityMedium DESIGNABLE true SCRIPTABLE true)

public:
    inline AnimeProgressBar(QWidget* parent = 0) :
        QWidget(parent),
        OutlineRadius(0),
        ProgressbarRadius(0),
        TextColor(QColor(0,0,0)),
        OutlineColor(QColor(160,160,160)),
        BackgroundColor(QColor(250,250,250)),
        Progressbar_Current(QColor(92,213,0)),
        Progressbar_Completed(QColor(51,153,255)),
        Progressbar_OnHold(QColor(255,230,0)),
        Progressbar_Dropped(QColor(255,86,60)),
        PriorityHigh(QColor(231, 76, 60)),
        PriorityMedium(QColor(243, 156, 18))
    {}

    // *********************************** Set and get methods ************************************
    float GetOutlineRadius() const { return OutlineRadius; }
    void SetOutlineRadius(float Radius) { OutlineRadius = Radius / 2; } //More precise rounding

    float GetProgressbarRadius() const { return ProgressbarRadius; }
    void SetProgressbarRadius(float Radius) { ProgressbarRadius = Radius / 2; } //More precise rounding

    QColor GetTextColor() const { return TextColor; }
    void SetTextColor(QColor Color) { TextColor = Color; }

    QColor GetOutlineColor() const { return OutlineColor; }
    void SetOutlineColor(QColor Color) { OutlineColor = Color; }

    QColor GetBackgroundColor() const { return BackgroundColor; }
    void SetBackgroundColor(QColor Color) { BackgroundColor = Color; }

    QColor GetProgressbar_Current() const { return Progressbar_Current; }
    void SetProgressbar_Current(QColor Color) { Progressbar_Current = Color; }

    QColor GetProgressbar_Completed() const { return Progressbar_Completed; }
    void SetProgressbar_Completed(QColor Color) { Progressbar_Completed = Color; }

    QColor GetProgressbar_OnHold() const { return Progressbar_OnHold; }
    void SetProgressbar_OnHold(QColor Color) { Progressbar_OnHold = Color; }

    QColor GetProgressbar_Dropped() const { return Progressbar_Dropped; }
    void SetProgressbar_Dropped(const QColor &Color) { Progressbar_Dropped = Color; }

    QColor GetPriorityHigh() const { return PriorityHigh; }
    void SetPriorityHigh(const QColor &Color) { PriorityHigh = Color; }

    QColor GetPriorityMedium() const { return PriorityMedium; }
    void SetPriorityMedium(const QColor &Color) { PriorityMedium = Color; }

private:

    //Progress Delegate properties
    float OutlineRadius;
    float ProgressbarRadius;
    QColor TextColor;
    QColor OutlineColor;
    QColor BackgroundColor;
    QColor Progressbar_Current;
    QColor Progressbar_Completed;
    QColor Progressbar_OnHold;
    QColor Progressbar_Dropped;
    QColor PriorityHigh;
    QColor PriorityMedium;

};

}

#endif // ANIMEPROGRESSBAR_H
