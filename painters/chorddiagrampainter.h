/*
  * Copyright (C) 2022 Cameron White
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

#ifndef PAINTERS_CHORDDIAGRAMPAINTER_H
#define PAINTERS_CHORDDIAGRAMPAINTER_H

#include <QColor>
#include <QGraphicsItem>

class ChordDiagram;
class Score;
class ScoreClickEvent;
struct DiagramSizeInfo
{
	double DIAGRAM_WIDTH = 46;
	double DIAGRAM_HEIGHT = 55;
	double TOP_FRET_Y = 22;

	/// Size of the fret marker dots.
	double DOT_RADIUS = 1.5;
	/// Hit radius for clicking on fret markers.
	double HIT_RADIUS = 2.5;

	/// Height of the diagram, in frets.
	int NUM_FRETS = 6;
	/// Spacing between frets.
	double FRET_SPACING = 6.0;
	/// Spacing between strings.
	double STRING_SPACING = FRET_SPACING;

	/// Y position for drawing open / muted strings above the nut.
	double Y_ABOVE = TOP_FRET_Y - 0.6 * FRET_SPACING;
    void scale(double s)
    {
        DIAGRAM_WIDTH *= s;
        DIAGRAM_HEIGHT *= s;
        TOP_FRET_Y *= s;
        DOT_RADIUS *= s;
        HIT_RADIUS *= s;
        FRET_SPACING *= s;
        STRING_SPACING *= s;
        Y_ABOVE *= s;
    }
    void setHeightByScale(double height)
    {
        scale(height/ DIAGRAM_HEIGHT);
    }
};
/// Renders a single chord diagram.
/// Use the static renderDiagrams() method to layout and render all of the
/// diagrams in a score.
class ChordDiagramPainter : public QGraphicsObject
{
    Q_OBJECT

public:
    ChordDiagramPainter(const ChordDiagram &diagram, const QColor &color, DiagramSizeInfo size = DiagramSizeInfo());

    QRectF boundingRect() const override { return myBounds; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
               QWidget *) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /// Returns a group of all the chord diagrams, constrained to the provided
    /// width.
    static QGraphicsItem *renderDiagrams(const Score &score,
                                         const QColor &color,
                                         const ScoreClickEvent &click_event,
                                         double max_width,
        DiagramSizeInfo size = DiagramSizeInfo());
signals:
    void clicked(int string, int fret);

private:
    bool findFretAtPosition(const QPointF &pos, int &string, int &fret) const;

	DiagramSizeInfo mySize;
    const QRectF myBounds;
    const QColor myColor;
    const ChordDiagram &myDiagram;
    double myXPad = 0;
};

#endif
