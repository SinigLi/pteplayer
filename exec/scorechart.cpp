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
#include "scorechart.h"
#include <QPainter>
#include <app/PtePlayer.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <app/ScoreFileLoader.h>
//![0]
//QTimer timmer;
ScoreChart::ScoreChart(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    m_player = new PtePlayer(nullptr);
    // m_player->openFile("E:/Empty3.gp");
    // m_cur_show_rect = m_player->getStartInfoRect();
    connect(m_player, &PtePlayer::caretMvRect, this, &ScoreChart::_caretMvRect);
    connect(m_player, &PtePlayer::endPlayback, [this](){
        m_is_playing = false;
        _caretMvRect(m_player->getSene()->itemsBoundingRect());

        emit endPlayBack();
            });
    mCurSettings = std::make_shared<ScoreSettingInfos>();
    // connect(this,&QQuickItem::heightChanged,
    //         this,&ScoreChart::_scaleChanged);
}

ScoreChart::~ScoreChart()
{
    if (m_player)
    {
        delete m_player;
        m_player = nullptr;
    }
}

//![0]

// QString ScoreChart::name() const
// {
//     return m_name;
// }

// void ScoreChart::setName(const QString &name)
// {
//     m_name = name;
// }

// QColor ScoreChart::color() const
// {
//     return m_color;
// }

// void ScoreChart::setColor(const QColor &color)
// {
//     m_color = color;
// }

//![1]
void ScoreChart::paint(QPainter *painter)
{
	if (!m_player->hadOpenFile()) {
		return;
	}
    // QRectF curbox = boundingRect();
    // curbox = curbox.adjusted(10, 10, -10, -10);
    // painter->drawRect(curbox);
    m_player->getSene()->render(painter, boundingRect(), m_cur_show_rect,
                                Qt::AspectRatioMode::IgnoreAspectRatio);
    // QPen pen(QColor(255,0,0), 2);
    // painter->setPen(pen);
    // painter->setRenderHints(QPainter::Antialiasing, true);
    // painter->drawPie(boundingRect().adjusted(1, 1, -1, -1), 90 * 16, 290 * 16);
}

Q_INVOKABLE void
ScoreChart::startStopPlay()
{
	if (!m_player->hadOpenFile()) {
		return;
	}
    m_is_playing = !m_is_playing;
    m_player->startStopPlayback();
    if(!m_is_playing){
        _caretMvRect(m_player->getSene()->itemsBoundingRect());
        // m_cur_show_rect = ;
    }
}

Q_INVOKABLE void
ScoreChart::resetToStartPos()
{
	if (!m_player->hadOpenFile()) {
		return;
	}
    if(m_is_playing){
        startStopPlay();
        // m_player->startStopPlayback();
        // m_is_playing = !m_is_playing;
    }
    m_player->rewindPlaybackToStart();
	m_player->moveToBar(mCurSettings->start_bar - 1);
	_caretMvRect(m_player->getSene()->itemsBoundingRect());
}

int ScoreChart::getHeightByBaseWidth(int width,int height)
{
	if (!m_player->hadOpenFile()) {
		return height;
	}
    // if(m_cur_show_rect.isEmpty())
    // {
    //     m_cur_show_rect = m_player->getSene()->itemsBoundingRect();
    // }
    if(m_cur_show_rect.isEmpty()){
        qDebug()<<" m_cur_show_rect is empty!";
        return height;
    }
    double widthRatio = (double)width/(double)m_cur_show_rect.width();
    // QRectF rectf = m_player->getSene()->itemsBoundingRect();

    int curheight = widthRatio*m_cur_show_rect.height();
    if(m_is_playing){
        qDebug()<<" is playing!";
        if(height>curheight){
            return curheight;
        }
        return height;
    }
    return curheight;
}

QString getScoreFilePath(const QString &groupname, const QString &scoreName)
{

    QStringList scoreNames;
    QStringList scorePaths;
    ScoreFileLoader::instace().getScoreNames(groupname,scoreNames,scorePaths);
    for(int i =0;i<scoreNames.size();++i)
    {
        if(scoreNames[i].trimmed().toUpper()==scoreName.trimmed().toUpper())
        {
            return scorePaths[i];
        }
    }
    return "";
}
QString ScoreChart::openScore(const QString &scoreGroup, const QString &scoreName)
{
    QString scorePath = getScoreFilePath(scoreGroup,scoreName);
    if(scorePath.isEmpty()){
        return "open score err "+scoreGroup +" "+scoreName;
    }
    mCurGroup = scoreGroup;
    mCurScoreName = scoreName;


	ScoreSettingInfos infos;
	// bool hadPreSet = true;
    bool orgHadSetInfo = false;
    if (ScoreFileLoader::instace().getScoreSetting(mCurGroup, mCurScoreName, infos))
    {
        orgHadSetInfo = true;
    }

    m_player->openFile(scorePath, infos.player_infos);
    _caretMvRect(m_player->getSene()->itemsBoundingRect());

    // qDebug()<<"openScore 2";
    //ScoreSettingInfos infos;
    // bool hadPreSet = true;
    if(!orgHadSetInfo)
    {
        infos.bpm = orgBpRate();
        infos.start_bar = 1;
        infos.player_infos = m_player->orgPlayerSetInfos();
        // hadPreSet = false;
    }else {
        if(infos.player_infos.empty())
        {
            infos.player_infos = m_player->orgPlayerSetInfos();
        }
    }
    mCurSettings = std::make_shared<ScoreSettingInfos>(infos);
    // qDebug()<<"openScore 3";

    m_player->setSpeedBpm(mCurSettings->bpm);
    m_player->rewindPlaybackToStart();
    m_player->moveToBar(mCurSettings->start_bar-1);
    // if(hadPreSet){
    // }
    return "";

}

