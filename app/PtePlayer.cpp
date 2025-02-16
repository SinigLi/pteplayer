#include "PtePlayer.h"
#include "ui_PtePlayer.h"

#include <actions/addalternateending.h>
#include <actions/addbarline.h>
#include <actions/addchordtext.h>
#include <actions/adddirection.h>
#include <actions/adddynamic.h>
#include <actions/addinstrument.h>
#include <actions/addirregulargrouping.h>
#include <actions/addmultibarrest.h>
#include <actions/addnote.h>
#include <actions/addnoteproperty.h>
#include <actions/addplayer.h>
#include <actions/addpositionproperty.h>
#include <actions/addrest.h>
#include <actions/addspecialnoteproperty.h>
#include <actions/addstaff.h>
#include <actions/addsystem.h>
#include <actions/adjustlinespacing.h>
#include <actions/chorddiagram.h>
#include <actions/editbarline.h>
#include <actions/editdynamic.h>
#include <actions/editfileinformation.h>
#include <actions/editinstrument.h>
#include <actions/editkeysignature.h>
#include <actions/editnoteduration.h>
#include <actions/editplayer.h>
#include <actions/editplayerchange.h>
#include <actions/editrehearsalsign.h>
#include <actions/editstaff.h>
#include <actions/edittabnumber.h>
#include <actions/edittempomarker.h>
#include <actions/edittextitem.h>
#include <actions/edittimesignature.h>
#include <actions/editviewfilters.h>
#include <actions/polishscore.h>
#include <actions/polishsystem.h>
#include <actions/removealternateending.h>
#include <actions/removebarline.h>
#include <actions/removechordtext.h>
#include <actions/removedirection.h>
#include <actions/removedynamic.h>
#include <actions/removeinstrument.h>
#include <actions/removeirregulargrouping.h>
#include <actions/removenote.h>
#include <actions/removenoteproperty.h>
#include <actions/removeplayer.h>
#include <actions/removeposition.h>
#include <actions/removepositionproperty.h>
#include <actions/removespecialnoteproperty.h>
#include <actions/removestaff.h>
#include <actions/removesystem.h>
#include <actions/shiftpositions.h>
#include <actions/shiftstring.h>
#include <actions/tremolobar.h>
#include <actions/undomanager.h>
#include <actions/volumeswell.h>

#include <app/appinfo.h>
#include <app/caret.h>
#include <app/clipboard.h>
#include <app/command.h>
#include <app/documentmanager.h>
#include <app/paths.h>
#include <app/recentfiles.h>
#include <app/scorearea.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <app/tuningdictionary.h>

#include <audio/midiplayer.h>
#include <audio/settings.h>

#include <chrono>

#include <dialogs/alterationofpacedialog.h>
#include <dialogs/alternateendingdialog.h>
#include <dialogs/artificialharmonicdialog.h>
#include <dialogs/barlinedialog.h>
#include <dialogs/benddialog.h>
#include <dialogs/bulkconverterdialog.h>
#include <dialogs/chorddiagramdialog.h>
#include <dialogs/chordnamedialog.h>
#include <dialogs/directiondialog.h>
#include <dialogs/dynamicdialog.h>
#include <dialogs/fileinformationdialog.h>
#include <dialogs/gotobarlinedialog.h>
#include <dialogs/gotorehearsalsigndialog.h>
#include <dialogs/infodialog.h>
#include <dialogs/irregulargroupingdialog.h>
#include <dialogs/keyboardsettingsdialog.h>
#include <dialogs/keysignaturedialog.h>
#include <dialogs/lefthandfingeringdialog.h>
#include <dialogs/multibarrestdialog.h>
#include <dialogs/playerchangedialog.h>
#include <dialogs/preferencesdialog.h>
#include <dialogs/rehearsalsigndialog.h>
#include <dialogs/staffdialog.h>
#include <dialogs/tappedharmonicdialog.h>
#include <dialogs/tempomarkerdialog.h>
#include <dialogs/textitemdialog.h>
#include <dialogs/timesignaturedialog.h>
#include <dialogs/tremolobardialog.h>
#include <dialogs/trilldialog.h>
#include <dialogs/tuningdictionarydialog.h>
#include <dialogs/viewfilterdialog.h>
#include <dialogs/volumeswelldialog.h>

#include <formats/fileformatmanager.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QScrollArea>
#include <QTabBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QVBoxLayout>
//#ifndef WIN32

#include <QtCore5Compat/QRegExp>


//#endif // WIN32


#include <score/dynamic.h>
#include <score/utils.h>
#include <score/voiceutils.h>

#include <util/tostring.h>
#include <util/version.h>

#include <widgets/instruments/instrumentpanel.h>
#include <widgets/mixer/mixer.h>
#include <widgets/playback/playbackwidget.h>
#include <widgets/toolbox/toolbox.h>

#include <QActionGroup>
#include "SettingDialog.h"
#include <QStyle>
// #include <QtCore/private/qandroidextras_p.h>
// #include <fstream>
PtePlayer::PtePlayer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PtePlayer),
      mySettingsManager(new SettingsManager()),
      myDocumentManager(new DocumentManager()),
      myFileFormatManager(new FileFormatManager(*mySettingsManager)),
      myTuningDictionary(new TuningDictionary()),
      myIsPlaying(false),
      myActiveDurationType(Position::EighthNote),
      // myTabWidget(nullptr),
      // myMixer(nullptr),
      myPlaybackWidget(nullptr),
      myPlaybackArea(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":icons/app_icon.png"));
    setAcceptDrops(true);

           // Load the music notation font.
    QFontDatabase::addApplicationFont(":fonts/emmentaler-13.otf");
    // Load the tab note font.
    QFontDatabase::addApplicationFont(":fonts/LiberationSans-Regular.ttf");
    QFontDatabase::addApplicationFont(":fonts/LiberationSerif-Regular.ttf");

    myTuningDictionary->loadInBackground();
    mySettingsManager->load(Paths::getConfigDir());

    createMidiThread();
    createTickThread();
    // createMixer();
    // createInstrumentPanel();
    createCommands();
    // loadKeyboardShortcuts();
    // createMenus();
    // createToolBox();
    // myRecentFilesMenu = menuBar()->addMenu("temp files");
           // Set up the recent files menu.
    // myRecentFiles =
    //     new RecentFiles(*mySettingsManager, myRecentFilesMenu, this);
    // connect(myRecentFiles, &RecentFiles::fileSelected, this,
    //         &PtePlayer::openFile);

    createTabArea();
    {
        auto settings = mySettingsManager->getReadHandle();
        myPreviousDirectory =
            QString::fromStdString(settings->get(Settings::PreviousDirectory));

        // Restore the state of any dock widgets.
restoreState(settings->get(Settings::WindowState));
    }

    // setCentralWidget(myPlaybackArea);
    // setMinimumSize(800, 600);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(getApplicationName());
    //openFile(R"(D:\Users\sinigli\Desktop\烟火里的尘埃 - 5copo.gp)");
    ui->btn_last_song->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->btn_next_song->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));


}

