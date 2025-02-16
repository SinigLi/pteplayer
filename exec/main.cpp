/*
  * Copyright (C) 2011 Cameron White
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
 
#include <app/appinfo.h>
#include <app/paths.h>
#include <app/powertabeditor.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <csignal>
#include <dialogs/crashdialog.h>
#include <exception>
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <QFileOpenEvent>
#include <QLibraryInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTranslator>
#include <string>
#include <app/PtePlayer.h>
#include <QDir>
#include <QFileInfo>
#include <formats/gp7/document.cpp>
#include <QDebug>
#include <fstream>
#include <QFile>
#include <app/ScoreFileLoader.h>


/**/

#ifndef _WIN32
#include <QJniObject>

// #include <QtAndroidExtras/QtAndroid>
// #include <QtAndroidExtras/QAndroidJniObject>
void
setLandscapeMode()
{
	return;
	QJniObject activity = QJniObject::callStaticObjectMethod(
		"org/qtproject/qt6/android/QtNative", "activity",
		"()Landroid/app/Activity;");

	if (activity.isValid())
	{
		activity.callMethod<void>("setRequestedOrientation", "(I)V",
			0); // 0: 横屏
	}
}
#endif

#include <QQmlError>
#include <QQuickView>


#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>
#include <QIcon>

#include <QStyle>

#include <QQuickImageProvider>
class StandardIconProvider : public QQuickImageProvider
{
public:
    StandardIconProvider(QStyle *style)
        : QQuickImageProvider(Pixmap)
          , m_style(style)
    {}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        Q_UNUSED(size)
        static const auto metaobject = QMetaEnum::fromType<QStyle::StandardPixmap>();
        const int value = metaobject.keyToValue(id.toLatin1());
        QIcon icon = m_style->standardIcon(static_cast<QStyle::StandardPixmap>(value));
        return icon.pixmap(requestedSize);
    }

    QStyle *m_style;
};

#include <QAudioDevice>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QAudioOutput>
void
get_add_audio_device()
{
    auto allD = QMediaDevices::audioOutputs();
    qDebug() << "divice count:" << allD.size();

    for (QAudioDevice &oned : allD)
    {
        qDebug() << oned.description();
    }
}

#ifdef __APPLE__
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#endif
#include <boost/stacktrace.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void displayError(const std::string &reason)
{
    std::string message = reason;
#if BOOST_VERSION >= 106900
    message += boost::stacktrace::to_string(
        boost::stacktrace::stacktrace());
#else
    for (auto &&frame : boost::stacktrace::stacktrace())
    {
        message += boost::stacktrace::to_string(frame);
        message += "\n";
    }
#endif

    // If there is no QApplication instance, something went seriously wrong
    // during startup - just dump the error to the console.
    if (!QApplication::instance())
        std::cerr << message << std::endl;
    else
    {
        CrashDialog dialog(QString::fromStdString(message),
                           QApplication::activeWindow());
        dialog.exec();
    }

    std::exit(EXIT_FAILURE);
}

static void terminateHandler()
{
    std::string message;

    // If an exception was thrown, grab its message.
    std::exception_ptr eptr = std::current_exception();
    if (eptr)
    {
        message = "Unhandled exception: ";
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception &e)
        {
            message += e.what();
        }
    }
    else
        message = "Program terminated unexpectedly";

    displayError(message);
}

static void signalHandler(int )
{
    displayError("Segmentation fault");
}

class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv)
    {
    }

protected:
    virtual bool event(QEvent *event) override
    {
        switch (event->type())
        {
            // Forward file open requests to the application (e.g. double
            // clicking a file on OSX).
            case QEvent::FileOpen:
            {
                auto app = dynamic_cast<PowerTabEditor *>(activeWindow());
                Q_ASSERT(app);

                app->openFiles({
                    static_cast<QFileOpenEvent *>(event)->file()
                });
                return true;
            }
            default:
                return QApplication::event(event);
        }
    }
};

