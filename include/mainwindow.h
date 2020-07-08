#ifndef     MAINWINDOW_H
#define     MAINWINDOW_H

#include    <QMainWindow>
#include    <QTimer>

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

    Ui::MainWindow *ui;

    QViewerWidget *qviewer;

    QTimer timer;

    void paintEvent(QPaintEvent *);

    cloud_io ci;

private slots:

    void update();

    void open();

    void clean();

    void quit();
};

#endif // MAINWINDOW_H