QString PtePlayer::getApplicationName() const
{
    QString name = QString("%1 %2 Beta").arg(
        AppInfo::APPLICATION_NAME,
        AppInfo::APPLICATION_VERSION);

    name += QString::fromStdString(Version::get());

    return name;
}

void PtePlayer::openFile(QString filename, const QList<ScoreSettingInfos::PlayerSetInfo>& setInfos)
{
    assert(!filename.isEmpty());
    QFileInfo fileInfo(filename);
    // QString realFileName = getRealPathFromUri(filename);// fileInfo.filesystemAbsoluteFilePath();
    // qDebug() << "Opening file: org name:" <<filename <<" real name:"<< realFileName;
    auto path = Paths::fromQString(filename);

    int validationResult = myDocumentManager->findDocument(path);
    if (validationResult > -1)
    {
        closeTab();
        // qDebug() << "File: " << filename << " is already open";
        // myTabWidget->setCurrentIndex(validationResult);
        // return;
    }

    auto start = std::chrono::high_resolution_clock::now();


    std::optional<FileFormat> format = myFileFormatManager->findFormat(
        fileInfo.suffix().toStdString());

    if (!format)
    {
        QMessageBox::warning(this, tr("Error Opening File"),
            tr("Unsupported file type."));
        return;
    }

    try
    {
        Document& doc = myDocumentManager->addDocument(*mySettingsManager);

        myFileFormatManager->importFile(doc.getScore(), path, *format);
        Score& score = doc.getScore();
        for (auto& oneplayer : score.getPlayers())
        {
            oneplayer.setMaxVolume(0);
        }
        auto end = std::chrono::high_resolution_clock::now();
        qDebug() << "File loaded in"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms";

        doc.setFilename(path);
        setPreviousDirectory(filename);
        // myRecentFiles->add(filename);
        setPlayerSetInfos(setInfos, false);
        setupNewTab();
    }
    catch (const std::exception& e)
    {
        myDocumentManager->removeDocument(
            myDocumentManager->getCurrentDocumentIndex());

        QMessageBox::warning(
            this, tr("Error Opening File"),
            tr("Error opening file: %1").arg(QString(e.what())));
    }
}

void PtePlayer::updateLocationLabel()
{
    myPlaybackWidget->updateLocationLabel(
        Util::toString(getCaret().getLocation()));
}
void PtePlayer::setupNewTab()
{
    auto start = std::chrono::high_resolution_clock::now();
    qDebug() << "Tab creation started ...";

    Q_ASSERT(myDocumentManager->hasOpenDocuments());
    Document& doc = myDocumentManager->getCurrentDocument();
    if (doc.getViewOptions().showStdStuffs().empty())
    {
		for (auto oneplayer : doc.getScore().getPlayers())
		{
			QString playstr = QString::fromStdString(oneplayer.getDescription());
			doc.getViewOptions().showStdStuffs().push_back(false);
			doc.getViewOptions().showNumberStuffs().push_back(false);
			if (playstr.contains(QString::fromWCharArray(L"旋律")))
			{
				doc.getViewOptions().showTabStuffs().push_back(false);
			}
			else
			{
				doc.getViewOptions().showTabStuffs().push_back(true);
			}

		}
    }
    //doc.getViewOptions().showStdStuffs().push_back(false);
    //doc.getViewOptions().showStdStuffs().push_back(false);

    //doc.getViewOptions().showTabStuffs().push_back(true);
    //doc.getViewOptions().showTabStuffs().push_back(false);

    doc.getCaret().subscribeToChanges([=]() {
        updateCommands();
        updateLocationLabel();

               // When changing location to somewhere on the staff, clear any existing
               // selected item.
        if (getCaret().getSelectedItem() == ScoreItem::Staff ||
            !myIsHandlingClick)
        {
            getScoreArea()->clearSelection();
        }
    });
    if(mScoreArea)
    {
        delete mScoreArea;
        mScoreArea = nullptr;
    }
    auto scorearea = new ScoreArea(*mySettingsManager, this);
    scorearea->renderDocument(doc);
    scorearea->installEventFilter(this);
    mScoreArea = scorearea;
    // myUndoManager->addNewUndoStack();

    QString filename = "Untitled";
    if (doc.hasFilename())
        filename = Paths::toQString(doc.getFilename());

    QFileInfo fileInfo(filename);

           // Create title for the tab bar.
    QString title = fileInfo.fileName();
    QFontMetrics fm (mScoreArea->font());

           // Each tab is 200px wide, so we want to shorten the name if it's wider
           // than 140px.
    bool chopped = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
    while (fm.horizontalAdvance(title) > 140)
#else
    while (fm.width(title) > 140)
#endif
    {
        title.chop(1);
        chopped = true;
    }

    if (chopped)
        title.append("...");

    // const int tabIndex = myTabWidget->addTab(scorearea, title);
    // myTabWidget->setTabToolTip(tabIndex, fileInfo.fileName());
    ui->layout_player->addWidget(mScoreArea);
    mScoreArea->setEnabled(false);
    // myMixer->reset(doc.getScore());
    // myInstrumentPanel->reset(doc.getScore());
    myPlaybackWidget->reset(doc);
    // myIsPausing = false;
    const Score &score = myDocumentManager->getCurrentDocument().getScore();
    myLastBpm = score.getBasicRateBp();
    myLastBar = 0;
//#ifndef _WIN32
    myTickPlayer->stop();
    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::stop,
    //                           Qt::QueuedConnection);
    // if(mTickPlayer)
    // {
    //     mTickPlayer->pause();
    // }

    //#endif
           // Switch to the new document.
    // myTabWidget->setCurrentIndex(myDocumentManager->getCurrentDocumentIndex());
    myPlaybackWidget->setEnabled(true);

    enableEditing(true);
    updateCommands();
    scorearea->setFocus();

    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Tab opened in"
             << std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count() << "ms";
    // scorearea->scroolXToMiddle();
}

