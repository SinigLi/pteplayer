#include "ScoreGroupMan.h"
#include <app/ScoreFileLoader.h>
#include <QFileDialog>
ScoreGroupMan::ScoreGroupMan(QObject *parent) : QObject{ parent }
{
    ScoreFileLoader::instace().init();
}

QStringList ScoreGroupMan::scoreGroups() const
{
    return ScoreFileLoader::instace().getAllScoreDirNames();
}

QStringList ScoreGroupMan::getScoreNamesByGroup(const QString &groupName) const
{
    QStringList scoreNames;
    QStringList scorePaths;
    ScoreFileLoader::instace().getScoreNames(groupName,scoreNames,scorePaths);
    return scoreNames;
}

QStringList ScoreGroupMan::getScoreNamesByGroupFilter(const QString &groupName,
                                                      const QString &filter) const
{
    QStringList res = getScoreNamesByGroup(groupName);
    QStringList ret;
    for(QString one:res)
    {
        if(one.contains(filter,Qt::CaseInsensitive))
        {
            ret.push_back(one);
        }
    }
    return ret;
}

QString ScoreGroupMan::newScoreGroup()
{
    QString path = QFileDialog::getExistingDirectory(nullptr,QString::fromWCharArray(L"选择文件夹"));
    if(path.trimmed().isEmpty()){
        return "";
    }
    QString scoreSetName = QFileInfo(path).fileName();
    if(!ScoreFileLoader::instace().loadScoreFromPath(path,scoreSetName))
    {
        return "未成功加载有效谱:"+path;
    }

    emit scoreGroupChanged(scoreSetName);
    return "";

}

void ScoreGroupMan::delScoreGroup(const QString &groupName)
{
    if(!ScoreFileLoader::instace().isDirExist(groupName))
    {
        return;
    }
    ScoreFileLoader::instace().removeScoreSet(groupName);
    auto subname = ScoreFileLoader::instace().getAllScoreDirNames();
    QString ngroupname = "";
    if (!subname.empty())
    {
        ngroupname = subname.at(0);
    }
    emit scoreGroupChanged(ngroupname);
}

QString ScoreGroupMan::getScoreFilePath(const QString &groupname, const QString &scoreName) const
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
