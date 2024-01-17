#ifndef VISIONSYSTEM_H
#define VISIONSYSTEM_H
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <vector>
#include <exception>

#include <filesystem>
#include <vector>
#include <limits>

#include <Eigen/Dense>

#include <QDebug>

#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

#include "windows.h"
#include "VisionBooster.h"

struct cropSize_t
{
	cropSize_t() 
		:min_x(DBL_MAX), min_y(DBL_MAX), min_z(DBL_MAX),
		max_x(DBL_MIN), max_y(DBL_MIN), max_z(DBL_MIN) {}
	double min_x, min_y, min_z;
	double max_x, max_y, max_z;
};

class VisionSystem
{
public:
	VisionSystem(std::string installPath);

	~VisionSystem();

	void initVisionSystem();

	bool scanOnce(std::vector<VST3D_PT> & VSTPoints);
	
    void save2File(const std::vector<VST3D_PT> & VSTPoints, std::string filename);

    void transformPointcloud(const std::vector<VST3D_PT> & VSTPoints, Eigen::Matrix4f & m, std::vector<VST3D_PT> & new_VSTPoints,
                             const cropSize_t & cropSize = cropSize_t(), bool ifCrop = false);

	void cropPointCloud(const cropSize_t & cropSize, const std::vector<VST3D_PT> & VSTPoints, std::vector<VST3D_PT> & new_VSTPoints);

	const cropSize_t calBoundingBox(const std::vector<VST3D_PT> & VSTPoints);
	const cropSize_t calBoundingBox(const std::string pointsFilename);

	void fittingCylidner(const std::string pointsFilename, Eigen::Vector3f &point, Eigen::Vector3f &axis);

	Eigen::Matrix4f generateRMatrixAlongAxis(Eigen::Vector3f point, Eigen::Vector3f axis, float angle);

	void retryCon(const std::string errorStr, const int codeLine, const std::string codeFile);

	void disConnect();

    bool isConnected();

private:
    int VST3D_RESULT;

	std::string installPath;

	// Default Capture Method: 2
	const int captureMethod;

	float ToRadians(float deg);

	void loadPointsFromFile(const std::string pointsFilename, std::vector<VST3D_PT>& VSTPoints);
};

#endif