void PtePlayer::setPreviousDirectory(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    myPreviousDirectory = fileInfo.absolutePath();

    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::PreviousDirectory,
                  myPreviousDirectory.toStdString());
}
void PtePlayer::createCommands()
{

    myPlayPauseCommand = new Command(tr("Play"), "Playback.PlayPause",
                                     Qt::Key_Space, this);
    connect(myPlayPauseCommand, &QAction::triggered, this,
            &PtePlayer::startStopPlayback);
#ifdef Q_OS_MAC
    // Command-Space is used by Spotlight.
    QKeySequence play_start_seq = Qt::META + Qt::Key_Space;
#else
    QKeySequence play_start_seq = Qt::CTRL + Qt::Key_Space;
#endif
    myPlayFromStartOfMeasureCommand = new Command(
        tr("Play From Start Of Measure"), "Playback.PlayFromStartOfMeasure",
        play_start_seq, this);
    connect(myPlayFromStartOfMeasureCommand, &QAction::triggered, [this]() {
        startStopPlayback(/* from_measure_start */ true);
    });

    myStopCommand =
        new Command(tr("Stop"), "Playback.Stop", Qt::ALT + Qt::Key_Space, this);
    connect(myStopCommand, &QAction::triggered, this,
            &PtePlayer::stopPlayback);

    myRewindCommand = new Command(tr("Rewind"), "Playback.Rewind",
                                  Qt::ALT + Qt::Key_Left, this);
    connect(myRewindCommand, &QAction::triggered, this,
            &PtePlayer::rewindPlaybackToStart);

    myMetronomeCommand = new Command(tr("Metronome"), "Playback.Metronome",
                                     QKeySequence(), this);
    myMetronomeCommand->setCheckable(true);
    connect(myMetronomeCommand, &QAction::triggered, this,
            &PtePlayer::toggleMetronome);


    myCountInCommand = new Command(tr("Count-In"), "Playback.CountIn",
                                   QKeySequence(), this);
    myCountInCommand->setCheckable(true);
    connect(myCountInCommand, &QAction::triggered, this,
            &PtePlayer::toggleCountIn);
}

void PtePlayer::toggleCountIn()
{
    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::CountInEnabled, myCountInCommand->isChecked());
}

void PtePlayer::toggleMetronome()
{
    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::MetronomeEnabled, myMetronomeCommand->isChecked());
}

void PtePlayer::moveCaretToFirstSection()
{
    getCaret().moveToFirstSystem();
}

void PtePlayer::moveCaretToStart()
{
    getCaret().moveToStartPosition();
}

void PtePlayer::rewindPlaybackToStart()
{
    const bool wasPlaying = myIsPlaying;

    if (wasPlaying)
        startStopPlayback();

    moveCaretToFirstSection();
    moveCaretToStart();
//#ifndef _WIN32

    myTickPlayer->stop();
    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::stop,
    //                           Qt::QueuedConnection);
//#endif // !_WIN32

    //emit caretMvRect(getCurCaretRect());

    // myIsPausing = false;
    // qDebug()<<"rewindPlaybackToStart";
    if (wasPlaying)
        startStopPlayback();
}

void PtePlayer::stopPlayback()
{
    assert(myIsPlaying);
    startStopPlayback();
    getCaret().moveToLocation(myMidiPlayer->getStartLocation());
}

ScoreArea *PtePlayer::getScoreArea()
{
    return mScoreArea;
    // return dynamic_cast<ScoreArea *>(myTabWidget->currentWidget());
}

void PtePlayer::setScoreZoom(int percent, bool playback_widget_update)
{
    myDocumentManager->getCurrentDocument().getViewOptions().setZoom(percent);
    applyZoomChange(playback_widget_update);
}

void PtePlayer::applyZoomChange(bool playback_widget_update)
{
    getScoreArea()->refreshZoom();

    if (!playback_widget_update)
    {
        myPlaybackWidget->reset(myDocumentManager->getCurrentDocument());
    }

    auto settings = mySettingsManager->getWriteHandle();
    settings->set(Settings::LastZoomLevel,
                  myDocumentManager->getCurrentDocument().getViewOptions().getZoom());
}

void PtePlayer::updateActiveVoice(int voice)
{
    getLocation().setVoiceIndex(voice);
    updateCommands();
}

void PtePlayer::updateActiveFilter(int filter)
{
    myDocumentManager->getCurrentDocument().getViewOptions().setFilter(filter);
    redrawScore();
}

void PtePlayer::redrawScore()
{
    Document &doc = myDocumentManager->getCurrentDocument();
    doc.validateViewOptions();
    getCaret().moveToValidPosition();
    getScoreArea()->renderDocument(doc);
    updateCommands();

    // myMixer->reset(doc.getScore());
    // myInstrumentPanel->reset(doc.getScore());
    myPlaybackWidget->reset(doc);
}

void PtePlayer::createTabArea()
{
    // myTabWidget = new QTabWidget(this);
    // myTabWidget->setDocumentMode(true);
    // myTabWidget->setTabsClosable(true);

    // connect(myTabWidget, &QTabWidget::tabCloseRequested, this,
    //         &PtePlayer::closeTab);
    // connect(myTabWidget, &QTabWidget::currentChanged, this,
    //         &PtePlayer::switchTab);

    {
        auto settings = mySettingsManager->getReadHandle();
        double initial_zoom = settings->get(Settings::LastZoomLevel);
        myPlaybackWidget = new PlaybackWidget(
            *myPlayPauseCommand, *myRewindCommand, *myStopCommand,
            *myMetronomeCommand, *myCountInCommand, initial_zoom, this);
    }

    connect(myPlaybackWidget, &PlaybackWidget::activeVoiceChanged, this,
            &PtePlayer::updateActiveVoice);

    connect(myPlaybackWidget, &PlaybackWidget::activeFilterChanged, this,
            &PtePlayer::updateActiveFilter);

    connect(myPlaybackWidget, &PlaybackWidget::zoomChanged, this, [=](int percent) {
        setScoreZoom(percent, true);
    });

    auto update_metronome_state = [&]() {
        auto settings = mySettingsManager->getReadHandle();
        myMetronomeCommand->setChecked(
            settings->get(Settings::MetronomeEnabled));
        myCountInCommand->setChecked(settings->get(Settings::CountInEnabled));
    };

    update_metronome_state();
    mySettingsManager->subscribeToChanges(update_metronome_state);

    // myPlaybackArea = new QWidget(this);
    // QVBoxLayout *layout = new QVBoxLayout(myPlaybackArea);
    // ui->layout_player->addWidget(myTabWidget);
    mScoreArea = new ScoreArea(*mySettingsManager, this);
    ui->layout_player->addWidget(mScoreArea);
    mScoreArea->installEventFilter(this);
    mScoreArea->setEnabled(false);
    ui->layout_clt->addWidget(myPlaybackWidget);
    // ui->layout_player->addWidget(myPlaybackWidget, 0, Qt::AlignHCenter);
    // layout->setMargin(0);
    // ui->layout_player->setSpacing(0);

    enableEditing(false);
    myPlaybackWidget->setEnabled(false);
}

PtePlayer::~PtePlayer()
{
    delete ui;
}

QGraphicsScene *
PtePlayer::getSene() const
{
    return mScoreArea->scene();
}

QRectF
PtePlayer::getStartInfoRect() const
{
    QRectF rect = mScoreArea->getSysRect(0, perShowSysCount());
    return mScoreArea->getStartInfoRect().united(rect);
}

