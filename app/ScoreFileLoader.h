#ifndef SCOREFILELOADER_H
#define SCOREFILELOADER_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QSettings>
struct ScoreSettingInfos
{
    ScoreSettingInfos() {}
    int bpm = 100;
    int start_bar = 1;
    struct PlayerSetInfo
    {
        PlayerSetInfo() {}
        QString name;
        bool tab_stuff_show = false;
        bool std_stuff_show = false;
        bool number_stuff_show = false;
        bool activated = false;

    };
    QList<PlayerSetInfo> player_infos;
    QString toString()const;
    bool fromString(const QString &str);
};
class ScoreFileLoader
{
public:
    static ScoreFileLoader &instace();
    void init();
    bool loadScoreFromPath(const QString &dirpath,
                           QString dirName="");
    bool removeScoreSet(const QString& setName);
    QStringList getAllScoreDirNames()const{
        return mAllScoreDirNames;
    }
    bool isDirExist(const QString &dir)const
    {
        return mAllScoreDirNames.indexOf(dir)>=0;
    }
    QString baseDir()const
    {
        return mBaseDir;
    }
    void getScoreNames(const QString &scoreDir,QStringList &scoreNames,
                       QStringList &scorePaths) const;

    bool getScoreSetting(const QString &scoreDir,
                         const QString &scoreName,
                         ScoreSettingInfos &infos)const;
    void setScoreSetting(const QString &scoreDir,
                         const QString &scoreName,
                         const ScoreSettingInfos &infos);
private:
    ScoreFileLoader();
    QStringList mAllScoreDirNames;
    QString mBaseDir;
    QString mSettingFile;
    // std::unique_ptr<QSettings> mSetting;
    QMap<QString,ScoreSettingInfos> mScoreSettings;
};


#endif // SCOREFILELOADER_H
