#ifndef VISIONSYSTEM_H
#define VISIONSYSTEM_H
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <vector>
#include <exception>

#include <QDebug>

#include "windows.h"
#include "VisionBooster.h"

//const int CAPTURE_TIME_OUT = 6000;
// Scan once
#define VST_SINGLE_SCAN			 
//#define VST_UnorderedPointClouds // Unordered points

class VisionSystem
{
public:
	VisionSystem(std::string installPath);

	~VisionSystem();

	void initVisionSystem();

	bool scanOnce(std::vector<VST3D_PT> & VSTPoints);

    void save2File(const std::vector<VST3D_PT> & VSTPoints, std::string filename);

	void disConnect();

	bool isConnected();

private:
	int VST3D_RESULT;

	std::string installPath;

	// Default Capture Method: 2
	const int captureMethod;

    void retryCon(const std::string errorStr, const int codeLine, const std::string codeFile);
};

#endif