void
PtePlayer::setPerShowSysCount(int count)
{
    myPerShowSysCount = count;
}

int
PtePlayer::perShowSysCount() const
{
    return myPerShowSysCount;
}

QRectF PtePlayer::getCurCaretRect()
{
    int cursys = getCaret().getLocation().getSystemIndex();
    return mScoreArea->getSysRect(cursys,myPerShowSysCount );
}

QSizeF PtePlayer::getAvgSystemSize() const
{
    return mScoreArea->getAvgSysSize();
}

int PtePlayer::orgBaseBpm() const
{
    if(myDocumentManager->getDocumentListSize()==0){
        return 0;
    }
    const Score &score = myDocumentManager->getCurrentDocument().getScore();
    return score.getBasicRateBp();
}

int PtePlayer::allBarCount() const
{
    const Score &score = myDocumentManager->getCurrentDocument().getScore();
    int allBarCount = 0;
    for(System onesys:score.getSystems())
    {
        allBarCount += onesys.getBarlines().size();
        allBarCount --;
    }
    return allBarCount;
}

void PtePlayer::setSpeedBpm(int bpm)
{
    myPlaybackWidget->setPlayBackSpeedBpm(bpm);
}

void PtePlayer::moveToBar(int startBar)
{
    const Score &score = myDocumentManager->getCurrentDocument().getScore();
    int curBar = 0;
    int sysidx = 0;
    for(System onesys:score.getSystems())
    {
        // qDebug()<<"bar count :"<<onesys.getBarlines().size();
        size_t bari =0;
        for(auto onebar:onesys.getBarlines())
        {
            if(bari==onesys.getBarlines().size()-1){
                break;
            }
            if(curBar==startBar)
            {
                getCaret().moveToSystem(sysidx,false);
                getCaret().moveToPosition(onebar.getPosition());
                return;
            }
            ++curBar;
            ++bari;
        }
        ++sysidx;
    }

    // getLocation().setSystemIndex();
    // getLocation().setPositionIndex();
}

bool PtePlayer::hadOpenFile() const
{
    return myDocumentManager->hasOpenDocuments();
}

QList<ScoreSettingInfos::PlayerSetInfo> PtePlayer::orgPlayerSetInfos() const
{
    QList<ScoreSettingInfos::PlayerSetInfo> result;
    if(!hadOpenFile()){
        return result;
    }
    Document &doc = myDocumentManager->getCurrentDocument();
    for (auto oneplayer : doc.getScore().getPlayers())
    {
        ScoreSettingInfos::PlayerSetInfo oneinfo;
        QString playstr = QString::fromStdString(oneplayer.getDescription());
        oneinfo.name = playstr;
        oneinfo.activated = true;
        oneinfo.number_stuff_show = false;
        oneinfo.std_stuff_show = false;
        // doc.getViewOptions().showStdStuffs().push_back(false);
        if (playstr.contains(QString::fromWCharArray(L"旋律")))
        {
            oneinfo.tab_stuff_show = false;
            // doc.getViewOptions().showTabStuffs().push_back(false);
        }
        else
        {
            oneinfo.tab_stuff_show = true;
            // doc.getViewOptions().showTabStuffs().push_back(true);
        }
        result.push_back(oneinfo);
    }
    return result;
}

void PtePlayer::setPlayerSetInfos(const QList<ScoreSettingInfos::PlayerSetInfo> &setInfos, bool reRander)
{
    if(!hadOpenFile()){
        return ;
    }
    Document &doc = myDocumentManager->getCurrentDocument();
    doc.getViewOptions().showTabStuffs().clear();
    doc.getViewOptions().showStdStuffs().clear();
    doc.getViewOptions().showNumberStuffs().clear();
    for(const auto &one:setInfos)
    {
        if(!one.activated){
            doc.getViewOptions().showStdStuffs().push_back(false);
            doc.getViewOptions().showTabStuffs().push_back(false);
            doc.getViewOptions().showNumberStuffs().push_back(false);
            continue;
        }
        if(one.std_stuff_show){
            doc.getViewOptions().showStdStuffs().push_back(true);
        }
        else
		{
			doc.getViewOptions().showStdStuffs().push_back(false);
        }
        if(one.tab_stuff_show){
            doc.getViewOptions().showTabStuffs().push_back(true);
        }
        else
		{
			doc.getViewOptions().showTabStuffs().push_back(false);
		}
		if (one.number_stuff_show) {
			doc.getViewOptions().showNumberStuffs().push_back(true);
		}
		else
		{
			doc.getViewOptions().showNumberStuffs().push_back(false);
		}
    }
    if (reRander)
    {
		mScoreArea->renderDocument(doc);
    }

}

void PtePlayer::createMidiThread()
{
    myMidiThread = std::make_unique<QThread>();

    myMidiPlayer = new MidiPlayer(*mySettingsManager);
    myMidiPlayer->moveToThread(myMidiThread.get());
    connect(myMidiThread.get(), &QThread::finished, myMidiPlayer,
            &QObject::deleteLater);

    connect(myMidiPlayer, &MidiPlayer::error, this,
            [=](const QString &msg)
            { QMessageBox::critical(this, tr("Midi Error"), msg); });

    connect(myMidiPlayer, &MidiPlayer::playbackSystemChanged, this,
            &PtePlayer::moveCaretToSystem);
    connect(myMidiPlayer, &MidiPlayer::playbackPositionChanged, this,
            &PtePlayer::moveCaretToPosition);
    connect(myMidiPlayer, &MidiPlayer::playbackFinished, this,
            [this]() { 
            startStopPlayback();
                emit endPlayback();
        });
    connect(myMidiPlayer, &MidiPlayer::perFormCountInStart, this,
            [this]() { scorePerFormCountInStart(); });
    connect(myMidiPlayer, &MidiPlayer::performCountInEnd, this,
            [this]() { scorePerFormCountInEnd(); });
    connect(myMidiPlayer, &MidiPlayer::playBackStart, this,
            [this]() { playerBackStart(); });

           // Start the thread and setup the MIDI device in the background.
    myMidiThread->start();
    QMetaObject::invokeMethod(myMidiPlayer, &MidiPlayer::init,
                              Qt::QueuedConnection);
}


void
PtePlayer::createTickThread()
{
//#ifndef _WIN32
    myTickThread = std::make_unique<QThread>();
    // myTickPlayer = new TickPlayer(myTickThread.get());
    myTickPlayer = new TickPlayer(this);
    // connect(myTickThread.get(), &QThread::finished, myTickPlayer,
    //         &QObject::deleteLater);
    // myTickPlayer->moveToThread(myTickThread.get());
    // myTickThread->start();
    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::init,
    //                           Qt::QueuedConnection);
     myTickPlayer->init();
//#endif
}

