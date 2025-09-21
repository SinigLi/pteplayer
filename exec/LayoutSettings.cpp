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

#include "LayoutSettings.h"
#include <formats/layoutconfig.h>
#include <app/settings.h>
#include <app/settingsmanager.h>

LayoutSettings::LayoutSettings(QObject *parent)
    : QObject(parent), m_measuresPerSystem(4)
{
    // Load initial value from settings
    SettingsManager &settings_manager = SettingsManager::instance();
    auto settings = settings_manager.getReadHandle();
    m_measuresPerSystem = settings->get(Settings::MeasuresPerSystem);
}

int LayoutSettings::measuresPerSystem() const
{
    return m_measuresPerSystem;
}

void LayoutSettings::setMeasuresPerSystem(int measures)
{
    if (m_measuresPerSystem != measures) {
        m_measuresPerSystem = measures;
        
        // Update the C++ settings
        SettingsManager &settings_manager = SettingsManager::instance();
        auto settings = settings_manager.getWriteHandle();
        settings->set(Settings::MeasuresPerSystem, measures);
        
        // Update the layout configuration immediately
        LayoutConfig::setMeasuresPerSystem(measures);
        
        emit measuresPerSystemChanged();
    }
}