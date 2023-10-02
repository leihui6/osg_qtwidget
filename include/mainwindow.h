#ifndef     MAINWINDOW_H
#define     MAINWINDOW_H

#include    <QObject>
#include    <QMainWindow>
#include    <QTimer>
#include    <QTextEdit>


#include    "cloud_io.h"
#include    "qviewerwidget.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
namespace Ui
{
    class MainWindow;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
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

    int print2widget(QString text, QTextEdit * textEdit);

    Ui::MainWindow * m_ui;

    QViewerWidget * m_qviewer;

    QTimer timer;

    void paintEvent(QPaintEvent *);

    cloud_io m_ci;

private slots:

    void update();

    void open();

    void clean();

    void quit();

    void on_actionAdd_XYZ_axes_toggled(bool arg1);
};

#endif // MAINWINDOW_H
