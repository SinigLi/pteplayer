/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef SCORECHART_H
#define SCORECHART_H

//![0]
#include <QtQuick/QQuickPaintedItem>
#include <QColor>
#include <QVariantList>
class PtePlayer;
struct ScoreSettingInfos;
class ScoreChart : public QQuickPaintedItem
{
    Q_OBJECT
    // Q_PROPERTY(QString name READ name WRITE setName FINAL)
    //Q_PROPERTY(QColor color READ color WRITE setColor)
    QML_ELEMENT
public:
    ScoreChart(QQuickItem *parent = 0);
    ~ScoreChart();
    // QString name() const;
    // void setName(const QString &name);

    // QColor color() const;
    // void setColor(const QColor &color);

    void paint(QPainter *painter);

    Q_INVOKABLE void startStopPlay();
    Q_INVOKABLE void resetToStartPos();
    Q_INVOKABLE int getHeightByBaseWidth(int width,int height);
    Q_INVOKABLE QString openScore(const QString &scoreGroup,const QString &scoreName);

    Q_INVOKABLE double getCurPosPosition();

    Q_INVOKABLE void caculateSysSizeByHeight(int width,int height);

    Q_INVOKABLE int startBarNum();
    Q_INVOKABLE void setStartBar(int num);
    Q_INVOKABLE int allBarCount();

    Q_INVOKABLE int curBpRate();
    Q_INVOKABLE int orgBpRate();
    Q_INVOKABLE void setBpRate(int num);


    Q_INVOKABLE void setPlayerSetting(QVariantList vlist);
    Q_INVOKABLE QVariantList orgPlayerSetting()const;
    Q_INVOKABLE QVariantList curPlayerSetting()const;

    Q_INVOKABLE int curMeasuresPerSystem();
    Q_INVOKABLE void setMeasuresPerSystem(int measures);

    // Q_INVOKABLE void setBaseScoreGroup(const QString &scoreGroup);
signals:
    void viewHadChanged();
    void endPlayBack();
private:
    // QString m_name;
    // QColor m_color;
    PtePlayer * m_player = nullptr;
    QRectF m_cur_show_rect;
    // QRectF m_full_scene_rect;
    bool m_is_playing = false;
    QString mCurGroup;
    QString mCurScoreName;
    std::shared_ptr<ScoreSettingInfos> mCurSettings;
private slots:

    void _caretMvRect(QRectF rect);
    // void _scaleChanged();
};
//![0]

#endif

