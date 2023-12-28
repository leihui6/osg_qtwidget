#ifndef CLOUD_GEOMETRY_H
#define CLOUD_GEOMETRY_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <QString>
#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Point>
#include <osg/ShapeDrawable>

#include "VisionBooster.h"

struct pointcloud_params
{
    osg::Vec4f background_color;
    osg::Vec4f pointcloud_color;
    int pointcloud_size;

    pointcloud_params():background_color(0,0,0,1), pointcloud_color(0,0,0,1), pointcloud_size(1) {}

    pointcloud_params & operator = (const pointcloud_params & p)
    {
        if (this != &p)
        {
            this->background_color = p.background_color;
            this->pointcloud_color = p.pointcloud_color;
            this->pointcloud_size = p.pointcloud_size;
        }
        return *this;
    }

    pointcloud_params(const pointcloud_params & p)
    {
        this->background_color = p.background_color;
        this->pointcloud_color = p.pointcloud_color;
        this->pointcloud_size = p.pointcloud_size;
    }
};

struct point_3d
{
    point_3d(const point_3d & p);

    point_3d();

    point_3d(float x, float y, float z);

    void set_xyz(float x, float y, float z);

    void set_nxyz(float nx, float ny, float nz);

    void set_rgb(float r, float g, float b);

    friend std::ostream & operator << (std::ostream & os, const point_3d & p);

    point_3d & operator = (const point_3d & p);
    point_3d operator + (const point_3d & p);
    point_3d operator / (const float num);

    float x, y, z;

    float nx, ny, nz;

    float r, g, b;
};

void points_to_osg_structure(std::vector<point_3d>& points, osg::ref_ptr<osg::Vec3Array> coords, osg::ref_ptr<osg::Vec4Array> colors, osg::ref_ptr<osg::Vec3Array> normals, float r, float g, float b, float w);

void points_to_geometry_node(std::vector<point_3d>& points, osg::ref_ptr<osg::Geometry> geometry, float r, float g, float b, float w);

void VST3D_to_points3D(const std::vector<VST3D_PT> & VST3DPoints, std::vector<point_3d> & point3DVec);

#endif  // CLOUD_GEOMETRY_H
