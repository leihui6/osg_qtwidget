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

#endif  // CLOUD_GEOMETRY_H
