#include "ScoreFileLoader.h"
#include "paths.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
ScoreFileLoader &ScoreFileLoader::instace()
{
    static ScoreFileLoader instace;
    return instace;
}

void ScoreFileLoader::init()
{
    #ifdef WIN32
    Paths::path curdir =
        Paths::fromQString(QCoreApplication::applicationDirPath()) / "lscores";
    #else

    Paths::path curdir = Paths::getUserDataDir() / "lscores";
    #endif
    mBaseDir = Paths::toQString(curdir);
    if(!QDir(mBaseDir).exists())
    {
        QDir d(Paths::toQString(Paths::getUserDataDir()));
        if(!d.mkdir(mBaseDir))
        {
            qDebug()<<"create dir err:"<<curdir.c_str();
            return;
        }
    }
    mAllScoreDirNames.clear();
    for(const auto &oneDir:QDir(mBaseDir).entryInfoList())
    {
        if(!oneDir.isDir())
        {
            continue;
        }
        if(oneDir.isHidden()){
            continue;
        }
        if(oneDir.fileName()=="."
            ||oneDir.fileName()=="..")
        {
            continue;
        }
        mAllScoreDirNames.push_back(oneDir.fileName());
    }

    qDebug()<<"load score setting!";
    mSettingFile = mBaseDir+"/scores_setting.ini";
    QSettings mSetting = QSettings(mSettingFile,QSettings::IniFormat);
    mSetting.beginGroup("score_infos");
    for(QString onkey: mSetting.allKeys())
    {
        QString str = mSetting.value(onkey).toString();
        ScoreSettingInfos oneset;
        if(!oneset.fromString(str)){
            continue;
        }
        mScoreSettings[onkey]=oneset;
    }

    mSetting.endGroup();
    qDebug()<<"load score setting end "<<mScoreSettings.size();
}

bool ScoreFileLoader::loadScoreFromPath(const QString &dirpath,
                                   QString dirName)
{

    QDir curdir(dirpath);
    if(!curdir.exists())
    {
        return false;
    }
    if(dirName.isEmpty()){
        dirName = curdir.dirName();
    }
    bool hadAdd=false;
    for(const auto &onefile:curdir.entryInfoList())
    {
        if (onefile.isDir())
        {
            continue;
        }
        QFile fileread(onefile.absoluteFilePath());
        if(!fileread.open(QFile::OpenModeFlag::ReadOnly))
        {
            qDebug()<<"read err:"<<onefile;
            continue;
        }
        auto readAll = fileread.readAll();
        Paths::path curdir =Paths::fromQString(mBaseDir);// Paths::getUserDataDir()/dirName.toLocal8Bit().toStdString();
        curdir/=dirName.toLocal8Bit().toStdString();
        QDir d(mBaseDir);
        if(!d.exists(Paths::toQString(curdir)))
        {
            if(!d.mkdir(Paths::toQString(curdir)))
            {
                qDebug()<<"create dir err:"<<curdir.c_str();
                continue;
            }
        }
        QString curpath =Paths::toQString(curdir/onefile.fileName().toLocal8Bit().toStdString());
        // qDebug()<<"curpath"<<curpath;
        QFile localPath(curpath);
        if(!localPath.open(QFile::OpenModeFlag::WriteOnly))
        {
            continue;
        }
        localPath.write(readAll);
        localPath.close();
        hadAdd = true;
    }
    if(!hadAdd){
        return false;
    }
    mAllScoreDirNames.push_back(dirName);
    return hadAdd;
}

bool ScoreFileLoader::removeScoreSet(const QString &setName)
{
    int idx = mAllScoreDirNames.indexOf(setName);
    if(idx<0)
    {
        return false;
    }

    QDir curdir(mBaseDir+"/"+setName);
    if(!curdir.exists())
    {
        return true;
    }
    if(!curdir.removeRecursively())
    {
        return false;
    }
#ifdef WIN32
    mAllScoreDirNames.removeAt(idx);
#else
    mAllScoreDirNames.remove(idx);
#endif
    return true;
}

ScoreFileLoader::ScoreFileLoader()
{
}
void ScoreFileLoader::getScoreNames(const QString &scoreDir,
                               QStringList &scoreNames,
                               QStringList &scorePaths) const
{
    int indx = mAllScoreDirNames.indexOf(scoreDir);
    if(indx<0){
        return ;
    }
    QString dirName = mAllScoreDirNames.at(indx);
    QString subDirPath = mBaseDir+"/"+dirName;

    QDir curdir(subDirPath);
    for(const auto &onefile:curdir.entryInfoList())
    {
        if (onefile.isDir())
        {
            continue;
        }
        scoreNames.append(onefile.fileName());
        scorePaths.append(onefile.absoluteFilePath());
    }
}

bool ScoreFileLoader::getScoreSetting(const QString &scoreDir, const QString &scoreName, ScoreSettingInfos &infos) const
{
    QString key = scoreDir.trimmed().toLower()+"@"+scoreName.trimmed().toLower();
    if(!mScoreSettings.contains(key)){
        return false;
    }
    infos = mScoreSettings.value(key);
    return true;
}

void ScoreFileLoader::setScoreSetting(const QString &scoreDir, const QString &scoreName, const ScoreSettingInfos &infos)
{

    QString key = scoreDir.trimmed().toLower()+"@"+scoreName.trimmed().toLower();
    mScoreSettings[key]=infos;
    QSettings mSetting = QSettings(mSettingFile,QSettings::IniFormat);
    mSetting.beginGroup("score_infos");

    QString str = infos.toString();
    mSetting.setValue(key,str);
    mSetting.endGroup();
}

QString ScoreSettingInfos::toString() const
{
    QJsonDocument doc;
    QJsonObject obj;
    obj.insert("bpm",bpm);
    obj.insert("start_bar",start_bar);
    QJsonArray playerArr;
    for(const auto &one:player_infos)
    {
        QJsonObject oneobj;
        oneobj.insert("name",one.name);
        oneobj.insert("tab_stuff_show",one.tab_stuff_show);
        oneobj.insert("std_stuff_show",one.std_stuff_show);
        oneobj.insert("number_stuff_show",one.number_stuff_show);
        oneobj.insert("activated",one.activated);
        playerArr.push_back(oneobj);
    }
    obj.insert("player_infos",playerArr);
    doc.setObject(obj);
    QByteArray strarr = (doc.toJson());
    return QString::fromUtf8(strarr.toBase64());
}

bool ScoreSettingInfos::fromString(const QString &str)
{
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromBase64(str.toUtf8()));
    if(doc.isEmpty())
    {
        return false;
    }
    if(!doc.isObject())
    {
        return false;
    }
    QJsonObject obj = doc.object();
    bpm = obj.value("bpm").toInt();
    start_bar = obj.value("start_bar").toInt();
    QJsonArray playerarr = obj.value("player_infos").toArray();
    player_infos.clear();

    for(int i = 0;i<playerarr.count();++i)
    {
        PlayerSetInfo oneinfo;
        QJsonObject oneobj = playerarr[i].toObject();
        oneinfo.name = oneobj.value("name").toString();
        oneinfo.tab_stuff_show = oneobj.value("tab_stuff_show").toBool();
        oneinfo.std_stuff_show = oneobj.value("std_stuff_show").toBool();
        oneinfo.number_stuff_show = oneobj.value("number_stuff_show").toBool();
        oneinfo.activated = oneobj.value("activated").toBool();
        player_infos.push_back(oneinfo);
    }
    return true;
}
