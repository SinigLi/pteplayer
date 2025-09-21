/*
 * Copyright (C) 2020 Cameron White
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

#ifndef FORMATS_GP7_TO_PT2_H
#define FORMATS_GP7_TO_PT2_H

class Score;

namespace Gp7
{
struct Document;

/// Converts the Guitar Pro document into the provided score.
void convert(const Gp7::Document &doc, Score &score);

/// Converts the Guitar Pro document into the provided score with custom layout.
/// @param measures_per_system Number of measures per system (line). If 0, use original layout.
void convert(const Gp7::Document &doc, Score &score, int measures_per_system);

} // namespace Gp7

#endif
