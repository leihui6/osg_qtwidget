#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
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
    // window_model: 0: default setting; 1: settings for one point cloud
    explicit SettingsDialog(const QString window_name, int window_model, QWidget *parent = nullptr);
    ~SettingsDialog();

    int set_default_value(pointcloud_params & pc_params);

    // default background color
    pointcloud_params m_pc_params;

    bool if_save_as_config();
    bool if_background_apply_now();
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