int ScoreChart::startBarNum()
{
	if (!m_player->hadOpenFile()) {
		return 1;
	}
    return mCurSettings->start_bar;
}

void ScoreChart::setStartBar(int num)
{
	if (!m_player->hadOpenFile()) {
		return;
	}
    mCurSettings->start_bar = num;
    ScoreFileLoader::instace().setScoreSetting(mCurGroup,mCurScoreName,*mCurSettings);
}

int ScoreChart::allBarCount()
{
	if (!m_player->hadOpenFile()) {
		return 0;
	}
    return m_player->allBarCount();
}

int ScoreChart::curBpRate()
{
	if (!m_player->hadOpenFile()) {
		return 0;
	}
    return mCurSettings->bpm;
}

int ScoreChart::orgBpRate()
{
	if (!m_player->hadOpenFile()) {
		return 0;
	}
    return m_player->orgBaseBpm();
}

void ScoreChart::setBpRate(int num)
{
	if (!m_player->hadOpenFile()) {
		return;
	}
    mCurSettings->bpm = num;
    m_player->setSpeedBpm(num);
    ScoreFileLoader::instace().setScoreSetting(mCurGroup,mCurScoreName,*mCurSettings);
}

void ScoreChart::setPlayerSetting(QVariantList vlist)
{
    mCurSettings->player_infos.clear();
    for (const QVariant &one : vlist)
    {
        QVariantMap onemap = one.toMap();
        ScoreSettingInfos::PlayerSetInfo pinfo;
        pinfo.name = onemap["name"].toString();
        pinfo.tab_stuff_show = onemap["tab_stuff_show"].toBool();
		pinfo.std_stuff_show = onemap["std_stuff_show"].toBool();
		pinfo.number_stuff_show = onemap["number_stuff_show"].toBool();
        pinfo.activated = onemap["activated"].toBool();
        mCurSettings->player_infos.push_back(pinfo);
    }
    ScoreFileLoader::instace().setScoreSetting(mCurGroup,mCurScoreName,*mCurSettings);
    m_player->setPlayerSetInfos(mCurSettings->player_infos);
    // _caretMvRect(m_player->getSene()->itemsBoundingRect());
    // auto type = vlist.metaType();
    // auto typenam = vlist.typeName();
    // auto list= vlist.value<QVariantList>();
    // bool pause = true;
}

QVariantList ScoreChart::orgPlayerSetting() const
{
    QVariantList result;
    for(int i =0 ;i<mCurSettings->player_infos.size();++i)
    {
        const auto &one=mCurSettings->player_infos.at(i);
        QVariantMap onevalue;
        onevalue["name"]=one.name;
        onevalue["tab_stuff_show"]=one.tab_stuff_show;
		onevalue["std_stuff_show"] = one.std_stuff_show;
		onevalue["number_stuff_show"] = one.number_stuff_show;
        onevalue["activated"]=one.activated;
        result.push_back(onevalue);

    }
    return result;

}

QVariantList ScoreChart::curPlayerSetting() const
{
    QVariantList result;
    for(int i =0 ;i<mCurSettings->player_infos.size();++i)
    {
        const auto &one=mCurSettings->player_infos.at(i);
        QVariantMap onevalue;
        onevalue["name"]=one.name;
        onevalue["tab_stuff_show"]=one.tab_stuff_show;
		onevalue["std_stuff_show"] = one.std_stuff_show;
		onevalue["number_stuff_show"] = one.number_stuff_show;
        onevalue["activated"]=one.activated;
        result.push_back(onevalue);
    }
    return result;

    // QVariantList result;
    // QVariantMap onevalue;
    // onevalue["name"]="吉他2";
    // onevalue["tab_stuff_show"]=true;
    // onevalue["std_stuff_show"]=true;
    // onevalue["activated"]=true;
    // result.push_back(onevalue);
    // onevalue["name"]="旋律2";
    // result.push_back(onevalue);

    // return result;
}

double ScoreChart::getCurPosPosition()
{
	if (!m_player->hadOpenFile()) {
		return 0;
	}
    double topy = m_player->getCurCaretRect().top();
    QRectF allRect = m_player->getSene()->itemsBoundingRect();
    if(allRect.height()<1e-3){
        return 0;
    }
    return (topy-allRect.top())/allRect.height();
}

void ScoreChart::caculateSysSizeByHeight(int width,int height)
{
    if (!m_player->hadOpenFile()) {
        return;
    }
    QSizeF syssize = m_player->getAvgSystemSize();
    if(syssize.height()<=1e-6
        ||syssize.width()<=1e-6){
        return;
    }
    double widthRatio = (double)width/(double)syssize.width();
    // QRectF rectf = m_player->getSene()->itemsBoundingRect();

    double curheight = widthRatio*syssize.height();

    int syscount = std::round((double)height/curheight);
    if(syscount<=0){
        syscount =1;
    }
    m_player->setPerShowSysCount(syscount);
}


void
ScoreChart::_caretMvRect(QRectF rect)
{
    m_cur_show_rect = rect;
    emit viewHadChanged();
    // this->update();
}


//![1]

