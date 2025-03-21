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
  
#include "keysignaturepainter.h"

#include "musicfont.h"
#include "scoreclickevent.h"

#include <QCoreApplication>
#include <QCursor>
#include <QPainter>
#include <score/keysignature.h>
#include <score/staff.h>

KeySignaturePainter::KeySignaturePainter(const LayoutConstPtr &layout,
                                         const KeySignature &key,
                                         const ConstScoreLocation &location,
                                         const ScoreClickEvent &click_event)
    : ClickableItem(QCoreApplication::translate(
                        "ScoreArea", "Double-click to edit key signature."),
                    click_event, location, ScoreItem::KeySignature),
      myLayout(layout),
      myKeySignature(key),
      myMusicFont(MusicFont::getFont(MusicFont::DEFAULT_FONT_SIZE)),
      myBounds(0, -10, LayoutInfo::getWidth(myKeySignature),
               layout->getStdNotationStaffHeight())
{
    initAccidentalPositions();
}

void
KeySignaturePainter::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    ClickableItem::paint(painter, option, widget);

    painter->setFont(myMusicFont);

    // Draw the appropriate accidentals.
    if (myKeySignature.usesSharps())
        drawAccidentals(mySharpPositions, (ushort)MusicFont::AccidentalSharp, painter);
    else
        drawAccidentals(myFlatPositions, (ushort)MusicFont::AccidentalFlat, painter);
}

void KeySignaturePainter::adjustHeightOffset(QVector<double> &lst)
{
    for (auto &elem : lst)
    {
        elem -= myLayout->getTopStdNotationLine();
    }
}

void KeySignaturePainter::drawAccidentals(QVector<double> &positions,
                                          QChar accidental,
                                          QPainter *painter)
{
    // Display natural if a cancellation occurs.
    if (myKeySignature.isCancellation())
        accidental = (ushort)MusicFont::Natural;

    for (int i = 0; i < myKeySignature.getNumAccidentals(true); ++i)
    {
        painter->drawText(i * LayoutInfo::ACCIDENTAL_WIDTH,
                          positions.at(i), accidental);
    }
}

void KeySignaturePainter::initAccidentalPositions()
{
    myFlatPositions.resize(7);
    mySharpPositions.resize(7);

    int clefOffset = 0;
    if (myLocation.getStaff().getClefType() == Staff::BassClef)
    {
        clefOffset = 1;
    }

    // Generate the positions for the key signature accidentals.
    myFlatPositions.replace(0, myLayout->getStdNotationLine(3 + clefOffset));
    myFlatPositions.replace(1, myLayout->getStdNotationSpace(1 + clefOffset));
    myFlatPositions.replace(2, myLayout->getStdNotationSpace(3 + clefOffset));
    myFlatPositions.replace(3, myLayout->getStdNotationLine(2 + clefOffset));
    myFlatPositions.replace(4, myLayout->getStdNotationLine(4 + clefOffset));
    myFlatPositions.replace(5, myLayout->getStdNotationSpace(2 + clefOffset));
    myFlatPositions.replace(6, myLayout->getStdNotationSpace(4 + clefOffset));

    mySharpPositions.replace(0, myLayout->getStdNotationLine(1 + clefOffset));
    mySharpPositions.replace(1, myLayout->getStdNotationSpace(2 + clefOffset));
    mySharpPositions.replace(2, myLayout->getStdNotationSpace(0 + clefOffset));
    mySharpPositions.replace(3, myLayout->getStdNotationLine(2 + clefOffset));
    mySharpPositions.replace(4, myLayout->getStdNotationSpace(3 + clefOffset));
    mySharpPositions.replace(5, myLayout->getStdNotationSpace(1 + clefOffset));
    mySharpPositions.replace(6, myLayout->getStdNotationLine(3 + clefOffset));

    adjustHeightOffset(myFlatPositions);
    adjustHeightOffset(mySharpPositions);
}
