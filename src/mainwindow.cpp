#include    "mainwindow.h"
#include    "ui_mainwindow.h"

#include    <QGridLayout>
#include    <QFileDialog>
#include    <QWindow>

#include    <osg/Group>
#include    <osgDB/ReadFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_add_xyz_text("Add XYZ axes")
    , m_ui(new Ui::MainWindow)
    , m_qviewer(Q_NULLPTR)

{
    m_ui->setupUi(this);
    //this->setAttribute(Qt::WA_NativeWindow, true);
    // make frame as a nativewindow, avoid the non-expose windows problem
    m_ui->frame->setAttribute(Qt::WA_NativeWindow, true);

    QGridLayout * layout = new QGridLayout;
    m_qviewer = new QViewerWidget(QRect(0, 0, m_ui->frame->width(), m_ui->frame->height()));
    layout->addWidget(m_qviewer);
    m_ui->frame->setLayout(layout);
    m_ui->verticalLayout->addWidget(m_ui->frame);

    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::quit);
    connect(m_ui->actionClean, &QAction::triggered, this, &MainWindow::clean);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::open);

    connect(&timer, &QTimer::timeout, this, &MainWindow::update);
    timer.start(10);

    initialization_scene();

    this->setWindowTitle("QViewerWidget example");
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

int MainWindow::initialization_scene()
{
    set_menu_action(m_add_xyz_text, true);

    m_qviewer->show_XYZ_axes();
    m_qviewer->activate_XYZ_axes(true);

    return 0;
}

int MainWindow::print2widget(QString text, QTextEdit *textEdit )
{
    QString pre = ">";

    textEdit->append(pre + text);

    return textEdit->toPlainText().size();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if (windowHandle() && windowHandle()->isExposed())
    {
        m_ui->frame->update();
    }
}

void MainWindow::update()
{
    if (windowHandle() && windowHandle()->isExposed())
    {
        QMainWindow::update(this->geometry());
    }

}

void MainWindow::open()
{
    osg::Group *scene = m_qviewer->getScene();

    if (scene == nullptr)
        return;

    QString path = QFileDialog::getOpenFileName(Q_NULLPTR,
                                                tr("Open model file"),
                                                "./",
                                                tr("OpenSceneGraph (*.txt)"));

    if (path.isEmpty())
        return;

    scene->removeChildren(0, scene->getNumChildren());

    std::vector<point_3d> points;
    m_ci.load_point_cloud_txt(path.toStdString(), points);
    std::cout << points.size()<<std::endl;
    m_qviewer->add_point_cloud(points, "point_cloud_1");
}

void MainWindow::clean()
{
    osg::Group *scene = m_qviewer->getScene();

    if (scene == nullptr)
        return;

    scene->removeChildren(0, scene->getNumChildren());
}

void MainWindow::quit()
{
    QApplication::quit();
}

bool MainWindow::set_menu_action(const QString & action_name, bool action_status)
{
    QAction * action = get_menu_action(action_name);

    bool last_checked_status = action->isChecked();

    action->setChecked(action_status);

    return last_checked_status;
}

QAction * MainWindow::get_menu_action(const QString & action_name)
{
    foreach (QAction *action, this->menuBar()->actions())
    {
        foreach (QAction *subaction, action->menu()->actions())
        {
            if(subaction->text() == action_name)
            {
                action->setCheckable(true);
                std::cout << "found" <<std::endl;
                return subaction;
            }
        }
    }

    return nullptr;
}

void MainWindow::on_actionAdd_XYZ_axes_toggled(bool arg1)
{
    if (arg1 == true)
    {
        m_qviewer->activate_XYZ_axes(true);
        print2widget("Activate Coordinate Frame", m_ui->textEdit);
    }
    else {
        m_qviewer->activate_XYZ_axes(false);
        print2widget("Deactivate Coordinate Frame", m_ui->textEdit);
    }
}

void MainWindow::on_actionColors_triggered()
{

}
