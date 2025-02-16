/*
  * Copyright (C) 2015 Cameron White
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
  
#ifndef APP_VIEWOPTIONS_H
#define APP_VIEWOPTIONS_H

#include <optional>
#include <array>
#include <vector>

/// Stores any view options that are not saved with the score (e.g. the current
/// zoom level or the active score filter).
class ViewOptions
{
public:
    static constexpr int MIN_ZOOM = 25;
    static constexpr int MAX_ZOOM = 300;
    static constexpr std::array<int, 13> ZOOM_LEVELS = { 25, 50, 70, 80, 90, 100, 110, 125, 150, 175, 200, 250, 300 };

public:
    ViewOptions();

    const std::optional<int> &getFilter() const { return myFilter; }
    void setFilter(int filter) { myFilter = filter; }
    void clearFilter() { myFilter.reset(); }

    int getZoom() const { return myZoom; }

    /// Fixates the zoom level. Note that we support two types of zoom
    /// changes - explicit level or one from a list
    bool setZoom(int percent);
    /// Increases to the next possible zoom level.
    bool increaseZoom();
    /// Increases to the previous possible zoom level.
    bool decreaseZoom();

    bool showStdStuff(size_t staffIndx) const;

	bool showTabStuff(size_t staffIndx) const;
	bool showNumberStuff(size_t staffIndx) const;

    std::vector<bool> &showStdStuffs();
	std::vector<bool>& showTabStuffs();
	std::vector<bool>& showNumberStuffs();

private:
    std::optional<int> myFilter;
    int myZoom;
    std::vector<bool> mShowStdStuffs;
    std::vector<bool> mShowTabStuffs;
    std::vector<bool> mShowNumberStuffs;
};

#endif
