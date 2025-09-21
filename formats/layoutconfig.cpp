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

#include "layoutconfig.h"

namespace LayoutConfig
{
    // Global variable to store the measures per system setting
    static int g_measuresPerSystem = 0; // 0 means use original layout
    
    void setMeasuresPerSystem(int measures)
    {
        g_measuresPerSystem = measures;
    }
    
    int getMeasuresPerSystem()
    {
        return g_measuresPerSystem;
    }
}