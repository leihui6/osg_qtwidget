#include    "mainwindow.h"
#include    "ui_mainwindow.h"

#include    <QGridLayout>
#include    <QFileDialog>
#include    <QWindow>
#include    <QThread>

#include    <osg/Group>
#include    <osgDB/ReadFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_add_xyz_text("Add XYZ axes")
    , m_current_pointcloud_name(NONE)
    , m_pc_num(0)
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
    timer.setInterval(1000/60.0);
    timer.start();

    initWidgets();

    //InitDevicesThread thread;
    //thread.start();
    //thread.wait();

    update_control_panel();
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
    //m_ui->pbt_scanning->setEnabled(true);

    set_menu_action(m_add_xyz_text, true);

    m_qviewer->show_XYZ_axes();

    m_ui->pbt_del->setEnabled(false);
    m_ui->pbt_edit->setEnabled(false);

    return 0;
}

bool MainWindow::initDevices(int deviceID)
{
    QMessageBox msgBox;
    if (deviceID == 0)
    {
        // UR Robot
        try
        {
            if (p_urobot) delete p_urobot;
            p_urobot = new URobot("192.168.1.254");
            setLabelColor(*m_ui->label_urob_signal, "green");
            return true;
        }
        catch (const std::exception& e)
        {
            qDebug() << e.what();
            msgBox.setText("UR Robot Initialization Failed");
            msgBox.exec();
            setLabelColor(*m_ui->label_urob_signal, "red");
            //m_ui->pbt_scanning->setEnabled(false);
            return false;
        }
    }
    else if (deviceID == 1)
    {
        // VisionSystem
        try
        {
            if (p_vsystem) delete p_vsystem;
            p_vsystem = new VisionSystem("D:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe");
            setLabelColor(*m_ui->label_cam_signal, "green");
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
            return true;
        }
        catch (const std::exception&)
        {
            msgBox.setText("3D VisionSystem Initialization Failed");
            msgBox.exec();
            setLabelColor(*m_ui->label_cam_signal, "red");
            //m_ui->pbt_scanning->setEnabled(false);
            return false;
        }
    }
    return false;
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
//        QScreen* ws =  windowHandle()->screen();
//        if (ws) {
//            double rate = ws->refreshRate();
//            qDebug() <<"rate: "<< rate; // 59
//        }
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
    m_qviewer->add_point_cloud(points, PCNAME + ("#" + QString::number(m_pc_num)));
    m_pc_num += 1;

    print2widget("Open " + path + "(" + QString::number(points.size()) + ")", m_ui->textEdit);

    update_control_panel();
}

void MainWindow::clean()
{
    m_qviewer->clean();
    m_pc_num = 0;

    update_control_panel();
    print2widget("All data cleaned.", m_ui->textEdit);
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

    if (current_pointcloud_name == AXIS_X ||
            current_pointcloud_name == AXIS_Y ||
            current_pointcloud_name == AXIS_Z)
    {
        QMessageBox msgBox;
        msgBox.setText("AXIS is not supported to be removed.");
        msgBox.exec();
        return ;
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

void MainWindow::on_pbt_autoScan_clicked()
{
    bool ok;
    unsigned int autoTime = QInputDialog::getInt(this, "Set value",
                   "Set the number of scans:", 2, 2, 18, 1, &ok) ;
    if (!ok) return ;

    float eachAngle = 20;
    //size_t total_scanning = 18;
    if (p_urobot == nullptr || p_vsystem == nullptr)
    {
        QMessageBox msgBox;
        msgBox.setText("UR Robot or 3D Camera is not initialized.");
        msgBox.exec();
    }
    else
    {
        print2widget(QString("Start scanning with ")+QString::number(autoTime) + " times.", m_ui->textEdit);
        p_urobot->rotate2Zero();
        // scan around 360 degree
        for (size_t i = 0; i < autoTime; i++)
        {
            print2widget("#" + QString::number(i+1) + ": Scanning", m_ui->textEdit);

            // Object Scanning
            std::vector<VST3D_PT> capturedPointsOnce, t_capturedPointsOnce;
            qDebug() << "scanning";
            p_vsystem->scanOnce(capturedPointsOnce);
            qDebug()<<"capturedPointsOnce.size:"<<capturedPointsOnce.size();

            // Transform point cloud
            Eigen::Matrix4f tsfm = p_vsystem->generateRMatrixAlongAxis(cylinderPoint,cylinderAxis,eachAngle * (i + 1));
            p_vsystem->transformPointcloud(capturedPointsOnce, tsfm, t_capturedPointsOnce);

            // Save to file for saving
            std::string filename = "./data/t_Points_"+std::to_string(i) +".txt";
            p_vsystem->save2File(t_capturedPointsOnce, filename);

            // Visulizalize the tranformed point cloud
            std::vector<point_3d> pointsVec;
            VST3D_to_points3D(t_capturedPointsOnce, pointsVec);
            m_qviewer->add_point_cloud(pointsVec, QString("ScannedPoint#") + QString::number(m_pc_num));
            m_pc_num += 1;

            this->m_qviewer->getViewer()->frame();
            update_control_panel();

            if (i == autoTime - 1)
            {
                break;
            }
            // rotate the robot flange
            p_urobot->rotateAlongZ(-eachAngle);
        }
    }
}


void MainWindow::on_ptb_urobot_clicked()
{
    initDevices(0);
}

void MainWindow::on_ptb_camera_clicked()
{
    initDevices(1);
}

void MainWindow::on_pbt_onceScan_clicked()
{
    if (p_urobot == nullptr || p_vsystem == nullptr)
    {
        QMessageBox msgBox;
        msgBox.setText("UR Robot or 3D Camera is not initialized.");
        msgBox.exec();
    }
    else
    {
        p_urobot->rotate2Zero();
        std::vector<VST3D_PT> capturedPointsOnce;
        qDebug() << "scanning";
        p_vsystem->scanOnce(capturedPointsOnce);
        qDebug()<<"capturedPointsOnce.size:"<<capturedPointsOnce.size();

        // Save to file for saving
        QDateTime dateTime= QDateTime::currentDateTime();
        QString dateStr = dateTime.toString("yyyy_MM_dd_hh_mm_ss");
        QString filename = "./data/PointsScanOnce_"+ dateStr +".txt";
        p_vsystem->save2File(capturedPointsOnce, filename.toStdString());

        // Visulizalize the tranformed point cloud
        std::vector<point_3d> pointsVec;
        VST3D_to_points3D(capturedPointsOnce, pointsVec);
        m_qviewer->add_point_cloud(pointsVec, QString("ScannedPoint#") + QString::number(m_pc_num));
        m_pc_num += 1;

        //this->update();
        update_control_panel();
    }
}
