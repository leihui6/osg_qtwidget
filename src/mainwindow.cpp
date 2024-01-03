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
    , m_pc_num(0)
    , m_current_pointcloud_name(NONE)
    , m_ui(new Ui::MainWindow)
    , m_qviewer(Q_NULLPTR)
    , p_urobot(nullptr)
    , p_vsystem(nullptr)
{
    m_ui->setupUi(this);
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

    initWidgets();
    initDevices();
    this->setWindowTitle(TITLE);
}

MainWindow::~MainWindow()
{
    if (p_urobot)
        delete p_urobot;

    if (p_vsystem)
        delete p_vsystem;

    delete m_ui;
}

int MainWindow::initWidgets()
{
    setLabelText(*m_ui->label_cam_signal, " ");
    setLabelText(*m_ui->label_urob_signal, " ");
    setLabelColor(*m_ui->label_cam_signal, "red");
    setLabelColor(*m_ui->label_urob_signal, "red");
    m_ui->pbt_scanning->setEnabled(true);

    set_menu_action(m_add_xyz_text, true);

    m_qviewer->show_XYZ_axes();

    m_ui->pbt_del->setEnabled(false);
    m_ui->pbt_edit->setEnabled(false);

    return 0;
}

void MainWindow::initDevices()
{
    QMessageBox msgBox;
    // UR Robot
    try
    {
        p_urobot = new URobot("192.168.1.254");
        setLabelColor(*m_ui->label_cam_signal, "green");
    }
    catch (const std::exception&)
    {
        msgBox.setText("UR Robot Initialization Failed");
        msgBox.exec();
        setLabelColor(*m_ui->label_cam_signal, "red");
        m_ui->pbt_scanning->setEnabled(false);
    }

    // VisionSystem
    try
    {
        p_vsystem = new VisionSystem("D:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe");
        setLabelColor(*m_ui->label_urob_signal, "green");
        p_vsystem->fittingCylidner("data/CylinderPoints.txt", cylinderPoint, cylinderAxis);

        // add the axis of cylinder
        m_qviewer->add_line_segment(
                    point_3d(cylinderPoint[0], cylinderPoint[1],cylinderPoint[2]),
                    point_3d(
                    cylinderPoint[0] + 100*cylinderAxis[0],
                    cylinderPoint[1] + 100*cylinderAxis[1],
                    cylinderPoint[2] + 100*cylinderAxis[2]),
                    "cylinderAxis", 2);
        update_control_panel();

    }
    catch (const std::exception&)
    {
        msgBox.setText("3D VisionSystem Initialization Failed");
        msgBox.exec();
        setLabelColor(*m_ui->label_urob_signal, "red");
        m_ui->pbt_scanning->setEnabled(false);
    }
}

void MainWindow::setLabelColor(QLabel & qlabel, QString color)
{
    QString color_text =
        QString("min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px; background:%1;").arg(color);
    qlabel.setStyleSheet(color_text);
}

void MainWindow::setLabelText(QLabel & qlabel, QString text)
{
    qlabel.setEnabled(true);
    qlabel.clear();
    qlabel.setText(text);
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
        if (pcn.indexOf(PCNAME) != -1)
        {
            //qDebug() << pcn;
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
        if (pcn.indexOf(PCNAME) != -1)
        {
            //qDebug() << pcn;
            m_qviewer->set_pointcloud_size(pcn, point_size);
        }
    }
}

void MainWindow::update_control_panel()
{
    QVector<QString> pc_name_list = m_qviewer->get_pointcloud_list();

    m_ui->listWidget->clear();
    for(auto & pc_name:pc_name_list)
        m_ui->listWidget->addItem(pc_name);
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
    //std::cout << points.size()<<std::endl;
    m_qviewer->add_point_cloud(points, PCNAME + ("#" + QVariant(m_pc_num).toString()));
    m_pc_num += 1;

    update_control_panel();
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
    m_setting_wiondow = new SettingsDialog("Default Settings", 0, this);
    m_setting_wiondow->set_default_value(m_qviewer->m_pc_params);

    if (m_setting_wiondow->exec() == QDialog::Accepted)
    {
        qDebug() << "accepted";

        m_qviewer->m_pc_params = m_setting_wiondow->m_pc_params;

        //dialog_settings_ui->get_configuration();
        //qDebug() <<"bgc"<< m_qviewer->m_bgc[0] << m_qviewer->m_bgc[1] << m_qviewer->m_bgc[2];
        //qDebug() <<"pcc"<< m_qviewer->m_pcc[0] << m_qviewer->m_pcc[1] << m_qviewer->m_pcc[2];
        //qDebug() <<"pcs"<< m_qviewer->m_pcs;
        //set_background_color(m_qviewer->m_bgc);
        //set_pointcloud_color(m_qviewer->m_pcc);
        //set_pointcloud_size(m_qviewer->m_pcs);

        if (m_setting_wiondow->if_save_as_config())
        {
            m_qviewer->save_config();
        }

        if (m_setting_wiondow->if_background_apply_now())
        {
            set_background_color(m_qviewer->m_pc_params.background_color);
        }
    }
    else
    {
        //qDebug() << "rejected";
    }
    m_setting_wiondow = nullptr;
}