void PtePlayer::moveCaretToSystem(int system)
{
    myLastBar = 0;
//#ifndef _WIN32

           // QMetaObject::invokeMethod(myTickPlayer, [this]()
           //                           {

    // myTickPlayer->stop();
    // myTickPlayer->start();
    //                           },
    //                           Qt::QueuedConnection);
    qDebug() << "myTickPlayer moveCaretToSystem ";
    if(myIsPlaying){
        myTickPlayer->stop();
        myTickPlayer->start();
    }
    //mTickPlayer->play();
//#endif
    getCaret().moveToSystem(system, true);
    QRectF srect = mScoreArea->getSysRect(system,myPerShowSysCount );
    emit caretMvRect(srect);
}

void PtePlayer::moveCaretToPosition(int position)
{
    const Score &score = getCaret().getLocation().getScore();
    int sys = getCaret().getLocation().getSystemIndex();
    int curbpm = score.getRateBpByPos(sys,position);
    int curbar = score.getBarByPos(sys, position);
    //if (myLastBar != curbar)
    //{
    //    myLastBar = curbar;
    //}
    //if (curbpm != myLastBpm)
    //{
    //    bool pause = true;
    //}
    //#ifndef _WIN32

    if (curbar != myLastBar)
    {
        myLastBar = curbar;
        // QMetaObject::invokeMethod(myTickPlayer,[this](){

        // myTickPlayer->stop();
        // myTickPlayer->start();
        // },
        //                           Qt::QueuedConnection);
        qDebug() << "myTickPlayer moveCaretToPosition";
        if(myIsPlaying){
            myTickPlayer->stop();
            myTickPlayer->start();
        }
    }

    if(curbpm!=myLastBpm)
    {
        // QMetaObject::invokeMethod(myTickPlayer,[this,curbpm](){

        // double curRate = myTickPlayer->curPlayRate();
        // curRate *= ((double)curbpm / (double)myLastBpm);
        // myTickPlayer->stop();
        // myTickPlayer->setPlayerOpts(myTickPlayer->curTickName(),curRate);
        // myTickPlayer->start();
        //                           },
        //                           Qt::QueuedConnection);

        qDebug() << "myTickPlayer moveCaretToPosition 2";
        double curRate = myTickPlayer->curPlayRate();
        curRate *= ((double)curbpm / (double)myLastBpm);
        if(myIsPlaying){

            myTickPlayer->stop();
            myTickPlayer->setPlayerOpts(myTickPlayer->curTickName(),curRate);
            myTickPlayer->start();
            // mTickPlayer->stop();
            //mTickPlayer->play();
        }
    }
    //#endif
    getCaret().moveToPosition(position);

    QRectF srect = mScoreArea->getSysRect(sys, myPerShowSysCount);
    emit caretMvRect(srect);
}

void PtePlayer::startStopPlayback(bool from_measure_start)
{
    myIsPlaying = !myIsPlaying;

    if (myIsPlaying)
    {
        // Start up the midi player.
        // myPlayPauseCommand->setText(tr("Pause"));
        resetFullScreen();
               // Move the caret to the start of the current bar if necessary.
        if (from_measure_start)
        {
            moveCaretToPrevBar();
            moveCaretToNextBar();
        }

        getCaret().setIsInPlaybackMode(true);
        myPlaybackWidget->setPlaybackMode(true);
        enableEditing(false);

        emit caretMvRect(getCurCaretRect());
        connect(
            myPlaybackWidget, &PlaybackWidget::playbackSpeedChanged,
            myMidiPlayer, &MidiPlayer::liveChangePlaybackSpeed,
            Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));

               // Notify the MIDI thread to start playing.
        QMetaObject::invokeMethod(
            myMidiPlayer,
            [&]()
            {
                int baseR = getLocation().getScore().getBasicRateBp();
                myMidiPlayer->playScore(
                    getLocation(), myPlaybackWidget->getPlaybackSpeed(baseR));


            },
            Qt::QueuedConnection);

        //#ifndef _WIN32

        // if(mTickPlayer && myIsPausing)
        // {
        //     mTickPlayer->play();
        //     qDebug()<<"mTickPlayer && myIsPausing";
        //     return;
        // }
        // auto allD = QMediaDevices::audioOutputs();
        // if(allD.empty()){
        //     return;
        // }
        // if(TickPlayer)
        // {
        //     disconnect(mTickPlayer,&QMediaPlayer::playbackStateChanged,
        //                this,&PtePlayer::tickState);
        //     mTickPlayer->stop();
        // }
        // QAudioDevice device = allD.at(0);
        // if(!mTickOutPut)
        // {
        //     mTickOutPut= new QAudioOutput(this);
        // }
        // mTickOutPut->setDevice(device);
        // if(!mTickPlayer)
        // {
        //     mTickPlayer = new QMediaPlayer(this);
        // }
        // mTickPlayer->setAudioOutput(mTickOutPut);
        QString tname;
        double rate = 1.0;
        _getBaseTempoInfo(tname,rate);
        const Score &score = myDocumentManager->getCurrentDocument().getScore();
        myLastBpm = score.getBasicRateBp();
        myLastBar = 0;
        // QMetaObject::invokeMethod(myTickPlayer, [this,tname,rate](){
        //     myTickPlayer->stop();
        // myTickPlayer->setPlayerOpts(tname,rate);
        //                 },
        //                           Qt::QueuedConnection);

        myTickPlayer->stop();
        myTickPlayer->setPlayerOpts(tname,rate);
        // QMetaObject::invokeMethod(
        //     myTickPlayer,
        //     [tname,rate,this]()
        //     {
        //     },
        //     Qt::QueuedConnection);
        // QUrl nameurl(tname);
        // qDebug()<<"tanem:"<<tname<<" rate "<<rate;
        // qDebug()<< nameurl.errorString();
        // qDebug()<<"assest path:"<<nameurl.path();
        // getCaret();
        // mTickPlayer->setPlaybackRate(rate);
        // mTickPlayer->setSource(nameurl);
        // connect(mTickPlayer,&QMediaPlayer::playbackStateChanged,
        //         this,&PtePlayer::tickState);
        // mTickPlayer->play();
        //#endif
    }
    else
    {
        // Ensure playback has finished.
        myMidiPlayer->stopPlayback();

        // myPlayPauseCommand->setText(tr("Play"));
        getCaret().setIsInPlaybackMode(false);
        myPlaybackWidget->setPlaybackMode(false);

        enableEditing(true);
        updateCommands();
        qDebug() << "myTickPlayer->stop() play back end";
//#ifndef _WIN32
        // myTickPlayer->pausePlayer();
        myTickPlayer->stop();
        // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::pause,
        //                           Qt::QueuedConnection);
        // QMetaObject::invokeMethod(
        //     myTickPlayer,
        //     [&]()
        //     {
        //         myTickPlayer->pausePlayer();
        //     },
        //     Qt::QueuedConnection);
        // if (mTickPlayer)
        // {
        //     mTickPlayer->pause();
        //     myIsPausing = true;
        //     qDebug()<<"mTickPlayer set pausing ";
        // }
        //#endif
    }
}


