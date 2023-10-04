#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>

// child window
#include <settingsdialog.h>

#include "cloud_io.h"
#include "qviewerwidget.h"

namespace Ui
{
    class MainWindow;
}

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
    int initialization_scene();
private:
    cloud_io m_ci;
    Ui::MainWindow * m_ui;
    QViewerWidget * m_qviewer;
    QTimer timer;

    void paintEvent(QPaintEvent *);
    int print2widget(QString text, QTextEdit * textEdit);
    SettingsDialog * m_setting_wiondow;
    void set_background_color(osg::Vec4f & c);
    void set_pointcloud_color(osg::Vec4f & c);
    void set_pointcloud_size(int point_size);
private slots:
    void update();
    void open();
    void clean();
    void quit();
    void on_actionAdd_XYZ_axes_toggled(bool arg1);
    void on_actionColors_triggered();
};

#endif // MAINWINDOW_H
