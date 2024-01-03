#ifndef QVIEWER_WIDGET_H
#define QVIEWER_WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QWindow>
#include <QVector>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <osgViewer/Viewer>
#include <osgQt/GraphicsWindowQt>

#include "cloud_geometry.h"

#define KEY_BGC "default-background-color"
#define KEY_PCC "default-pointcloud-color"
#define KEY_PCS "default-pointcloud-size"
#define AXIS_X "axis_x"
#define AXIS_Y "axis_y"
#define AXIS_Z "axis_z"

class QViewerWidget : public QWidget
{
public:
    QViewerWidget(const QRect &geometry);

    int load_config(QString config_name);

    virtual ~QViewerWidget();

    osg::Group *getScene();

    osgViewer::Viewer *getViewer();

    int clean();

    size_t add_point_cloud(std::vector<point_3d> & point_cloud, const QString point_cloud_name);

    int add_line_segment(const point_3d &beg_p, const point_3d &end_p, const QString & line_name, float line_width);

    size_t del_point_cloud(const QString point_cloud_name);

    int show_XYZ_axes();

    int activate_XYZ_axes(bool show_status);

    // default value for pointcloud
    pointcloud_params m_pc_params;

    QVector<QString> get_pointcloud_list();

    int set_pointcloud_color(const QString & point_cloud_name, osg::Vec4 point_color);
    int set_pointcloud_size(const QString & point_cloud_name, int point_size);

    int save_config();
private:

    QString m_config_name;

    std::map<QString, osg::ref_ptr<osg::Node>> m_node_map;

    osg::ref_ptr<osg::Group> m_scene;

    osgViewer::Viewer m_viewer;

    osg::ref_ptr<osgQt::GraphicsWindowQt> m_gw;

private:

    osg::Vec4f json_array2vec4(QJsonValue & value);

    QJsonValue json_vec42array(osg::Vec4f & v);

    osgQt::GraphicsWindowQt *createGraphicsWindow(const QRect &geometry);

    void initCamera(const QRect &geometry);

    void paintEvent(QPaintEvent *);

// Basic functions for OSG
private:

    int activate_point_cloud(const QString &point_cloud_name, bool status);

    int points_to_osg_structure(std::vector<point_3d>& points, osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<osg::Vec4Array> colors, float w);

    int points_to_geometry_node(std::vector<point_3d> & points, osg::ref_ptr<osg::Geometry> geometry, float w = 1.0);

    int update(const QString & point_cloud_name, osg::ref_ptr<osg::Node> node);

};

#endif // QVIEWER_WIDGET_H
