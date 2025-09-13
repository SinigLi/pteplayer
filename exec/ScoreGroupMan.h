#ifndef SCOREGROUPMAN_H
#define SCOREGROUPMAN_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QQmlEngine>

class ScoreGroupMan : public QObject
{
    Q_OBJECT
    // Q_PROPERTY(QStringList scoreGroups READ scoreGroups  NOTIFY scoreGroupChanged FINAL)
    QML_ELEMENT
public:
    explicit ScoreGroupMan(QObject *parent = nullptr);
    Q_INVOKABLE QStringList scoreGroups()const;


    Q_INVOKABLE QStringList getScoreNamesByGroup(const QString &groupName)const;
    Q_INVOKABLE QStringList getScoreNamesByGroupFilter(const QString &groupName,const QString &filter)const;
    Q_INVOKABLE QString newScoreGroup();
    Q_INVOKABLE void delScoreGroup(const QString &groupName);
    Q_INVOKABLE QString getScoreFilePath(const QString &groupname,const QString &scoreName)const;

signals:
    void scoreGroupChanged(QString groupname);
private:

};

#endif // SCOREGROUPMAN_H