void PtePlayer::enableEditing(bool enable)
{
           // Prevent the user from changing tabs during playback.
    // myTabWidget->tabBar()->setEnabled(enable);
           mScoreArea->setEnabled(false);
}




void PtePlayer::moveCaretToPrevBar()
{
    getCaret().moveToPrevBar();
    // Move to the first position after the barline.
    if (getCaret().getLocation().getPositionIndex() != 0)
        getCaret().moveHorizontal(1);
}

void PtePlayer::moveCaretToPrevStaff()
{
    getCaret().moveStaff(-1);
}

void PtePlayer::moveCaretToNextBar()
{
    getCaret().moveToNextBar();
    // Move to the first position after the barline.
    if (getCaret().getLocation().getPositionIndex() != 0)
        getCaret().moveHorizontal(1);
}

Caret &PtePlayer::getCaret()
{
    return myDocumentManager->getCurrentDocument().getCaret();
}

ScoreLocation &PtePlayer::getLocation()
{
    return getCaret().getLocation();
}

void PtePlayer::updateCommands()
{
    if (myIsPlaying)
        return;

    // ScoreLocation location = getLocation();
    // const Score &score = location.getScore();
    // if (score.getSystems().empty())
    //     return;

    // const System &system = location.getSystem();
    // if (system.getStaves().empty())
    //     return;

    // const Staff &staff = location.getStaff();
    // const Position *pos = location.getPosition();
    // const int position = location.getPositionIndex();
    // const Note *note = location.getNote();
    // const Barline *barline = location.getBarline();
    // const TempoMarker *tempoMarker =
    //     ScoreUtils::findByPosition(system.getTempoMarkers(), position);
    // const AlternateEnding *altEnding =
    //     ScoreUtils::findByPosition(system.getAlternateEndings(), position);
    // const Dynamic *dynamic =
    //     ScoreUtils::findByPosition(staff.getDynamics(), position);
    // const bool positions_selected =
    //     location.hasSelection() && !location.getSelectedPositions().empty();

    // myRemoveCurrentSystemCommand->setEnabled(score.getSystems().size() > 1);
    // myRemoveCurrentStaffCommand->setEnabled(system.getStaves().size() > 1);
    // myIncreaseLineSpacingCommand->setEnabled(score.getLineSpacing() <
    //                                          Score::MAX_LINE_SPACING);
    // myDecreaseLineSpacingCommand->setEnabled(score.getLineSpacing() >
    //                                          Score::MIN_LINE_SPACING);
    // myRemoveSpaceCommand->setEnabled(!pos && (position == 0 || !barline) &&
    //                                  !tempoMarker && !altEnding && !dynamic);
    // myRemoveItemCommand->setEnabled(
    //     pos || positions_selected ||
    //     canDeleteItem(getCaret().getSelectedItem()));
    // myRemovePositionCommand->setEnabled(pos || positions_selected);

    // myChordNameCommand->setChecked(
    //     ScoreUtils::findByPosition(system.getChords(), position) != nullptr);
    // myTextCommand->setChecked(
    //     ScoreUtils::findByPosition(system.getTextItems(), position) != nullptr);

           // Note durations
    // Position::DurationType durationType = myActiveDurationType;
    // if (pos)
    //     durationType = pos->getDurationType();

    // switch (durationType)
    // {
    //     case Position::WholeNote:
    //         myWholeNoteCommand->setChecked(true);
    //         myWholeRestCommand->setChecked(true);
    //         break;
    //     case Position::HalfNote:
    //         myHalfNoteCommand->setChecked(true);
    //         myHalfRestCommand->setChecked(true);
    //         break;
    //     case Position::QuarterNote:
    //         myQuarterNoteCommand->setChecked(true);
    //         myQuarterRestCommand->setChecked(true);
    //         break;
    //     case Position::EighthNote:
    //         myEighthNoteCommand->setChecked(true);
    //         myEighthRestCommand->setChecked(true);
    //         break;
    //     case Position::SixteenthNote:
    //         mySixteenthNoteCommand->setChecked(true);
    //         mySixteenthRestCommand->setChecked(true);
    //         break;
    //     case Position::ThirtySecondNote:
    //         myThirtySecondNoteCommand->setChecked(true);
    //         myThirtySecondRestCommand->setChecked(true);
    //         break;
    //     case Position::SixtyFourthNote:
    //         mySixtyFourthNoteCommand->setChecked(true);
    //         mySixtyFourthRestCommand->setChecked(true);
    //         break;
    // }

    // myIncreaseDurationCommand->setEnabled(durationType != Position::WholeNote);
    // myDecreaseDurationCommand->setEnabled(durationType !=
    //                                       Position::SixtyFourthNote);

    // updatePositionProperty(myDottedCommand, pos, Position::Dotted);
    // updatePositionProperty(myDoubleDottedCommand, pos, Position::DoubleDotted);
    // myAddDotCommand->setEnabled(pos &&
    //                             !pos->hasProperty(Position::DoubleDotted));
    // myRemoveDotCommand->setEnabled(pos &&
    //                                (pos->hasProperty(Position::Dotted) ||
    //                                 pos->hasProperty(Position::DoubleDotted)));

    // myLeftHandFingeringCommand->setEnabled(note != nullptr);
    // myLeftHandFingeringCommand->setChecked(note && note->hasLeftHandFingering());

    // myShiftStringUpCommand->setEnabled(note != nullptr || positions_selected);
    // myShiftStringDownCommand->setEnabled(note != nullptr || positions_selected);

    // if (note)
    // {
    //     myTieCommand->setText(tr("Tied"));
    //     myTieCommand->setChecked(note->hasProperty(Note::Tied));
    //     myTieCommand->setEnabled(true);
    // }
    // else if (!barline)
    // {
    //     myTieCommand->setText(tr("Insert Tied Note"));
    //     myTieCommand->setChecked(false);
    //     myTieCommand->setEnabled(true);
    // }
    // else
    //     myTieCommand->setEnabled(false);

    // updateNoteProperty(myMutedCommand, note, Note::Muted);
    // updateNoteProperty(myGhostNoteCommand, note, Note::GhostNote);
    // updatePositionProperty(myLetRingCommand, pos, Position::LetRing);
    // updatePositionProperty(myFermataCommand, pos, Position::Fermata);
    // updatePositionProperty(myGraceNoteCommand, pos, Position::Acciaccatura);
    // updatePositionProperty(myStaccatoCommand, pos, Position::Staccato);
    // updatePositionProperty(myMarcatoCommand, pos, Position::Marcato);
    // updatePositionProperty(mySforzandoCommand, pos, Position::Sforzando);

    // updateNoteProperty(myOctave8vaCommand, note, Note::Octave8va);
    // updateNoteProperty(myOctave8vbCommand, note, Note::Octave8vb);
    // updateNoteProperty(myOctave15maCommand, note, Note::Octave15ma);
    // updateNoteProperty(myOctave15mbCommand, note, Note::Octave15mb);

    // myAddRestCommand->setEnabled(!pos || !pos->isRest());

    // myTripletCommand->setEnabled(pos != nullptr);
    // myIrregularGroupingCommand->setEnabled(pos != nullptr);

    // myMultibarRestCommand->setEnabled(!barline || position == 0);
    // myMultibarRestCommand->setChecked(pos && pos->hasMultiBarRest());

    // myRehearsalSignCommand->setEnabled(barline != nullptr);
    // myRehearsalSignCommand->setChecked(barline && barline->hasRehearsalSign());

    // const bool isAlterationOfPace =
    //     (tempoMarker &&
    //      tempoMarker->getMarkerType() == TempoMarker::AlterationOfPace);
    // myTempoMarkerCommand->setEnabled(!tempoMarker || !isAlterationOfPace);
    // myTempoMarkerCommand->setChecked(tempoMarker && !isAlterationOfPace);
    // myAlterationOfPaceCommand->setEnabled(!tempoMarker || isAlterationOfPace);
    // myAlterationOfPaceCommand->setChecked(isAlterationOfPace);

    // myKeySignatureCommand->setEnabled(barline != nullptr);
    // myTimeSignatureCommand->setEnabled(barline != nullptr);
    // myStandardBarlineCommand->setEnabled(!pos && !barline);
    // myDirectionCommand->setChecked(
    //     ScoreUtils::findByPosition(system.getDirections(), position) !=
    //     nullptr);
    // myRepeatEndingCommand->setChecked(altEnding != nullptr);

    //        // dynamics
    // myDynamicCommand->setChecked(dynamic != nullptr);

    // if (dynamic)
    // {
    //     switch (dynamic->getVolume())
    //     {
    //         case VolumeLevel::Off:
    //             break;
    //         case VolumeLevel::ppp:
    //             myDynamicPPPCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::pp:
    //             myDynamicPPCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::p:
    //             myDynamicPCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::mp:
    //             myDynamicMPCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::mf:
    //             myDynamicMFCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::f:
    //             myDynamicFCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::ff:
    //             myDynamicFFCommand->setChecked(true);
    //             break;
    //         case VolumeLevel::fff:
    //             myDynamicFFFCommand->setChecked(true);
    //             break;
    //     }
    // }
    // else
    // {
    //     QAction *checkedAction = myDynamicGroup->checkedAction();
    //     if (checkedAction)
    //         checkedAction->setChecked(false);
    // }

    // myVolumeSwellCommand->setEnabled(pos);
    // myVolumeSwellCommand->setChecked(pos && pos->hasVolumeSwell());

    // if (barline) // Current position is bar.
    // {
    //     myBarlineCommand->setText(tr("Edit Barline"));
    //     myBarlineCommand->setEnabled(true);
    // }
    // else if (!pos) // Current position is empty.
    // {
    //     myBarlineCommand->setText(tr("Insert Barline"));
    //     myBarlineCommand->setEnabled(true);
    // }
    // else // Current position has notes.
    // {
    //     myBarlineCommand->setDisabled(true);
    //     myBarlineCommand->setText(tr("Barline"));
    // }

    // myHammerPullCommand->setEnabled(note != nullptr);
    // myHammerPullCommand->setChecked(note &&
    //                                 note->hasProperty(Note::HammerOnOrPullOff));

    // updateNoteProperty(myHammerOnFromNowhereCommand, note,
    //                    Note::HammerOnFromNowhere);
    // updateNoteProperty(myPullOffToNowhereCommand, note, Note::PullOffToNowhere);
    // updateNoteProperty(myNaturalHarmonicCommand, note, Note::NaturalHarmonic);
    // myArtificialHarmonicCommand->setEnabled(note != nullptr);
    // myArtificialHarmonicCommand->setChecked(note &&
    //                                         note->hasArtificialHarmonic());
    // myTappedHarmonicCommand->setEnabled(note != nullptr);
    // myTappedHarmonicCommand->setChecked(note && note->hasTappedHarmonic());

    // myBendCommand->setEnabled(note != nullptr);
    // myBendCommand->setChecked(note && note->hasBend());

    // myTremoloBarCommand->setEnabled(pos != nullptr);
    // myTremoloBarCommand->setChecked(pos && pos->hasTremoloBar());

    // updateNoteProperty(mySlideIntoFromAboveCommand, note,
    //                    Note::SlideIntoFromAbove);
    // updateNoteProperty(mySlideIntoFromBelowCommand, note,
    //                    Note::SlideIntoFromBelow);
    // updateNoteProperty(myShiftSlideCommand, note, Note::ShiftSlide);
    // updateNoteProperty(myLegatoSlideCommand, note, Note::LegatoSlide);
    // updateNoteProperty(mySlideOutOfDownwardsCommand, note,
    //                    Note::SlideOutOfDownwards);
    // updateNoteProperty(mySlideOutOfUpwardsCommand, note,
    //                    Note::SlideOutOfUpwards);

    // updatePositionProperty(myVibratoCommand, pos, Position::Vibrato);
    // updatePositionProperty(myWideVibratoCommand, pos, Position::WideVibrato);
    // updatePositionProperty(myPalmMuteCommand, pos, Position::PalmMuting);
    // updatePositionProperty(myTremoloPickingCommand, pos,
    //                        Position::TremoloPicking);
    // myTrillCommand->setEnabled(note != nullptr);
    // myTrillCommand->setChecked(note && note->hasTrill());
    // updatePositionProperty(myTapCommand, pos, Position::Tap);
    // updatePositionProperty(myArpeggioUpCommand, pos, Position::ArpeggioUp);
    // updatePositionProperty(myArpeggioDownCommand, pos, Position::ArpeggioDown);
    // updatePositionProperty(myPickStrokeUpCommand, pos, Position::PickStrokeUp);
    // updatePositionProperty(myPickStrokeDownCommand, pos,
    //                        Position::PickStrokeDown);

    // myPlayerChangeCommand->setChecked(
    //     ScoreUtils::findByPosition(system.getPlayerChanges(), position) !=
    //     nullptr);
}

