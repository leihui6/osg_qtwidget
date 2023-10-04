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
    , m_scene(new osg::Group)
{
    this->setAttribute(Qt::WA_NativeWindow, true);

    load_config("./data/config.json");

    initCamera(geometry);

    m_viewer.setSceneData(m_scene);
    m_viewer.addEventHandler(new osgViewer::StatsHandler);
    m_viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    m_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    osgQt::GraphicsWindowQt *gw = static_cast<osgQt::GraphicsWindowQt *>(m_viewer.getCamera()->getGraphicsContext());

    QGridLayout *layout = new QGridLayout;

    if (layout != Q_NULLPTR)
    {
        layout->addWidget(gw->getGLWidget());
        this->setLayout(layout);
    }
    m_viewer.home();
}

QViewerWidget::~QViewerWidget()
{

}

osg::Group *QViewerWidget::getScene()
{
    return m_scene.get();
}

osgViewer::Viewer *QViewerWidget::getViewer()
{
    return &m_viewer;
}

int QViewerWidget::clean()
{
    if (m_scene == nullptr)
        return 1;

    //m_scene->removeChildren(0, m_scene->getNumChildren());
    //m_node_map.clear();

    return 0;
}

size_t QViewerWidget::add_point_cloud(std::vector<point_3d> & point_cloud, const QString point_cloud_name)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    points_to_geometry_node(point_cloud, geometry, 1.0);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, int(point_cloud.size())));
    geode->addDrawable(geometry);

    update(point_cloud_name, geode);

    return point_cloud.size();
}

int QViewerWidget::activate_XYZ_axes(bool show_status)
{
    activate_point_cloud("axis_x", show_status);
    activate_point_cloud("axis_y", show_status);
    activate_point_cloud("axis_z", show_status);
    //qDebug() << "activate function";

    return 0;
}

QVector<QString> QViewerWidget::get_pointcloud_list()
{
    QVector<QString> pointcloud_list;

    for (auto & node: m_node_map) {
        pointcloud_list.push_back(node.first);
    }
    return pointcloud_list;
}

osg::Vec4f QViewerWidget::json_array2vec4(QJsonValue & value)
{
    float v4[4] = {};
    auto tmp = value.toArray().toVariantList();
    for(int i=0; i < tmp.length();++i)
    {
        v4[i] = float(tmp.at(i).toFloat());
    }
    return osg::Vec4f(v4[0],v4[1],v4[2],v4[3]);
}

int QViewerWidget::load_config(QString config_name)
{
    QFile loadFile(config_name);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonParseError parseError;
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData, &parseError));
    if (parseError.error != QJsonParseError::NoError)
    {
       qDebug() <<"error:" << parseError.errorString();
       return 1;
    }

    const QJsonObject & json= loadDoc.object();
    foreach(const QString& key, json.keys())
    {
        QJsonValue value = json.value(key);
        //qDebug() << "Key = " << key << ", Value = " << value.toArray();
        if (key == "default-background-color" && value.isArray())
        {
            m_bgc = json_array2vec4(value);
        }
        else if (key == "default-pointcloud-color" && value.isArray()){
            m_pcc= json_array2vec4(value);
        }
        else if (key == "default-pointcloud-size" && value.isDouble()) {
            m_pcs = value.toInt();
        }
    }
    return 0;
}

int QViewerWidget::show_XYZ_axes()
{
    float scale = 1.0;
    point_3d p0(0,0,0), p1(1*scale,0,0),p2(0,1*scale,0),p3(0,0,1*scale);
    std::vector<point_3d> line_segment_points{p0, p1, p0, p2, p0, p3};

    add_line_segment(p0, p1, "axis_x", 4);
    set_pointcloud_color("axis_x", osg::Vec4(255, 0, 0, 1));

    add_line_segment(p0, p2, "axis_y", 4);
    set_pointcloud_color("axis_y", osg::Vec4(0, 255, 0, 1));

    add_line_segment(p0, p3, "axis_z", 4);
    set_pointcloud_color("axis_z", osg::Vec4(0, 0, 255, 1));

    return 0;
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
    osg::Camera *camera = m_viewer.getCamera();

    m_gw = createGraphicsWindow(geometry);

    m_gw->setTouchEventsEnabled(true);
    camera->setGraphicsContext(m_gw.get());

    const osg::GraphicsContext::Traits *traits = m_gw->getTraits();
    camera->setClearColor(osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
    camera->setViewport(0, 0, traits->width, traits->height);

    double aspect = static_cast<double>(traits->width) / static_cast<double>(traits->height);
    camera->setProjectionMatrixAsPerspective(30.0, aspect, 1.0, 500.0);
    camera->setClearColor(
                osg::Vec4(
                    float(m_bgc[0]/255),
                    float(m_bgc[1]/255),
                    float(m_bgc[2]/255),
                    m_bgc[3])
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
            m_viewer.frame();
        }
    }
}

