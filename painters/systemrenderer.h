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

#ifndef PAINTERS_SYSTEMRENDERER_H
#define PAINTERS_SYSTEMRENDERER_H

#include <map>
#include <painters/layoutinfo.h>
#include <painters/musicfont.h>
#include <QFontMetricsF>
#include <score/staff.h>
#include <QPalette>
#include <QPointF>
class QGraphicsItem;
class QGraphicsItemGroup;
class QGraphicsRectItem;
class Score;
class ScoreArea;
class ScoreLocation;
class System;
class ViewOptions;

class SystemRenderer
{
public:
    SystemRenderer(const ScoreArea *score_area, const Score &score,
                   const ViewOptions &view_options);

    QGraphicsItem *operator()(const System &system, int systemIndex);

private:
    /// Draws the tab clef.
    void drawTabClef(double x, const LayoutInfo &layout,
                     const ConstScoreLocation &location);

    /// Draws barlines, along with time signatures, rehearsal signs, etc.
    void drawBarlines(const ConstScoreLocation &location,
                      const LayoutConstPtr &layout);

    /// Draws the tab notes for all notes in the staff.
    void drawTabNotes(const Staff &staff, const LayoutConstPtr &layout,
        std::map<int,QPointF> &positionBottomPts,
        std::map<int, QPointF>& positionTopPts);

    /// Centers an item, by using its width to calculate the necessary
    /// offset from xmin.
    static void centerHorizontally(QGraphicsItem &item, double xmin,
                                   double xmax);

    /// Vertically centers a system symbol between y and y +
    /// LayoutInfo::SYSTEM_SYMBOL_SPACING.
    static void centerSymbolVertically(QGraphicsItem &item, double y);

    /// Draws a arpeggio up/down at the given position.
    void drawArpeggio(const Position &position, double x,
                      const LayoutInfo &layout);

    /// Draws system-level symbols such as alternate endings and tempo markers.
    void drawSystemSymbols(const ConstScoreLocation &location,
                           const LayoutInfo &layout);

    double drawSystemLyricsSymbols(const ConstScoreLocation &location,
                           const LayoutInfo &layout,double height,
                            bool showNumberStuff=false);
    /// Draws the bar number for the first bar in the system.
    void drawBarNumber(int systemIndex, const LayoutInfo &layout);

    /// Draws a divider line between system symbols.
    void drawDividerLine(double y);

    /// Draws all of the alternate endings in the system.
    void drawAlternateEndings(const ConstScoreLocation &location,
                              const LayoutInfo &layout, double height);

    /// Draws all of the tempo markers in the system.
    void drawTempoMarkers(const ConstScoreLocation &location,
                          const LayoutInfo &layout, double height);

    /// Draws all of the directions in the system.
    double drawDirections(const ConstScoreLocation &location,
                          const LayoutInfo &layout, double height);

    /// Draws all of the chord names in the system.
    int drawChordText(const ConstScoreLocation &location,
                       const LayoutInfo &layout, double height);

    /// Draws all of the text items in the system.
    void drawTextItems(const ConstScoreLocation &location,
                       const LayoutInfo &layout, double height);

    //int drawLyricsItems(const ConstScoreLocation &location,
    //                   const LayoutInfo &layout, double height);
    /// Draws the text symbols that appear below the tab staff
    /// (hammerons, slides, etc).
    void drawSymbolsBelowTabStaff(const LayoutInfo &layout);

    /// Creates a pick stroke symbol using the given character.
    QGraphicsItem *createPickStroke(const QString &text,double r=0,QPointF pos = QPointF(2,2));

    /// Creates a plain text item - useful for symbols that don't use the
    /// music font (hammerons, slides, etc).
    QGraphicsItem *createPlainTextSymbol(const QString &text,
                                         QFont::Style style);

    /// Draws symbols that appear above the standard notation staff (e.g. 8va).
    void drawSymbolsAboveStdNotationStaff(const LayoutInfo &layout);

    /// Draws symbols that are grouped across multiple positions
    /// (i.e. consecutive "let ring" symbols).
    QGraphicsItem *createConnectedSymbolGroup(const QString &text,
                                              QFont::Style style, double width,
                                              const LayoutInfo &layout);

    /// Create a dashed line in the given location.
    void createDashedLine(QGraphicsItemGroup *group, double left, double right,
                          double y);

    /// Draws symbols that appear below the standard notation staff (e.g. 8vb).
    void drawSymbolsBelowStdNotationStaff(const LayoutInfo &layout);

