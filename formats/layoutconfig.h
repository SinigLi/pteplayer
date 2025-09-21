/*
 * Copyright (C) 2024 Cameron White
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

#ifndef FORMATS_LAYOUTCONFIG_H
#define FORMATS_LAYOUTCONFIG_H

namespace LayoutConfig
{
    /// Set the default number of measures per system for imported files.
    /// @param measures Number of measures per system (0 = use original layout)
    void setMeasuresPerSystem(int measures);
    
    /// Get the current measures per system setting.
    /// @return Number of measures per system (0 = use original layout)
    int getMeasuresPerSystem();
}

#endif