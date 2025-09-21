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

#ifndef EXEC_LAYOUTSETTINGS_H
#define EXEC_LAYOUTSETTINGS_H

#include <QObject>
#include <QQmlEngine>

class LayoutSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int measuresPerSystem READ measuresPerSystem WRITE setMeasuresPerSystem NOTIFY measuresPerSystemChanged)

public:
    explicit LayoutSettings(QObject *parent = nullptr);

    int measuresPerSystem() const;
    void setMeasuresPerSystem(int measures);

signals:
    void measuresPerSystemChanged();

private:
    int m_measuresPerSystem;
};

#endif // EXEC_LAYOUTSETTINGS_H