#include "SettingDialog.h"
#include "ui_SettingDialog.h"
#include <QFileDialog>
#include "paths.h"
#include "ScoreFileLoader.h"
#include <QDebug>
SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    QStringList allScoreSet = ScoreFileLoader::instace().getAllScoreDirNames();
    ui->comboBox_curscoresets->addItems(allScoreSet);
    if(!allScoreSet.empty())
    {
        // qDebug()<<"setCurScoreSet 0";
        setCurScoreSet(allScoreSet[0]);
    }
}
SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setCurScoreSet(const QString &str)
{
//    m_cur_path = str;
    // loadScoreFiles(str);
    if(str.trimmed().isEmpty())
    {
        return;
    }
    blockSignals(true);
    ui->comboBox_curscores->blockSignals(true);
    ui->comboBox_curscoresets->blockSignals(true);
    m_cur_score_set = str;
    m_scores.clear();
    m_score_paths.clear();
    qDebug()<<"setCurScoreSet 0.1";
    ScoreFileLoader::instace()
        .getScoreNames(m_cur_score_set,
                       m_scores,
                       m_score_paths);
    qDebug()<<"setCurScoreSet 1";
    ui->comboBox_curscoresets->clear();
    QStringList allScoreSet = ScoreFileLoader::instace().getAllScoreDirNames();
    ui->comboBox_curscoresets->addItems(allScoreSet);
    ui->comboBox_curscoresets->setCurrentText(m_cur_score_set);
    ui->comboBox_curscores->clear();
    ui->comboBox_curscores->addItems(m_scores);
    m_norm_file = 0;
    if(m_scores.empty())
    {
        m_norm_file = -1;
    }
    blockSignals(false);
    ui->comboBox_curscores->blockSignals(false);
    ui->comboBox_curscoresets->blockSignals(false);
}

QString SettingDialog::curScoreSet() const{
    return m_cur_score_set;
}



QStringList SettingDialog::allScoreNames() const
{
    return m_scores;
}

int SettingDialog::normFileIndx() const
{
    return m_norm_file;
}

void SettingDialog::on_btn_ok_clicked()
{
    m_norm_file = ui->comboBox_curscores->currentIndex();
    accept();
}


void SettingDialog::on_btn_cancel_clicked()
{
    reject();
}


void SettingDialog::loadScoreFiles(const QString &path)
{
    if (path.trimmed().isEmpty())
    {
        return;
    }
    QString scoreSetName = QFileInfo(path).fileName();
    ScoreFileLoader::instace().loadScoreFromPath(path,scoreSetName);
    setCurScoreSet(scoreSetName);
    // QDir curdir(path);
    // QStringList allFiles;
    // QStringList fileNames;
    // for(const auto &onefile:curdir.entryInfoList())
    // {
    //     if (onefile.isDir())
    //     {
    //         continue;
    //     }
    //     QFile fileread(onefile.absoluteFilePath());
    //     if(!fileread.open(QFile::OpenModeFlag::ReadOnly))
    //     {
    //         qDebug()<<"read err:"<<onefile;
    //         continue;
    //     }
    //     auto readAll = fileread.readAll();
    //     Paths::path curdir = Paths::getUserDataDir()/"tempdir";
    //     QDir d(Paths::toQString(Paths::getUserDataDir()));
    //     if(!d.mkdir(Paths::toQString(curdir)))
    //     {
    //         qDebug()<<"create dir err:"<<curdir;
    //         continue;
    //     }
    //     QString curpath =Paths::toQString(curdir/onefile.fileName().toLocal8Bit().toStdString());
    //     qDebug()<<"curpath"<<curpath;
    //     QFile localPath(curpath);
    //     if(!localPath.open(QFile::OpenModeFlag::WriteOnly))
    //     {
    //         continue;
    //     }
    //     localPath.write(readAll);
    //     localPath.close();
    //     fileNames.append(onefile.fileName());
    //     allFiles.append(curpath);
    // }
    // if(allFiles.empty())
    // {
    //     return;
    // }
    // m_files = allFiles;
    // // ui->lineEdit_path->setText(path);
    // ui->comboBox_orgscore->addItems(fileNames);
    // m_cur_path = path;
    // m_norm_file = 0;
}

void SettingDialog::on_pushButton_selpath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,QString::fromWCharArray(L"选择文件夹"));
    if(path.trimmed().isEmpty()){
        return;
    }

    loadScoreFiles(path);

}


void SettingDialog::on_comboBox_curscoresets_currentIndexChanged(int )
{
    setCurScoreSet(ui->comboBox_curscoresets->currentText());
}


void SettingDialog::on_btn_del_clicked()
{
    QString curSet = ui->comboBox_curscoresets->currentText();
    if(!ScoreFileLoader::instace().removeScoreSet(curSet))
    {
        return;
    }
    QStringList allLs = ScoreFileLoader::instace().getAllScoreDirNames();
    if(allLs.empty())
    {
        m_score_paths.clear();
        m_scores.clear();
        m_cur_score_set.clear();
        ui->comboBox_curscores->clear();
        ui->comboBox_curscoresets->clear();
    }
    else
    {
        setCurScoreSet(allLs[0]);
    }
}

