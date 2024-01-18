#include "VisionSystem.h"

VisionSystem::VisionSystem(std::string installPath)
    :VST3D_RESULT(VST3D_RESULT_ERROR), captureMethod(2)
{
	this->installPath = installPath;

    initVisionSystem();

    if (!isConnected())
        throw std::bad_exception();
}

VisionSystem::~VisionSystem()
{

}

void VisionSystem::initVisionSystem()
{
    // default path, start without software's window
    VST3D_RESULT = VST3D_Init(this->installPath.data(), true);
    if (VST3D_RESULT != VST3D_RESULT_OK)
    {
        qDebug() << "Could not Start Scanner software";
        VST3D_Exit();
        throw std::runtime_error("Could not Start Scanner software");
    }
    // Self-check and connect
    VST3D_RESULT = VST3D_Connect();
    if (VST3D_RESULT != VST3D_RESULT_OK)
    {
        qDebug() << "Could not connect to Scanner.\n";
        VST3D_Exit();
        throw std::runtime_error("Could not Start Scanner software");
    }
    // qDebug() << "VST3D_RESULT" << VST3D_RESULT;

    // check camera by ID can only run after initialization normally
    VST3D_SetCameraID(0);
}

bool VisionSystem::scanOnce(std::vector<VST3D_PT> & VSTPoints)
{
	int result = VST3D_Scan(); //  Start scanning once
	if (result != VST3D_RESULT_OK)
	{
		retryCon("VST3D_Scan", __LINE__, __FILE__);
		return false;
	}

	int totalNum = 0;
	result = VST3D_GetNumPoints(totalNum); // obtain number of point at once
	if (result != VST3D_RESULT_OK)
	{
		retryCon("VST3D_GetNumPoints", __LINE__, __FILE__);
		return false;
	}

	VST3D_PT *pPointClouds = nullptr;
	result = VST3D_GetPointClouds(totalNum, &pPointClouds); 

	// Method 1
	if (captureMethod == 1)
	{
		VST3D_PT * myPts = new VST3D_PT[totalNum];
		if (myPts)
		{
			memcpy((void *)myPts, (void *)(pPointClouds), totalNum * sizeof(VST3D_PT));
			VST3D_PT tmp;
			for (int i = 0; i < totalNum; i++)
			{
				VST3D_PT &pt = myPts[i];
				//float x, y, z, nx, ny, nz;
				//float cr, cg, cb;
                tmp.x = pt.x;
                tmp.y = pt.y;
                tmp.z = pt.z;
				tmp.nx = pt.nx;
				tmp.ny = pt.ny;
				tmp.nz = pt.nz;
				tmp.cr = (float)pt.cr / 255;
				tmp.cg = (float)pt.cg / 255;
				tmp.cb = (float)pt.cb / 255;
				//file << x << " " << y << " " << z << " " << cr << " " << cg << " " << cb << " " << nx << " " << ny << " " << nz << endl;
				VSTPoints.push_back(tmp);
			}
			delete[] myPts;
		}
	}
	else if (captureMethod == 2)
	{
		// Method 2
		VST3D_PT tmp;
		// Iterate through the copy of all the acquired point clouds in disordered order 
		// to get the point cloud coordinates and other information, by indexing the way [0 1 2 ....]
		for (int i = 0; i < totalNum; i++)
		{
			VST3D_PT *pt = nullptr;
			VST3D_GetEachPointByIndex(i, &pt);
			//float x, y, z, nx, ny, nz;
			//float cr, cg, cb;
            tmp.x = pt->x;
            tmp.y = pt->y;
            tmp.z = pt->z;
			tmp.nx = pt->nx;
			tmp.ny = pt->ny;
			tmp.nz = pt->nz;
			tmp.cr = (float)pt->cr / 255;
			tmp.cg = (float)pt->cg / 255;
			tmp.cb = (float)pt->cb / 255;
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
        of << p.x << " " << p.y << " " << p.z << "\n";
           //<< p.nx << " " << p.ny << " " << p.nz << " "
           //<< p.cr << " " << p.cg << " " << p.cb << "\n";
    }
    of.close();
}

void VisionSystem::transformPointcloud(const std::vector<VST3D_PT>& VSTPoints, Eigen::Matrix4f & m,
                                       std::vector<VST3D_PT>& new_VSTPoints, const cropSize_t & cropSize, bool ifCrop)
{
    std::vector<VST3D_PT> optVSTPoints;
    if (ifCrop)
    {
        cropPointCloud(cropSize, VSTPoints, optVSTPoints);
    }
    else
    {
        optVSTPoints = VSTPoints;
    }

    std::vector<Eigen::Vector4f> pointsEigen;
    pointsEigen.resize(optVSTPoints.size());

    for (auto &p : optVSTPoints)
    {
        pointsEigen.push_back(Eigen::Vector4f{p.x, p.y, p.z, 1});
    }

    new_VSTPoints.resize(optVSTPoints.size());
    VST3D_PT tmp;
    for (auto &p : pointsEigen)
    {
        tmp.x = (m * p)[0];
        tmp.y = (m * p)[1];
        tmp.z = (m * p)[2];
        new_VSTPoints.push_back(tmp);
    }
}

void VisionSystem::cropPointCloud(const cropSize_t & cropSize, const std::vector<VST3D_PT>& VSTPoints, std::vector<VST3D_PT>& new_VSTPoints)
{
	new_VSTPoints.clear();

	for (auto &p: VSTPoints)
	{
		if (p.x > cropSize.min_x && p.x < cropSize.max_x &&
			p.y > cropSize.min_y && p.y < cropSize.max_y &&
			p.z > cropSize.min_z && p.z < cropSize.max_z)
		{
			new_VSTPoints.push_back(p);
		}
	}
}

const cropSize_t VisionSystem::calBoundingBox(const std::vector<VST3D_PT>& VSTPoints)
{
	cropSize_t tmpCropSize;
	for (auto &p : VSTPoints)
	{
		if (p.x > tmpCropSize.max_x) tmpCropSize.max_x = p.x;
		if (p.y > tmpCropSize.max_y) tmpCropSize.max_y = p.y;
		if (p.z > tmpCropSize.max_z) tmpCropSize.max_z = p.z;

		if (p.x < tmpCropSize.min_x) tmpCropSize.min_x = p.x;
		if (p.y < tmpCropSize.min_y) tmpCropSize.min_y = p.y;
		if (p.z < tmpCropSize.min_z) tmpCropSize.min_z = p.z;
	}

	return tmpCropSize;
}

const cropSize_t VisionSystem::calBoundingBox(const std::string pointsFilename)
{
	std::vector<VST3D_PT> VSTPoints;
	loadPointsFromFile(pointsFilename, VSTPoints);

	// bounding box size
	cropSize_t tmpCropSize;
	for (auto &p : VSTPoints)
	{
		if (p.x > tmpCropSize.max_x) tmpCropSize.max_x = p.x;
		if (p.y > tmpCropSize.max_y) tmpCropSize.max_y = p.y;
		if (p.z > tmpCropSize.max_z) tmpCropSize.max_z = p.z;

		if (p.x < tmpCropSize.min_x) tmpCropSize.min_x = p.x;
		if (p.y < tmpCropSize.min_y) tmpCropSize.min_y = p.y;
		if (p.z < tmpCropSize.min_z) tmpCropSize.min_z = p.z;
	}

	return tmpCropSize;
}

void VisionSystem::fittingCylidner(const std::string pointsFilename, Eigen::Vector3f & point, Eigen::Vector3f & axis)
{
	//std::vector<VST3D_PT> VSTPoints;
	//loadPointsFromFile(pointsFilename, VSTPoints);
	Py_Initialize();
	if (!Py_IsInitialized())
	{
        PyErr_Print();
		throw std::runtime_error("Python Initialization Failed");
		return ;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
    PyRun_SimpleString("sys.path.append('./data')");
    qDebug() << QString::fromStdString(std::filesystem::current_path().string());
	PyObject * module = PyImport_ImportModule("CylinderFitting");
	if (!module)
	{
		PyErr_Print();
		throw std::runtime_error("Module not found");
		return ;
	}

	PyObject * func = PyObject_GetAttrString(module, "CylinderFitting");
	if (!func || !PyCallable_Check(func))
	{
        PyErr_Print();
		throw std::runtime_error("Func in Module not found");
		return ;
	}
	PyObject * args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, Py_BuildValue("s", pointsFilename.data()));

	PyObject * res = PyObject_CallObject(func, args);

	point = Eigen::Vector3f::Zero();
	axis = Eigen::Vector3f::Zero();
	for (int i = 0; i < 6; ++i)
	{
		PyObject *pRet = PyList_GetItem(res, i);
		float tmp = 0;
		PyArg_Parse(pRet, "f", &tmp);

		if (i < 3) point[i] = tmp;
		else if (i >= 3) axis[i-3] = tmp;
	}
	
	Py_Finalize();
}

Eigen::Matrix4f VisionSystem::generateRMatrixAlongAxis(Eigen::Vector3f point, Eigen::Vector3f axis, float angle)
{
	Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();
	transform <<
		1, 0, 0, -point[0],
		0, 1, 0, -point[1],
		0, 0, 1, -point[2],
		0, 0, 0, 1;

	auto t = Eigen::AngleAxis<float>(ToRadians(angle), axis.normalized());
	Eigen::Matrix4f R4X4 = Eigen::Matrix4f::Identity();
	R4X4.block<3, 3>(0, 0) = t.matrix();

	return transform.inverse() * R4X4 * transform;
}

void VisionSystem::retryCon(const std::string errorStr, const int codeLine, const std::string codeFile)
{
    printf("Error(%s) at line number %d in file %s\n", errorStr.data(), codeLine, codeFile.data());

	int nreset = 0;
	while (true)
	{
		// Delay of a few seconds before each reconnection, camera, raster hardware driver loading takes time
		Sleep(6000);

		int result = VST3D_Reset(this->installPath.data()); 

		if (result != VST3D_RESULT_OK)
		{
			printf("Check cables connected to Scanner.\n"); 
		}
		else
		{
			break;
		}
		// Make 5 reconnect attempts, if the scanner does not connect properly, exit the software system.
		if (nreset++ > 5) 
		{
			VST3D_Exit();
		}
	}
}

void VisionSystem::disConnect()
{
	VST3D_Exit();
}

float VisionSystem::ToRadians(float deg)
{
	return (float)(deg * M_PI / 180.0f);
}

void VisionSystem::loadPointsFromFile(const std::string pointsFilename, std::vector<VST3D_PT>& VSTPoints)
{
	// loading points from the specific file
	std::ifstream ifile;
	try
	{
		std::ifstream ifile(pointsFilename, std::ios::in);
	}
	catch (const std::exception&)
	{
		throw std::runtime_error("Could not open file");
		return;
	}

	std::string line;
	VST3D_PT VSTpoint;
	while (std::getline(ifile, line))
	{
		std::stringstream s(line);
		float tmp = 0.0, value[3] = { 0 };
		int i = 0;
		while (s >> tmp)
		{
			value[i] = tmp;
			i++;
		}
		VSTpoint.x = value[0];
		VSTpoint.y = value[1];
		VSTpoint.z = value[2];
		VSTPoints.push_back(VSTpoint);
	}
}

bool VisionSystem::isConnected()
{
    if (VST3D_RESULT != VST3D_RESULT_OK)
        return false;
    else
        return true;
}
