#include "qviewerwidget.h"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <QWindow>
#include <QDebug>
#include <QGridLayout>
#include <Qt>
#include <QOpenGLContext>

QViewerWidget::QViewerWidget(const QRect &geometry)
    : QWidget()
    , scene(new osg::Group)
{
    this->setAttribute(Qt::WA_NativeWindow, true);

    initCamera(geometry);

    viewer.setSceneData(scene);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    osgQt::GraphicsWindowQt *gw = static_cast<osgQt::GraphicsWindowQt *>(viewer.getCamera()->getGraphicsContext());

    QGridLayout *layout = new QGridLayout;

    if (layout != Q_NULLPTR)
    {
        layout->addWidget(gw->getGLWidget());
        this->setLayout(layout);
    }
}

QViewerWidget::~QViewerWidget()
{

}

osg::Group *QViewerWidget::getScene()
{
    return scene.get();
}

osgViewer::Viewer *QViewerWidget::getViewer()
{
    return &viewer;
}

osgQt::GraphicsWindowQt *QViewerWidget::createGraphicsWindow(const QRect &geometry)
{
    osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "";
    traits->windowDecoration = true;
    traits->x = geometry.x();
    traits->y = geometry.y();
    traits->width = geometry.width();
    traits->height = geometry.height();

    if (traits->height == 0) traits->height = 1;

    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();
    traits->vsync = false;

    return new osgQt::GraphicsWindowQt(traits.get());
}

void QViewerWidget::initCamera(const QRect &geometry)
{
    osg::Camera *camera = viewer.getCamera();

    m_gw = createGraphicsWindow(geometry);

    m_gw->setTouchEventsEnabled(true);
    camera->setGraphicsContext(m_gw.get());

    const osg::GraphicsContext::Traits *traits = m_gw->getTraits();
    camera->setClearColor(osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
    camera->setViewport(0, 0, traits->width, traits->height);

    double aspect = static_cast<double>(traits->width) / static_cast<double>(traits->height);
    camera->setProjectionMatrixAsPerspective(30.0, aspect, 1.0, 1000.0);
    camera->setClearColor(
                osg::Vec4(
                    float(53/255.0),
                    float(81/255.0),
                    float(2/255.0),1)
                );
    GLenum buffer = (traits->doubleBuffer) ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);
    // Solve the problem that one point in geode does not show
    osg::CullStack::CullingMode cullingMode = camera->getCullingMode();
    cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
    camera->setCullingMode(cullingMode);
}

void QViewerWidget::paintEvent(QPaintEvent *)
{
    // refer:: https://github.com/USNavalResearchLaboratory/simdissdk/blob/8b214e46968ec89ea752fd2eb2db9458e65eff31/SDK/simQt/ViewWidget.cpp#L43
    osgQt::GLWidget * widget = m_gw->getGLWidget();
    if (widget && widget->windowHandle())
    {
        if (widget->windowHandle()->isExposed())
        {
            viewer.frame();
        }
    }
}
