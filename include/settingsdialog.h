#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QDebug>

#include <iostream>

#include "cloud_geometry.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    int set_default_value(osg::Vec4f & _bgc, osg::Vec4f &_pcc, int _pcs);

    // default background color
    osg::Vec4f m_bgc;
    // default pointcloud color
    osg::Vec4f m_pcc;
    // default pointcloud size
    int m_pcs;
private slots:
    void on_btn_bgc_clicked();
    void on_btn_pcc_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    QString rgb2str(osg::Vec4f & c);
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
