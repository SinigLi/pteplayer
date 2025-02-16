#ifndef PTEPLAYER_H
#define PTEPLAYER_H

#include <QMainWindow>

#include <memory>
#include <score/dynamic.h>
#include <score/position.h>
#include <string>
#include <vector>
// #include <QAudioDevice>
#include "ScoreFileLoader.h"
//#ifndef _WIN32
// #include <QAudioDevice>
// #include <QMediaDevices>
// #include <QMediaPlayer>
// #include <QAudioOutput>
#include "TickPlayer.h"

//#endif
class Caret;
class Command;
class DocumentManager;
class FileFormatManager;
class Instrument;
class InstrumentPanel;
class ToolBox;
class MidiPlayer;
class Mixer;
class PlaybackWidget;
class Player;
class QActionGroup;
class QThread;
class RecentFiles;
class ScoreArea;
class ScoreLocation;
class SettingsManager;
class TuningDictionary;
class UndoManager;

namespace Ui
{
class PtePlayer;
}
class QGraphicsScene;
class PtePlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit PtePlayer(QWidget *parent = nullptr);
    ~PtePlayer();
    QGraphicsScene *getSene() const;
    QRectF getStartInfoRect() const;
    void setPerShowSysCount(int count);
    int perShowSysCount() const;
    QRectF getCurCaretRect();
    QSizeF getAvgSystemSize()const;
    int orgBaseBpm()const;
    int allBarCount()const;
    void setSpeedBpm(int bpm);
    void moveToBar(int startBar);
    bool hadOpenFile()const;
    QList<ScoreSettingInfos::PlayerSetInfo> orgPlayerSetInfos()const;
    void setPlayerSetInfos(const QList<ScoreSettingInfos::PlayerSetInfo> &setInfos,bool reRander = true);
signals:
    void caretMvRect(QRectF rect);
    void endPlayback();
public slots:
    void openFile(QString filename, const QList<ScoreSettingInfos::PlayerSetInfo>& setInfos= QList<ScoreSettingInfos::PlayerSetInfo>());
    void moveCaretToSystem(int system);
    void moveCaretToPosition(int position);
    void startStopPlayback(bool from_measure_start = false);

    void moveCaretToPrevBar();
    void moveCaretToNextBar();
    void moveCaretToPrevStaff();
    void enableEditing(bool enable);
    void redrawScore();
    void moveCaretToFirstSection();
    void moveCaretToStart();
    void on_btn_last_song_clicked();

    void on_btn_setting_clicked();

    void on_btn_next_song_clicked();
    bool closeTab();

    void scorePerFormCountInStart();
    void scorePerFormCountInEnd();
    void playerBackStart();
// #ifdef _WIN32

// #else
//     void tickState(QMediaPlayer::PlaybackState state)
//     {
//         if(!mTickPlayer){
//             return;
//         }
//         if(state==QMediaPlayer::PlaybackState::StoppedState)
//         {
//             mTickPlayer->play();
//         }
//     }
//     #endif

private:
    Ui::PtePlayer *ui;
    void _getBaseTempoInfo(QString &tickName, double &rate) const;
//#ifndef _WIN32
    // QMediaPlayer *mTickPlayer = nullptr;
    // QAudioOutput *mTickOutPut = nullptr;
    TickPlayer * myTickPlayer = nullptr;
    std::unique_ptr<QThread> myTickThread;
//#endif
    void createMidiThread();

    std::unique_ptr<SettingsManager> mySettingsManager;
    std::unique_ptr<DocumentManager> myDocumentManager;
    std::unique_ptr<FileFormatManager> myFileFormatManager;
    std::unique_ptr<QThread> myMidiThread;
    MidiPlayer *myMidiPlayer = nullptr;
    std::unique_ptr<TuningDictionary> myTuningDictionary;
    /// Tracks whether we are currently in playback mode.
    bool myIsPlaying;
    int myLastBpm = 0;
    int myLastBar = 0;
    int myPerShowSysCount = 2;
    /// Flag for whether a score click event is being handled.
    bool myIsHandlingClick = false;
    /// Tracks the last directory that a file was opened from.
    QString myPreviousDirectory;
    // RecentFiles *myRecentFiles;
    Position::DurationType myActiveDurationType;
    // QTabWidget *myTabWidget;
    ScoreArea * mScoreArea=nullptr;;
    // Mixer *myMixer;
    PlaybackWidget *myPlaybackWidget;
    QWidget *myPlaybackArea;
    bool mCurShowFull = false;
    // QMenu *myRecentFilesMenu;

    Command *myPlayPauseCommand;
    Command *myPlayFromStartOfMeasureCommand;
    Command *myStopCommand;
    Command *myRewindCommand;
    Command *myMetronomeCommand;
    Command *myCountInCommand;

    QString m_cur_scoreset;
    QStringList m_cur_files;
    QStringList m_cur_score_names;
    int m_cur_file_idx=-1;

    QString getApplicationName() const;
    void createTickThread();
    Caret &getCaret();
    ScoreLocation &getLocation();

    void updateCommands();
    void createTabArea();
    void setScoreZoom(int percent, bool pb_widget_update=false);
    // void createMixer();
    void applyZoomChange(bool pb_widget_update=false);
    ScoreArea *getScoreArea();
    void updateActiveVoice(int);
    void updateActiveFilter(int);
    void createCommands();
    void stopPlayback();
    void toggleMetronome();
    void toggleCountIn();

    void updateLocationLabel();
    void setupNewTab();
    void setPreviousDirectory(const QString &fileName);
    void setCurFileName();



    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void resetFullScreen();
    void rewindPlaybackToStart();
};

#endif // PTEPLAYER_H
