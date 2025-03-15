#include "TickPlayer.h"
#include <QAudioSink>
TickPlayer::TickPlayer(QObject *parent):QObject(parent)
{

}

void TickPlayer::init()
{
    auto allD = QMediaDevices::audioOutputs();
    if(allD.empty()){
        return;
    }
    QAudioDevice device = allD.at(0);
    if(!mTickOutPut)
    {
        QAudioSink sink;
        mTickOutPut= new QAudioOutput(this);
    }
    mTickOutPut->setDevice(device);
    if(!mTickPlayer)
    {
        mTickPlayer = new QMediaPlayer(this);
    }
    mTickPlayer->setAudioOutput(mTickOutPut);
    connect(mTickPlayer,&QMediaPlayer::playbackStateChanged,
               this,&TickPlayer::tickState);
}

void TickPlayer::pause()
{
    mTickPlayer->pause();
}

void TickPlayer::stop()
{
    myIsAutoStop = false;
    mTickPlayer->stop();
}

void TickPlayer::start()
{
    auto allD = QMediaDevices::audioOutputs();
    if(allD.empty()){
        return;
    }
    QAudioDevice device = allD.at(0);
    mTickOutPut->setDevice(device);

    mTickPlayer->play();
    myIsAutoStop = true;
}

void TickPlayer::setPlayerOpts(const QString &tickName, double rate)
{
    mTickName = tickName;
    mTickPlayer->setSource(tickName);
    mTickPlayer->setPlaybackRate(rate);
}

double TickPlayer::curPlayRate() const
{
    return mTickPlayer->playbackRate();
}

QString TickPlayer::curTickName() const
{
    return mTickName;
}

void TickPlayer::tickState(QMediaPlayer::PlaybackState state)
{
    if(!mTickPlayer){
        return;
    }
    if(!myIsAutoStop){
        return;
    }
    if(state==QMediaPlayer::PlaybackState::StoppedState)
    {
        mTickPlayer->play();
    }
}