int QViewerWidget::set_pointcloud_color(const QString &point_cloud_name, osg::Vec4 point_color)
{
    if (m_node_map.find(point_cloud_name) == m_node_map.end()) return 1;

    osg::ref_ptr<osg::Node> node = m_node_map[point_cloud_name];
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    //color.set( color.r()/255.0f,color.g()/255.0f,color.b()/255.0f,color.w());

    colors->push_back(point_color);

    node->asGeode()->getChild(0)->asGeometry()->setColorArray(colors.get());
    node->asGeode()->getChild(0)->asGeometry()->setColorBinding(osg::Geometry::BIND_OVERALL);

    return 0;
}

int QViewerWidget::set_pointcloud_size(const QString &point_cloud_name, int point_size)
{
    auto it = m_node_map.find(point_cloud_name);
    if (it == m_node_map.end())
        return 1;

    osg::StateSet* stateSet = it->second->asGeode()->getChild(0)->asGeometry()->getOrCreateStateSet();
    stateSet->setAttribute(new osg::Point(point_size));
}

int QViewerWidget::activate_point_cloud(const QString &point_cloud_name, bool status)
{
    osg::ref_ptr<osg::Node> scene = m_viewer.getSceneData();
    if(!scene) return false;

    if(m_node_map.find(point_cloud_name) != m_node_map.end())
    {
        m_node_map[point_cloud_name]->setNodeMask(status);
    }
    return true;
}

int QViewerWidget::points_to_osg_structure(std::vector<point_3d>& points, osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<osg::Vec4Array> colors, float w)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        coords->push_back(osg::Vec3(points[i].x, points[i].y, points[i].z));
        colors->push_back(osg::Vec4(points[i].r, points[i].g, points[i].b, w));
    }

    return 0;
}

int QViewerWidget::points_to_geometry_node(std::vector<point_3d> &points, osg::ref_ptr<osg::Geometry> geometry, float w)
{
    osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    points_to_osg_structure(points, coords, colors, w);

    // vertex
    geometry->setVertexArray(coords.get());

    // color
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    return 0;
}

int QViewerWidget::add_line_segment(const point_3d &beg_p, const point_3d &end_p, const QString &line_name, float line_width)
{
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    std::vector<point_3d> line_segment{beg_p,end_p};
    points_to_geometry_node(line_segment, geometry, 1.0f);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, int(line_segment.size())));

    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(line_width);
    geometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    node->addChild(geometry);

    update(line_name,node);

    return 0;
}

int QViewerWidget::update(const QString &point_cloud_name, osg::ref_ptr<osg::Node> node)
{
    osg::ref_ptr<osg::Node> scene = m_viewer.getSceneData();

    if(!scene) return 1;

    // add new one
    if(m_node_map.find(point_cloud_name) == m_node_map.end())
    {
       scene->asGroup()->addChild(node);
       m_node_map[point_cloud_name] = node;
    }
    // replace the current one
    else
    {
       scene->asGroup()->replaceChild(m_node_map[point_cloud_name].get(),node);
       m_node_map[point_cloud_name] = node;
       qDebug() << "Replace";
    }

   set_pointcloud_color(point_cloud_name, m_pcc);
   set_pointcloud_size(point_cloud_name, m_pcs);

   qDebug()
           <<"Children Number:"<< scene->asGroup()->getNumChildren()
          << " scene:" <<scene
          <<"m_node_map size:"<< m_node_map.size();

   m_viewer.home();

   return 0;
}