void MainWindow::on_pbt_edit_clicked()
{
    if (m_ui->listWidget->count() == 0 || !m_ui->listWidget->currentItem())
        return ;

    QString current_pointcloud_name;
    if (m_current_pointcloud_name != NONE)
    {
        current_pointcloud_name = m_ui->listWidget->currentItem()->text();
        //qDebug() << current_pointcloud_name;
    }

    // TODO Edit window

    m_setting_wiondow = new SettingsDialog(current_pointcloud_name + "-Settings", 1, this);
    m_setting_wiondow->set_default_value(m_qviewer->m_pc_params);

    if (m_setting_wiondow->exec() == QDialog::Accepted)
    {
        //qDebug() << "accepted";
        set_background_color(m_setting_wiondow->m_pc_params.background_color);
        m_qviewer->set_pointcloud_color(current_pointcloud_name, m_setting_wiondow->m_pc_params.pointcloud_color);
        m_qviewer->set_pointcloud_size(current_pointcloud_name, m_setting_wiondow->m_pc_params.pointcloud_size);

    }
    else
    {
        //qDebug() << "rejected";
    }
    m_setting_wiondow = nullptr;
    // end for edit window

    m_current_pointcloud_name = NONE;
    m_ui->pbt_edit->setEnabled(false);
    m_ui->pbt_del->setEnabled(false);
    m_ui->listWidget->clearSelection();
}


void MainWindow::on_pbt_del_clicked()
{
    if (m_ui->listWidget->count() == 0 || !m_ui->listWidget->currentItem())
        return ;

    QString current_pointcloud_name;
    if (m_current_pointcloud_name != NONE)
    {
        current_pointcloud_name = m_ui->listWidget->currentItem()->text();
        //qDebug() << current_pointcloud_name;
    }

    bool click_status = false;
    QMessageBox::StandardButton reply = QMessageBox::question(this, current_pointcloud_name , "Delete pointcloud \"" + current_pointcloud_name + "\" ?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        click_status = true;
    } else
    {
        click_status = false;
    }

    // No
    if (!click_status)
    {
        return ;
    }
    // Yes
    m_qviewer->del_point_cloud(current_pointcloud_name);

    QList<QListWidgetItem*> items = m_ui->listWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        if (item->text() == current_pointcloud_name)
        {
            delete m_ui->listWidget->takeItem(m_ui->listWidget->row(item));
        }
    }

    m_current_pointcloud_name = NONE;
    m_ui->pbt_edit->setEnabled(false);
    m_ui->pbt_del->setEnabled(false);
    m_ui->listWidget->clearSelection();
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    m_current_pointcloud_name = item->text();

    if (m_ui->listWidget->count() > 0)
    {
        m_ui->pbt_edit->setEnabled(true);
        m_ui->pbt_del->setEnabled(true);
    }
}

void MainWindow::on_pbt_scanning_clicked()
{
    float eachAngle = 20;
    if (p_urobot->isConnected() && p_vsystem->isConnected())
    {
        // scan around 360 degree
        for (size_t i = 0; i < 3; i++)
        {
            // scan the object
            std::vector<VST3D_PT> capturedPointsOnce, t_capturedPointsOnce;
            qDebug() << "scanning";
            p_vsystem->scanOnce(capturedPointsOnce);
            qDebug()<<"capturedPointsOnce.size:"<<capturedPointsOnce.size();
            std::string filename = "./Points_"+std::to_string(i) +".txt";
            p_vsystem->save2File(capturedPointsOnce, filename);

            std::vector<point_3d> pointsVec;
            VST3D_to_points3D(capturedPointsOnce, pointsVec);
            m_qviewer->add_point_cloud(pointsVec, QString("scannedPoint#") + QString::number(i));

            Eigen::Matrix4f tsfm = p_vsystem->generateRMatrixAlongAxis(cylinderPoint,cylinderAxis,eachAngle);
            p_vsystem->transformPointcloud(capturedPointsOnce, tsfm, t_capturedPointsOnce);
            //m_pc_num += 1;
            update_control_panel();

            // rotate the robot flange
            qDebug() << "moving";
            p_urobot->rotateAlongZ(20);
        }
    }
}