static void
loadTranslations(QApplication &app, QTranslator &qt_translator,
                 QTranslator &ptb_translator)
{
    QLocale locale;
    qDebug() << "Finding translations for locale" << locale
             << "with UI languages" << locale.uiLanguages();

    for (auto &&path : Paths::getTranslationDirs())
    {
        QString dir = Paths::toQString(path);
        qDebug() << "  - Checking" << dir;

        if (ptb_translator.isEmpty() &&
            ptb_translator.load(locale, QStringLiteral("powertabeditor"),
                                QStringLiteral("_"), dir))
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            qDebug() << "Loaded application translations from"
                     << ptb_translator.filePath();
#else
            qDebug() << "Loaded application translations";
#endif
            app.installTranslator(&ptb_translator);
        }

        if (qt_translator.isEmpty() &&
            qt_translator.load(locale, QStringLiteral("qtbase"),
                               QStringLiteral("_"), dir))
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
            qDebug() << "Loaded Qt base translations from"
                     << qt_translator.filePath();
#else
            qDebug() << "Loaded Qt base translations";
#endif
            app.installTranslator(&qt_translator);
        }
    }
}
void show_all_dir(const QString &dir)
{
    qDebug()<<"main dir:"<<dir;
    QStringList subdirs;
    for(auto one:QDir(dir).entryInfoList())
    {
        if(one.fileName()=="."
            ||one.fileName()=="..")
        {
            continue;
        }
        if(one.isDir()){
            qDebug()<<"sub dir "<<one.absoluteFilePath();
            subdirs.push_back(one.absoluteFilePath());
        }
    }
    for(auto onesub:subdirs)
    {
        show_all_dir(onesub);
    }
}
int main(int argc, char *argv[])
{
    // Register handlers for unhandled exceptions and segmentation faults.
    std::set_terminate(terminateHandler);
    std::signal(SIGSEGV, signalHandler);

    Application a(argc, argv);

    // Set the app information (used by e.g. QSettings).
    QCoreApplication::setOrganizationName(AppInfo::ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(AppInfo::APPLICATION_ID);
    QCoreApplication::setApplicationVersion(AppInfo::APPLICATION_VERSION);

    QTranslator qt_translator;
    QTranslator ptb_translator;
    loadTranslations(a, qt_translator, ptb_translator);

    // Allow QWidget::activateWindow() to bring the application into the
    // foreground when running on Windows.
#ifdef _WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    // Parse command line arguments.
    QStringList files_to_open;
    {
        QCommandLineParser parser;
        parser.setApplicationDescription(QCoreApplication::translate(
            "PowerTabEditor", "A guitar tablature editor."));
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument(
            QStringLiteral("files"),
            QCoreApplication::translate("PowerTabEditor",
                                        "The files to be opened"),
            QStringLiteral("[files...]"));
        parser.process(a);

        files_to_open = parser.positionalArguments();
    }

    {
        SettingsManager settings_manager;
        settings_manager.load(Paths::getConfigDir());
        ScoreFileLoader::instace().init();
        // qDebug()<<"getConfigDir "<<Paths::getConfigDir().c_str();
        // qDebug()<<"getUserDataDir "<<Paths::getUserDataDir().c_str();
        // qDebug()<<"getHomeDir "<<Paths::getHomeDir().c_str();
        // std::string tfilepath = Paths::getHomeDir().c_str();
        // tfilepath+="/test.txt";
        // std::fstream testfile;
        // testfile.open(tfilepath);
        // if( testfile.is_open())
        // {
        //     qDebug()<<"openfile :";
        //     testfile.write("aaa",3);
        //     testfile.close();
        // }else
        // {
        //     qDebug()<<"openfile err:"<<tfilepath;
        // }
        // QFile tfile(tfilepath.c_str());
        // if(tfile.open(QFile::OpenModeFlag::ReadWrite))
        // {
        //     tfile.write("ccc");
        //     tfile.close();
        //     qDebug()<<"openfile 2:";
        // }else
        // {

        //     qDebug()<<"openfile err 2:";
        // }

        // std::ifstream ifstr(tfilepath);
        // if(ifstr.is_open())
        // {
        //     std::string ss;
        //     ss.resize(5);
        //     ifstr.read(ss.data(),3);
        //     qDebug()<<"read str:"<<ss;
        // }
        auto settings = settings_manager.getReadHandle();
        bool single_window_mode = !settings->get(Settings::OpenFilesInNewWindow);

        // If an instance of the program is already running and we're in
        // single-window mode, tell the running instance to open the files in
        // new tabs.
        if (!files_to_open.empty() && single_window_mode)
        {
            QLocalSocket socket;
            socket.connectToServer(AppInfo::APPLICATION_ID,
                                   QIODevice::WriteOnly);
            if (socket.waitForConnected(500))
            {
                {
                    QTextStream out(&socket);
                    for (const QString &file : files_to_open)
                        out << file << "\n";
                }

                socket.waitForBytesWritten();
                return EXIT_SUCCESS;
            }
        }
    }
#ifndef _ALL_QML_MODE

#ifndef _WIN32
	setLandscapeMode();

#endif // !_WIN32
	//setLandscapeMode();

    //get_add_audio_device();

    QIcon::setThemeName("gallery");

    QSettings settings;
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
        QQuickStyle::setStyle(settings.value("style").toString());

           // If this is the first time we're running the application,
           // we need to set a style in the settings so that the QML
           // can find it in the list of built-in styles.
    const QString styleInSettings = settings.value("style").toString();
    if (styleInSettings.isEmpty())
        settings.setValue(QLatin1String("style"), QQuickStyle::name());

    QQmlApplicationEngine engine;

    QStringList builtInStyles = { QLatin1String("Basic"), QLatin1String("Fusion"),
                                  QLatin1String("Imagine"), QLatin1String("Material"), QLatin1String("Universal") };
#if defined(Q_OS_MACOS)
    builtInStyles << QLatin1String("macOS");
    builtInStyles << QLatin1String("iOS");
#elif defined(Q_OS_IOS)
    builtInStyles << QLatin1String("iOS");
#elif defined(Q_OS_WINDOWS)
    builtInStyles << QLatin1String("Windows");
#endif
    engine.addImageProvider(QLatin1String("standardicons"), new StandardIconProvider(a.style()));

    engine.setInitialProperties({{ "builtInStyles", builtInStyles }});
    engine.load(QUrl("qrc:/gallery.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;
#else

    PtePlayer program;
    // Otherwise, launch a new window.

           // Set up a server to listen for messages about new files being opened.
    QLocalServer server;
    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket *socket = server.nextPendingConnection();
        if (!socket->canReadLine())
            socket->waitForReadyRead();

        QTextStream in(socket);
        QStringList files;

        while (!in.atEnd())
            files.push_back(in.readLine());

               //program.openFiles(files);
        if (program.isMinimized())
            program.showNormal();
        program.activateWindow();

        delete socket;
    });

    server.listen(AppInfo::APPLICATION_ID);

           // Launch the application.
    program.show();

#endif
    // auto allD = QMediaDevices::audioOutputs();
    // qDebug() << "divice count:" << allD.size();
    // if(allD.empty())
    // {
    //     return 0;
    // }
    // QAudioDevice device = allD.at(0);
    // QAudioOutput outpt;
    // outpt.setDevice(device);
    // QMediaPlayer player;
    // QObject::connect(&player, &QMediaPlayer::errorOccurred,
    //                  [](QMediaPlayer::Error err,const QString &estr){
    //                      qDebug()<<"err player "<<err<<" "<<estr;
    //                  });
    // player.setAudioOutput(&outpt);
    // QUrl nameurl("qrc:/music/tick-4-4-60bp");
    // qDebug()<< nameurl.errorString();
    // // qDebug()<<"assest path:"<<nameurl.path();
    // player.setSource(nameurl);
    // player.setPlaybackRate(2.2);
    // player.play();
    // qDebug()<<"EndPlayer!";
    // QObject::connect(&player, &QMediaPlayer::playbackStateChanged,
    //                  [&player](QMediaPlayer::PlaybackState state){
    //                      if(state==QMediaPlayer::PlaybackState::StoppedState)
    //                      {
    //                          qDebug()<<"replaying";
    //                          player.play();
    //                      }
    //                  });
    return a.exec();
}

/**/

/*

#include <app/appinfo.h>
#include <app/documentmanager.h>
#include <actions/undomanager.h>
#include <audio/midiplayer.h>
#include <app/settings.h>
#include <app/settingsmanager.h>
#include <QStandardPaths>
#include <formats/gp7/document.h>
#include <formats/gp7/document.cpp>

Paths::path fromQString(const QString &str)
{
    return std::filesystem::u8path(str.toStdString());
}

Paths::path getConfigDir()
{
    auto p = fromQString(
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    // On Linux, ConfigLocation is ~/.config, so append the application name.
#ifdef Q_OS_LINUX
    return p / AppInfo::APPLICATION_ID;
#else
    return p;
#endif

}
int main(int argc, char *argv[])
{
//    PowerTabEditor ptd;
    std::string ss = AppInfo::makeApplicationName();
    auto undoman = new UndoManager(nullptr);
    undoman->addNewUndoStack();
//    DocumentManager aa;
//    auto ff= Paths::getConfigDir();
    Paths::path pp=getConfigDir();
    //SettingsManager ssm;
    pugi::xml_document txt;

    Gp7::Document doc ;//= Gp7::from_xml(txt,Gp7::Version::V7);
    Gp7::to_xml(doc);
    return 0;
}

*/


// #include <amidi/AMidi.h>
// #include <android/log.h>

// void tessss()
// {
//     AMidiInputPort_send(nullptr,nullptr,0);
// }
