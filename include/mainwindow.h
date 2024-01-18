#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QLabel>
#include <QPushButton>
#include <QObject>
#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDateTime>

// child window
#include <settingsdialog.h>

#include "URobot.h"
#include "VIsionSystem.h"
#include "cloud_io.h"
#include "qviewerwidget.h"

#define TITLE "DEMO"
#define PCNAME "pointcloud"

#define NONE ""

namespace Ui
{
    class MainWindow;
}

class InitDevicesThread : public QThread {
private:
    void run() {
        qDebug() << "Running" ;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString m_add_xyz_text;
    QAction * get_menu_action(const QString & action_name);
    bool set_menu_action(const QString & action_name, bool action_status);
    int initWidgets();
private:
    QString m_current_pointcloud_name;
    size_t m_pc_num;
    cloud_io m_ci;
    Ui::MainWindow * m_ui;
    QViewerWidget * m_qviewer;
    QTimer timer;
    SettingsDialog * m_setting_wiondow;

    void paintEvent(QPaintEvent *);
    int print2widget(QString text, QTextEdit * textEdit);

    void set_background_color(osg::Vec4f & c);
    void set_pointcloud_color(osg::Vec4f & c);
    void set_pointcloud_size(int point_size);
    void update_control_panel();

    bool initDevices(int deviceID);
    void setLabelColor(QLabel & qlabel, QString color);
    void setLabelText(QLabel & qlabel, QString text);
    URobot * p_urobot;
    Eigen::Vector3f cylinderPoint; Eigen::Vector3f cylinderAxis;
    VisionSystem * p_vsystem;

private slots:
    void update();
    void open();
    void clean();
    void quit();
    void on_actionAdd_XYZ_axes_toggled(bool arg1);
    void on_actionColors_triggered();
    void on_pbt_edit_clicked();
    void on_pbt_del_clicked();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_ptb_camera_clicked();
    void on_ptb_urobot_clicked();
    void on_pbt_onceScan_clicked();
    void on_pbt_autoScan_clicked();
};

#endif // MAINWINDOW_H
