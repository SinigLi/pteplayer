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

#ifndef EXEC_LAYOUTMANAGER_H
#define EXEC_LAYOUTMANAGER_H

#include <QObject>
#include <QQmlEngine>

class LayoutManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static LayoutManager *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Q_INVOKABLE int getMeasuresPerSystem();
    Q_INVOKABLE void setMeasuresPerSystem(int measures);

private:
    explicit LayoutManager(QObject *parent = nullptr);
};

#endif // EXEC_LAYOUTMANAGER_H