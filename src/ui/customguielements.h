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

#ifndef CUSTOMGUIELEMENTS_H
#define CUSTOMGUIELEMENTS_H

#include <QObject>
#include <QPainter>
#include <QRect>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QToolButton>

#include "library/animeentity.h"
#include "ouroboros/globals.h"
#include "ouroboros/settings.h"
#include "ui/animeprogressbar.h"

namespace CustomGui
{

/********************************* Delegates *****************************************************/
// A Delegate is basically a control which we can embed into a view, E.g, in a torrent application
// we would use a delegate to add a progress bar to the view

//Progress Bar
class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ProgressDelegate(QObject *parent, AnimeProgressBar *Style);
    //Button types
    enum Button
    {
        Plus,
        Minus
    };

    //The paint method is where we do most of the work
    void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;

    //Now to see if the user clicked a box, we have to check the editor events
    bool editorEvent(QEvent *Event, QAbstractItemModel *Model, const QStyleOptionViewItem &Option, const QModelIndex &Index);

signals:
    void ButtonClicked(QString Slug,CustomGui::ProgressDelegate::Button Type);
    void SelectRow(QModelIndex Index);

private:

    CustomGui::AnimeProgressBar *Style;

};

//***************************************** Other Gui Components ******************************************

//Textbox with 'x' to clear
class EraseableLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    EraseableLineEdit(QWidget *parent = 0, bool PaintCross = true);

private:
    QToolButton *ClearButton;
    bool PaintCross;
    int CrossWidth,CrossHeight;

public slots:
    void UpdateCloseButton(const QString &Text);
    void SetupClearButtonRect();

};

}
#endif // CUSTOMGUIELEMENTS_H
