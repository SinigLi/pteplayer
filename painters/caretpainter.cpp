/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "caretpainter.h"

#include "styles.h"

#include <app/caret.h>
#include <app/viewoptions.h>
#include <painters/layoutinfo.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <score/scorelocation.h>
#include <score/score.h>
#include <score/system.h>
#include <util/tostring.h>

const double CaretPainter::PEN_WIDTH = 5;
const double CaretPainter::CARET_NOTE_SPACING = 6;

CaretPainter::CaretPainter(const Caret &caret, const ViewOptions &view_options,
                           const QPalette &palette)
    : myCaret(caret),
      myViewOptions(view_options),
      myPalette(palette),
      myCaretConnection(
          caret.subscribeToChanges([=]() { onLocationChanged(); }))
{
}

void CaretPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                         QWidget *)
{
    if (!myLayout)
    {
        onLocationChanged();
        return;
    }

    const ScoreLocation &location = myCaret.getLocation();
    const bool hasFocus = scene()->views().first()->hasFocus();

    // Set color.
    QColor color = myPalette.link().color();
    if (!hasFocus)
        color.setAlpha(128);
    else if (myCaret.isInPlaybackMode())
        color = myPalette.linkVisited().color();

    painter->setPen(QPen(color, PEN_WIDTH));

    double left = myLayout->getPositionX(location.getPositionIndex());
    const double y1 = 0;
    const double y2 = myLayout->getTabStaffHeight();
    const double x = LayoutInfo::centerItem(left,
                                            left + myLayout->getPositionSpacing(),
                                            1);

    // If in playback mode, just draw a vertical line and don't highlight
    // the selected note.
    // if (myCaret.isInPlaybackMode())
    // {
    //     painter->drawLine(x, y1, x, y2);
    //     return;
    // }

    QVector<QLine> lines(0);

    // Calculations for the box around the selected note.
    const double stringHeight = myLayout->getTabLine(location.getString() + 1) -
            myLayout->getTopTabLine();
    const double boundary1 = stringHeight - CARET_NOTE_SPACING;
    const double boundary2 = stringHeight + CARET_NOTE_SPACING;

    // Draw a line down to the highlighted string, if necessary.
    if (y1 < boundary1)
        lines.append(QLine(x, y1, x, boundary1));

    // Draw horizontal lines around note, but don't exceed the default width.
    const double width = std::min(myLayout->getPositionSpacing(),
                                  LayoutInfo::DEFAULT_POSITION_SPACING);

    left = LayoutInfo::centerItem(left, left + myLayout->getPositionSpacing(),
                                  width);
    lines.append(QLine(left, boundary1, left + width, boundary1));
    lines.append(QLine(left, boundary2, left + width, boundary2));

    // Draw to bottom of staff, if necessary.
    if (y2 > boundary2)
        lines.append(QLine(x, boundary2, x, y2));

    painter->drawLines(lines);

    // Draw the selection
    painter->setBrush(Styles::SelectionColor);
    painter->setPen(QPen(QBrush(), 0));

    left = myLayout->getPositionX(std::min(location.getSelectionStart(),
                                           location.getPositionIndex()));
    double right = myLayout->getPositionX(std::max(location.getSelectionStart(),
                                                   location.getPositionIndex()));

    if (left != right)
        right += myLayout->getPositionSpacing();

    painter->drawRect(QRectF(left, 1, right - left,
                             myLayout->getTabStaffHeight() - 1));
}

QRectF CaretPainter::boundingRect() const
{
    if (myLayout)
    {
        return QRectF(0, -CARET_NOTE_SPACING, LayoutInfo::STAFF_WIDTH,
                      myLayout->getTabStaffHeight() + 2 * CARET_NOTE_SPACING);
    }
    else
        return QRectF();
}

void CaretPainter::addSystemRect(const QRectF &rect)
{
    mySystemRects.push_back(rect);
}

void CaretPainter::setSystemRect(int index, const QRectF &rect)
{
    mySystemRects.at(index) = rect;
}


QRectF
CaretPainter::getSystemRect(int startIndx, int sizeRect) const
{
    QRectF result;
    int restCount = sizeRect;
    for (int i =0;i<sizeRect;++i)
    {
        size_t curi = i + startIndx;
        if(curi>=mySystemRects.size()){
            break;
        }
        result = result.united(mySystemRects.at(curi));
        --restCount;
    }
    for(int i = 1;i<=restCount;++i)
    {
        int curi =  startIndx - i;
        if(curi<0){
            break;
        }
        result = result.united(mySystemRects.at(curi));
    }
    return result;
}

QRectF CaretPainter::getCurrentSystemRect() const
{
    return mySystemRects.at(myCaret.getLocation().getSystemIndex());
}

void CaretPainter::updatePosition()
{
    // Force an update of the caret.
    onLocationChanged();
}

boost::signals2::connection CaretPainter::subscribeToMovement(
        const LocationChangedSlot::slot_type &subscriber)
{
    return onMyLocationChanged.connect(subscriber);
}

QSizeF CaretPainter::avgSysSize() const
{
    double height = 0;
    double width  = 0;
    QRectF combineRect;
    for(const QRectF &one:mySystemRects)
    {
        combineRect = combineRect.united(one);
        //height += one.height()/mySystemRects.size();
        //width += one.width()/mySystemRects.size();
    }
    if (combineRect.isEmpty()) {
        return QSizeF();
    }
    return QSizeF(combineRect.width(), combineRect.height()/ mySystemRects.size());
}

void CaretPainter::onLocationChanged()
{
    const ScoreLocation &location = myCaret.getLocation();
    if (location.getScore().getSystems().empty())
        return;

    const System &system = location.getSystem();
    if (system.getStaves().empty())
        return;

    myLayout = std::make_unique<LayoutInfo>(location, &myViewOptions);

    const ViewFilter *filter =
        myViewOptions.getFilter()
            ? &location.getScore().getViewFilters()[*myViewOptions.getFilter()]
            : nullptr;

    // Compute the offset due to the previous (visible) staves.
    double offset = 0;
    for (int i = 0; i < location.getStaffIndex(); ++i)
    {
        if (!filter ||
            filter->accept(location.getScore(), location.getSystemIndex(), i))
        {
            ScoreLocation staff_location(location);
            staff_location.setStaffIndex(i);
            offset += LayoutInfo(staff_location).getStaffHeight();
        }
    }

    const QRectF oldRect = sceneBoundingRect();
    setPos(0, mySystemRects.at(location.getSystemIndex()).top() + offset +
           myLayout->getSystemSymbolSpacing() + myLayout->getStaffHeight() -
           myLayout->getTabStaffBelowSpacing() - myLayout->STAFF_BORDER_SPACING -
           myLayout->getTabStaffHeight());
    update(boundingRect());
    // Ensure that a redraw always occurs at the old location.
    scene()->update(oldRect);

    setToolTip(QString::fromStdString(Util::toString(location)));

    // Notify anyone interested in the caret being redrawn.
    onMyLocationChanged();
}
