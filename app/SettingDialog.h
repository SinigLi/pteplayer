#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}


class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();
    void setCurScoreSet(const QString &str);
    QString curScoreSet()const;
    QStringList allScoreNames()const;
    QStringList allScorePaths()const{
        return m_score_paths;
    }
    int normFileIndx()const;
private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

    void on_pushButton_selpath_clicked();

    void on_comboBox_curscoresets_currentIndexChanged(int index);

    void on_btn_del_clicked();

private:
    Ui::SettingDialog *ui;
    QString m_cur_score_set;
    int m_norm_file;
    QStringList m_scores;
    QStringList m_score_paths;
    void loadScoreFiles(const QString &path);
};

#endif // SETTINGDIALOG_H
