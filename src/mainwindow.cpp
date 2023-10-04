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

void MainWindow::set_background_color(osg::Vec4f &c)
{
    osg::ref_ptr<osg::Camera> camera = m_qviewer->getViewer()->getCamera();
    camera->setClearColor(
                osg::Vec4(
                    c[0]/255,
                    c[1]/255.0f,
            c[2]/255.0f,c[3]));
}

void MainWindow::set_pointcloud_color(osg::Vec4f &c)
{
    QVector<QString> pointcloud_list = m_qviewer->get_pointcloud_list();
    for (QString & pcn: pointcloud_list)
    {
        // update all pointclouds with the new color
        if (pcn.indexOf("pointcloud") != -1)
        {
            qDebug() << pcn;
            m_qviewer->set_pointcloud_color(pcn, c);
        }
    }
}

void MainWindow::set_pointcloud_size(int point_size)
{
    QVector<QString> pointcloud_list = m_qviewer->get_pointcloud_list();
    for (QString & pcn: pointcloud_list)
    {
        // update all pointclouds with the new color
        if (pcn.indexOf("pointcloud") != -1)
        {
            qDebug() << pcn;
            m_qviewer->set_pointcloud_size(pcn, point_size);
        }
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

    //scene->removeChildren(0, scene->getNumChildren());

    std::vector<point_3d> points;
    m_ci.load_point_cloud_txt(path, points);
    std::cout << points.size()<<std::endl;
    m_qviewer->add_point_cloud(points, "pointcloud_1");
}

void MainWindow::clean()
{
    m_qviewer->clean();
}

void MainWindow::quit()
{
    this->clean();
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
    bool is_action_found = false;
    foreach (QAction *action, this->menuBar()->actions())
    {
        foreach (QAction *subaction, action->menu()->actions())
        {
            if(subaction->text() == action_name)
            {
                action->setCheckable(true);
                return subaction;
            }
        }
    }
    if (!is_action_found)
    {} // TODO Warning
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
    m_setting_wiondow = new SettingsDialog(this);
    QString m_title = "Default Settings";
    m_setting_wiondow->setWindowTitle(m_title);
    m_setting_wiondow->set_default_value(m_qviewer->m_bgc, m_qviewer->m_pcc, m_qviewer->m_pcs);

    m_setting_wiondow->setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
    m_setting_wiondow->setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);

    auto fontMetrics = m_setting_wiondow->fontMetrics();
    auto width = fontMetrics.boundingRect(m_title).width();
    m_setting_wiondow->setFixedSize(width*4, m_setting_wiondow->rect().height());

    if (m_setting_wiondow->exec() == QDialog::Accepted)
    {
        qDebug() << "accepted";

        m_qviewer->m_bgc = m_setting_wiondow->m_bgc;
        m_qviewer->m_pcc = m_setting_wiondow->m_pcc;
        m_qviewer->m_pcs = m_setting_wiondow->m_pcs;
        m_setting_wiondow = nullptr;

        //dialog_settings_ui->get_configuration();
        //qDebug() <<"bgc"<< m_qviewer->m_bgc[0] << m_qviewer->m_bgc[1] << m_qviewer->m_bgc[2];
        //qDebug() <<"pcc"<< m_qviewer->m_pcc[0] << m_qviewer->m_pcc[1] << m_qviewer->m_pcc[2];
        //qDebug() <<"pcs"<< m_qviewer->m_pcs;
        set_background_color(m_qviewer->m_bgc);
        set_pointcloud_color(m_qviewer->m_pcc);
        set_pointcloud_size(m_qviewer->m_pcs);
    }
    else
    {
        //qDebug() << "rejected";
    }
}