void PtePlayer::on_btn_last_song_clicked()
{
    if(m_cur_file_idx-1< 0)
    {
        return;
    }
    closeTab();
    --m_cur_file_idx;
    setCurFileName();
}


void PtePlayer::on_btn_setting_clicked()
{
    SettingDialog diag;
    diag.setCurScoreSet(m_cur_scoreset);
    if(QDialog::Accepted!=diag.exec())
    {
        return;
    }
//    diag.curPath();
    // QDir curdir(diag.curPath());
    m_cur_files = diag.allScorePaths();
    m_cur_score_names = diag.allScoreNames();
    // QStringList allFiles=diag.allFiles();
//    QString normfile = diag.normFile();

    if(m_cur_files.empty())
    {
        return;
    }
    if(m_cur_file_idx>=0)
    {
        closeTab();
    }
    // m_cur_files = allFiles;
    m_cur_file_idx = diag.normFileIndx();
    setCurFileName();
}


void PtePlayer::setCurFileName()
{
    openFile(m_cur_files[m_cur_file_idx]);
    ui->btn_next_song->setEnabled(true);
    ui->btn_last_song->setEnabled(true);
    if(m_cur_file_idx!=m_cur_files.size()-1)
    {
        QString filename =m_cur_score_names[m_cur_file_idx + 1];// QFileInfo(m_cur_files[m_cur_file_idx + 1]).fileName();
        ui->lineEdit_nextsong->setText(filename);
    }
    else
    {
        ui->btn_next_song->setEnabled(false);
    }
    if(m_cur_file_idx>0)
    {
        QString filename = m_cur_score_names[m_cur_file_idx - 1];//QFileInfo(m_cur_files[m_cur_file_idx - 1]).fileName();
        ui->lineEdit_lastsong->setText(filename);
    }
    else
    {
        ui->btn_last_song->setEnabled(false);
    }
}