    /// Draws hammerons, pulloffs, etc in the tab staff.
    void drawLegato(const Staff &staff, const LayoutInfo &layout);

    /// Draws player changes for the given staff.
    void drawPlayerChanges(const ConstScoreLocation &location,
                           const LayoutInfo &layout);

    /// Draws the symbols that appear above the tab staff (e.g. vibrato).
    void drawSymbolsAboveTabStaff(const ConstScoreLocation &location,
                                  const LayoutInfo &layout);

    /// Draws a sequence of continuous music symbols (e.g. vibrato).
    QGraphicsItem *drawContinuousFontSymbols(QChar symbol, int width);

    /// Creates a tremolo picking symbol.
    QGraphicsItem *createTremoloPicking(const LayoutInfo &layout);

    /// Creates a trill symbol.
    QGraphicsItem *createTrill(const LayoutInfo &layout);

    /// Creates an artificial harmonic symbol.
    QGraphicsItem *createArtificialHarmonicText(const Position &position);

    /// Creates a dynamic symbol.
    QGraphicsItem *createDynamic(const ConstScoreLocation &location,
                                 const SymbolGroup &symbol_group);

    /// Creates a volume swell.
    QGraphicsItem *createVolumeSwell(const ConstScoreLocation &location,
                                     const SymbolGroup &group,
                                     const LayoutInfo &layout);

    /// Creates a tremolo bar symbol.
    QGraphicsItem *createTremoloBar(const ConstScoreLocation &location,
                                    const SymbolGroup &group,
                                    const LayoutInfo &layout);

    /// Draws a group of bends.
    void createBendGroup(const ConstScoreLocation &location,
                         const SymbolGroup &group, const LayoutInfo &layout);

    /// Draws a single bend.
    void createBend(QGraphicsItemGroup *group, double left, double right,
                    double yStart, double yEnd, int pitch, bool prebend);

    /// Draws notes, beams, and rests for a staff.
    void drawStdNotation(const ConstScoreLocation &location,
                         const LayoutInfo &layout);
	void drawTabStems(const LayoutInfo& layout,
		std::map<int, QPointF>& positionBottomPt);
	void drawStems(QGraphicsItem *parent,const LayoutInfo& layout,
		std::map<int, QPointF>& positionBottomPt,
        double stemBottomY,bool drawBeamOnly ,
        double beamLineWidth = 2.0,
        double extBeam = 0.0,
        bool singleBeamToLine = false);
    void drawTilesByPos(QGraphicsItem* parent, 
        const Staff& staff,const LayoutInfo& layout,
        const std::map<int, QPointF>& positionTopPt,
        double baseHeiht );
    int getTabStemExtraBeamHeight(const LayoutInfo& layout)const;
    /// Draws all ties in the voice.
    void drawTies(const Voice &voice, const std::vector<StdNotationNote> &notes,
                  const std::vector<NoteStem> &stems, const LayoutInfo &layout);

    /// Draws all irregular groups in the voice.
    void drawIrregularGroups(const Voice &voice,
                             const std::vector<NoteStem> &stems);

    /// Draws a multi-bar rest symbol.
    void drawMultiBarRest(const ConstScoreLocation &location,
                          const Barline &leftBar, const LayoutInfo &layout,
                          int measureCount);

    /// Draws a rest symbol.
    void drawRest(const Position &pos, double x, const LayoutInfo &layout,bool drawByStd = true);

    /// Draws ledger lines for all positions in the staff.
    void drawLedgerLines(const LayoutInfo &layout,
                         const std::map<int, double> &minNoteLocations,
                         const std::map<int, double> &maxNoteLocations,
                         const std::map<int, double> &noteHeadWidths);

    /// Draws all slides in a staff.
    void drawSlides(const Staff &staff, const LayoutInfo &layout,
                    ConstScoreLocation location);

    /// Draws a single slide between the given positions.
    void drawSlide(const LayoutInfo &layout, int string, bool slideUp,
                   int position1, int position2) const;

    const ScoreArea *myScoreArea;
    const Score &myScore;
    const ViewOptions &myViewOptions;

    QGraphicsRectItem *myParentSystem;
    QGraphicsItem *myParentStaff;

    QFont myMusicNotationFont;
    QFontMetricsF myMusicFontMetrics;
    QFont myPlainTextFont;
    QFont myLyricTextFont;
    QFont mySymbolTextFont;
    QFont myRehearsalSignFont;

    QPalette myPalette;
};

#endif
