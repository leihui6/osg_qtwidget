#include    "mainwindow.h"
#include    "ui_mainwindow.h"

#include    <QGridLayout>
#include    <QFileDialog>
#include    <QWindow>

#include    <osg/Group>
#include    <osgDB/ReadFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , qviewer(Q_NULLPTR)
{
    ui->setupUi(this);
    //this->setAttribute(Qt::WA_NativeWindow, true);
    // make frame as a nativewindow, avoid the non-expose windows problem
    ui->frame->setAttribute(Qt::WA_NativeWindow, true);

    QGridLayout * layout = new QGridLayout;
    qviewer = new QViewerWidget(QRect(0, 0, ui->frame->width(), ui->frame->height()));
    layout->addWidget(qviewer);
    ui->frame->setLayout(layout);
    ui->verticalLayout->addWidget(ui->frame);

    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::quit);
    connect(ui->actionClean, &QAction::triggered, this, &MainWindow::clean);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);

    connect(&timer, &QTimer::timeout, this, &MainWindow::update);
    timer.start(10);

    this->setWindowTitle("QViewerWidget example");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if (windowHandle() && windowHandle()->isExposed())
    {
        ui->frame->update();
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
    osg::Group *scene = qviewer->getScene();

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
    ci.load_point_cloud_txt(path.toStdString(),points);
    std::cout << points.size()<<std::endl;

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    points_to_geometry_node(points, geometry, 255,255,255,1);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, int(points.size())));

    osg::ref_ptr<osg::Node> model = geometry;//osgDB::readNodeFile(path.toStdString());
    scene->addChild(model.get());
}

void MainWindow::clean()
{
    osg::Group *scene = qviewer->getScene();

    if (scene == nullptr)
        return;

    scene->removeChildren(0, scene->getNumChildren());
}

void MainWindow::quit()
{
    QApplication::quit();
}
