#ifndef TICKPLAYER_H
#define TICKPLAYER_H

#include <QObject>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QAudioOutput>

class TickPlayer:public QObject
{
    Q_OBJECT
public:
    TickPlayer(QObject *parent = nullptr);
    void init();
    void pause();
    void stop();
    void start();

    void setPlayerOpts(const QString &tickName,double rate);

    double curPlayRate()const;
    QString curTickName()const;
    // void pausePlayer();
    // void continuePlayer();
    // void rePlayer(const QString &tickName,double rate);
    // double curPlayRate()const;
    // void setPlayRate(double rate);
public slots:
    void tickState(QMediaPlayer::PlaybackState state);
private:
    QString mTickName;

    bool myIsAutoStop = true;
    QMediaPlayer *mTickPlayer = nullptr;
    QAudioOutput *mTickOutPut = nullptr;
};

#endif // TICKPLAYER_H