bool PtePlayer::eventFilter(QObject *watched, QEvent *event)
{
    // return QMainWindow::eventFilter(watched,event);
    if(watched==mScoreArea)
    {
        QMouseEvent * ment = dynamic_cast<QMouseEvent *>(event);
        if(event->type()==QEvent::MouseButtonPress && ment)
        {
#ifdef WIN32
            QPointF pt = ment->globalPos();
#else
            QPointF pt = ment->globalPosition();
#endif
            QRect r = this->rect();
            // r.setWidth(r.width()*0.8);
            // r.setHeight(r.height()*0.8);
            QPoint tl = mapToGlobal(r.topLeft());
            QPoint br = mapToGlobal(r.bottomRight());
            QPoint midpt=(tl+br)/2;
            QPoint tlnew = midpt+ (tl-midpt)*0.6;
            QPoint brnew = midpt+ (br-midpt)*0.6;
            QRect newr(tlnew,brnew);
            if(!newr.contains(pt.toPoint()))
            {
                return  QMainWindow::eventFilter(watched,event);
            }
            resetFullScreen();

        }
        // qDebug()<<"score area etype:"<< event->type();
    }
    return QMainWindow::eventFilter(watched,event);
}

void
PtePlayer::resetFullScreen()
{
    mCurShowFull = !mCurShowFull;

    bool hvisible = !mCurShowFull;
    ui->btn_last_song->setVisible(hvisible);
    ui->btn_next_song->setVisible(hvisible);
    ui->lineEdit_lastsong->setVisible(hvisible);
    ui->lineEdit_nextsong->setVisible(hvisible);
    ui->btn_setting->setVisible(hvisible);

    myPlaybackWidget->setVisible(hvisible);
    auto vlayout = dynamic_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if(mCurShowFull)
    {
        vlayout->setStretch(0,0);
        vlayout->setStretch(1,9);
        vlayout->setStretch(2,0);
    }else{
        vlayout->setStretch(0,2);
        vlayout->setStretch(1,5);
        vlayout->setStretch(2,2);
    }
}

void PtePlayer::on_btn_next_song_clicked()
{
    if(m_cur_file_idx+1>=m_cur_files.size())
    {
        return;
    }
    closeTab();
    ++m_cur_file_idx;
    setCurFileName();
}

bool PtePlayer::closeTab()
{

    // Prompt to save modified documents.
//    if (!myUndoManager->stacks()[index]->isClean())
//    {
//        QMessageBox msg(this);
//        msg.setWindowTitle(tr("Close Document"));
//        msg.setText(tr("The document has been modified."));
//        msg.setInformativeText(tr("Do you want to save your changes?"));
//        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
//                               QMessageBox::Cancel);
//        msg.setDefaultButton(QMessageBox::Save);

//        const int ret = msg.exec();
//        if (ret == QMessageBox::Save)
//        {
//            if (!saveFile(index))
//                return false;
//        }
//        else if (ret == QMessageBox::Cancel)
//            return false;
//    }

    if (myDocumentManager->getDocument(0).getCaret().isInPlaybackMode())
        startStopPlayback();

//    myUndoManager->removeStack(index);
    myDocumentManager->removeDocument(0);
    // delete myTabWidget->widget(index);

    // Get the index of the tab that we will now switch to.
    const int currentIndex = 0;// myTabWidget->currentIndex();

//    myUndoManager->setActiveStackIndex(currentIndex);
    myDocumentManager->setCurrentDocumentIndex(currentIndex);

    enableEditing(currentIndex != -1);
    myPlaybackWidget->setEnabled(currentIndex != -1);

    return true;
}

void PtePlayer::scorePerFormCountInStart()
{
    qDebug() << "myTickPlayer scorePerFormCountInStart ";
//#ifndef _WIN32

    myTickPlayer->start();
    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::start,
    //                           Qt::QueuedConnection);
//#endif // !_WIN32

}

void PtePlayer::scorePerFormCountInEnd()
{
//#ifndef _WIN32

    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::stop,
    //                           Qt::QueuedConnection);
    myTickPlayer->stop();
//#endif // !_WIN32
    qDebug()<<"scorePerFormCountInEnd ";
}

void PtePlayer::playerBackStart()
{
    if (!myIsPlaying)
    {
        return;
    }
//#ifndef _WIN32

    myTickPlayer->start();
    // QMetaObject::invokeMethod(myTickPlayer, &TickPlayer::start,
    //                           Qt::QueuedConnection);
//#endif // !_WIN32
    qDebug()<<"myTickPlayer playerBackStart ";
}

void
PtePlayer::_getBaseTempoInfo(QString &tickName, double &rate) const
{
    const Score &score = myDocumentManager->getCurrentDocument().getScore();

    int baseR = score.getBasicRateBp();
    double sr = (double)(myPlaybackWidget->getPlaybackSpeed(baseR))/100.0;
    for (auto sysbase : score.getSystems())
    {
        for (auto onetmpo : sysbase.getTempoMarkers())
        {
            tickName = "qrc:/music/tick-4-4-120bp";
            int permute = onetmpo.getBeatsPerMinute();
            rate = ((double)permute / 120.0) * sr;
            break;
        }
    }
    //System sysbase = *score.getSystems().begin();
}

