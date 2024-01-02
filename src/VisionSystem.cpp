#include "VisionSystem.h"

VisionSystem::VisionSystem(std::string installPath)
	: VST3D_RESULT(VST3D_RESULT_ERROR), captureMethod(2)
{
	this->installPath = installPath;

	initVisionSystem();

	if (!isConnected())
		throw std::bad_exception();
}

VisionSystem::~VisionSystem()
{
	VST3D_Exit();
}

void VisionSystem::initVisionSystem()
{
	// default path, start without software's window
	VST3D_RESULT = VST3D_Init(this->installPath.data(), true);
	if (VST3D_RESULT != VST3D_RESULT_OK)
	{
		qDebug() << "Could not Start Scanner software";
		VST3D_Exit();
		return;
	}
	// Self-check and connect
	VST3D_RESULT = VST3D_Connect();
	if (VST3D_RESULT != VST3D_RESULT_OK)
	{
		qDebug() << "Could not connect to Scanner.\n";
		VST3D_Exit();
		return;
	}
	// qDebug() << "VST3D_RESULT" << VST3D_RESULT;

	// check camera by ID can only run after initialization normally
	VST3D_SetCameraID(0);

	// qDebug() << "test";
}

bool VisionSystem::scanOnce(std::vector<VST3D_PT> &VSTPoints)
{
	VST3D_RESULT = VST3D_Scan(); //  Start scanning once
	if (VST3D_RESULT != VST3D_RESULT_OK)
	{
		retryCon("VST3D_Scan", __LINE__, __FILE__);
		return false;
	}

	int totalNum = 0;
	VST3D_RESULT = VST3D_GetNumPoints(totalNum); // obtain number of point at once
	if (VST3D_RESULT != VST3D_RESULT_OK)
	{
		retryCon("VST3D_GetNumPoints", __LINE__, __FILE__);
		return false;
	}

	VST3D_PT *pPointClouds = nullptr;
	VST3D_RESULT = VST3D_GetPointClouds(totalNum, &pPointClouds); // 得到当前单次扫描总点数和乱序的点云

	// Method 1
	if (captureMethod == 1)
	{
		VST3D_PT *myPts = new VST3D_PT[totalNum];
		if (myPts)
		{
			memcpy((void *)myPts, (void *)(pPointClouds), totalNum * sizeof(VST3D_PT));
			VST3D_PT tmp;
			for (int i = 0; i < totalNum; i++) // 遍历拷贝的采集到的所有乱序点云，得到点云坐标等信息，通过索引方式[0 1 2 ....]
			{
				VST3D_PT &pt = myPts[i];
				// float x, y, z, nx, ny, nz;
				// float cr, cg, cb;
				tmp.x = pt.x;
				tmp.y = pt.y;
				tmp.z = pt.z;
				tmp.nx = pt.nx;
				tmp.ny = pt.ny;
				tmp.nz = pt.nz;
				tmp.cr = (unsigned char)(pt.cr / 255);
				tmp.cg = (unsigned char)(pt.cg / 255);
				tmp.cb = (unsigned char)(pt.cb / 255);
				// file << x << " " << y << " " << z << " " << cr << " " << cg << " " << cb << " " << nx << " " << ny << " " << nz << endl;
				VSTPoints.push_back(tmp);
			}
			delete[] myPts;
		}
	}
	else if (captureMethod == 2)
	{
		// Method 2
		VST3D_PT tmp;
		for (int i = 0; i < totalNum; i++) // 遍历单次采集到的所有乱序点云，得到点云坐标等信息，通过索引方式[0 1 2 ....]
		{
			VST3D_PT *pt = nullptr;
			VST3D_GetEachPointByIndex(i, &pt);
			// float x, y, z, nx, ny, nz;
			// float cr, cg, cb;
			tmp.x = pt->x;
			tmp.y = pt->y;
			tmp.z = pt->z;
			tmp.nx = pt->nx;
			tmp.ny = pt->ny;
			tmp.nz = pt->nz;
			tmp.cr = (unsigned char)(pt->cr / 255);
			tmp.cg = (unsigned char)(pt->cg / 255);
			tmp.cb = (unsigned char)(pt->cb / 255);
			VSTPoints.push_back(tmp);
		}
	}
	return true;
}

void VisionSystem::save2File(const std::vector<VST3D_PT> &VSTPoints, std::string filename)
{
	std::ofstream of;
	of.open(filename);
	for (auto &p : VSTPoints)
	{
		of << p.x << " " << p.y << " " << p.z << " "
		   << p.nx << " " << p.ny << " " << p.nz << " "
		   << p.cr << " " << p.cg << " " << p.cb << "\n";
	}
	of.close();
}

void VisionSystem::retryCon(const std::string errorStr, const int codeLine, const std::string codeFile)
{
	printf("Error(%s) at line number %d in file %s\n", errorStr.data(), codeLine, codeFile.data());

	int nreset = 0;
	while (true)
	{
		// 每次重新连接前延时几秒，相机，光栅硬件驱动加载需要时间
        Sleep(6000);

		// 单机扫描情况
		VST3D_RESULT = VST3D_Reset(this->installPath.data()); // 单机扫描重新连接扫描仪系统，重启

		if (VST3D_RESULT != VST3D_RESULT_OK)
		{
			printf("Check cables connected to Scanner.\n"); // 重新连接失败，继续尝试
		}
		else
		{
			// 重新连接成功，重新扫描
			break;
		}
		if (nreset++ > 5) // 做5次重新连接尝试，如果不能正常连接扫描仪，退出软件系统
		{
			VST3D_Exit();
			// return result;
		}
	}
}

void VisionSystem::disConnect()
{
	VST3D_Exit();
}

bool VisionSystem::isConnected()
{
	if (VST3D_RESULT != VST3D_RESULT_OK)
		return false;
	else
		return true;
}
