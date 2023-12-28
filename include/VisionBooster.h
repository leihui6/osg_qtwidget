//=============================================================================
//
// Vision Sensitive Technology Co., Ltd, Tianjin ，China
//
// VisionBooster Scanner is product of Vision Sensitive Technology(VST), China
// VisenTOP® Studio  is trademark of Vision Sensitive Technology(VST), China
// www.visentech.com     Software Version 6.2.0.1  by Prof.jma
// Copyright © 2006 - 2021    All rights reserved
//
// Redistribution and use in source and binary forms,
// with or without modification, are permitted provided that
// the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// - Neither the name of VisenTOP nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//=============================================================================

#pragma once

#ifdef DLL_VST3D_EXPORTS
#define DLL_VST3D_API __declspec(dllexport)
#else
#define DLL_VST3D_API __declspec(dllimport)
#endif

//*****************************************************************************
/// @VisionBoosterLib
/// @VisionBooster Low Level Library (VisionBooster.lib and VisionBooster.dll)
/// Provides a low level C interface to '3D Scanning' functions.
//*****************************************************************************

//-------------------------------------------------------------------------
//  Scanner State and Device related errors.
//-------------------------------------------------------------------------
#define VST3D_RESULT_OK 0
#define VST3D_RESULT_ERROR -1
#define VST3D_RESULT_CAMERA_ERROR -2
#define VST3D_RESULT_SENSOR_ERROR -3

//=============================================================================
// Basic 3D Point Format
//=============================================================================
struct VST3D_PT
{ ///<  x y z coordinate
	float x;
	float y;
	float z;
	///<  nx ny nz normal vector  [0-1]
	float nx;
	float ny;
	float nz;
	///<  color value for each point,8bit grayscale format [0-255]
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
};

#ifdef __cplusplus
extern "C"
{
#endif
	/// Initialize VisionBooster Low Level Library.
	/// @param[in] VisionTOP studio.exe installation path. Default value is ok.
	/// @param[in] Show or close software window. Default value is ok.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Init(const char *exe_PathName = "C:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe", bool bShowWnd = true); // Starting VisenTOP Studio scanner software...

	/// Release VisionBooster Low Level Library.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Exit(); // Shut down 3DBooster Scanner (release all resources and close process handle)

	/// Check if Scanner is connected and setup related parameters for single snap mode.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Connect(); // Connect to the Scanner and setup the related parameters...

	/// Start a new asynchronous 3D data capture for all 3 modes. The function returns immediately.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Scan(); // Scanning...

	// Set current 3D Camera ID in Multi cameras scaning mode
	DLL_VST3D_API int VST3D_SetCameraID(int cId = 0);

	/// Reset 3D scanner only for single snap modes. The function returns immediately.
	/// @param[in] VisionTOP studio.exe installation path. Default value is ok.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Reset(const char *exe_PathName = "C:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe"); // Scanning...

	/// Get 3D Points data.
	/// @param[out] total number to each scan.
	/// @param[out] data Destination buffer to each scan.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetPointClouds(int &nPts, VST3D_PT **pts); // Push all 3D Points in each scanning into the DataBuffer and get the num of 3D Points

	/// Get a point data by index.
	/// @param[in]  index to some point in each scan.
	/// @param[out] data Destination buffer to a point.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetEachPointByIndex(int nIndex, VST3D_PT **pt); // Get the point info by index from [0, nPts-1], Output points coordinate,normal and color value

	/// Get total num of 3D Points data.
	/// @param[out] total number to each scan.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetNumPoints(int &nPts); // Get parameter: total num of 3D Points

	/// Start a new asynchronous 3D CircleMarker Capture or cancel this proc for all scan modes. The function returns immediately.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_SetCircleMarkSnap(); // Capture circle marks position or cancel it use software when scanning...

	/// Get total num of 3D Markers.
	/// @param[out] total number to each scan.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetNumMarkers(int &nPts); // Get parameter: total num of 3D Circle Markers

	/// Get 3D Circle markers data.
	/// @param[out] total number to each scan.
	/// @param[out] data Destination buffer to each scan.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetMarkersData(int &nPts, VST3D_PT **pts); // Push all 3D Points in each scanning into the DataBuffer and get the num of 3D markers

	/// Get the 2D image data .
	/// @param[out] data Destination buffer to 2D image.
	/// @param[out] width to an int that gets the width of the image.
	/// @param[out] height to an int that gets the height of the image.
	/// @param[in]  left image or right image.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetImage(unsigned char **pImag, int &width, int &height, bool bLeft = true); // Get Camera image buffer and size in scanning. where bLeft == true(defalut) ? Left :Right

	/// Get format of the 2D image .
	/// @param[out] width to an int that gets the width of the image.
	/// @param[out] height to an int that gets the height of the image.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetImageSize(int &width, int &height); // Get Camera image size in scanning...

	/// Generate relation map between 2D image and 3D cloud points .
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GeneratePointMap(); // Get PointMap memory in image format (width and height) in scanning...

	/// Get 3D point data for specific 2D pixel after VST3D_GeneratePointMap() function .
	/// @param[in]  row index for specific 2D image pixel in 2D image.
	/// @param[in]  row index for specific 2D image pixel in 2D image.
	/// @param[in]	width to an int in 2D image.
	/// @param[in]	height to an int in 2D image.
	/// @param[out] data Destination buffer to a point related with specific 2D image pixel.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetEachPointByPointMap(int nrow, int ncol, int width, int height, VST3D_PT **pt); // Get Each Point info by image index (i-nrow,j-ncol) in scanning...

	/// Check if Scanner is connected and setup related parameters for single snap and align mode and automaticlly stitch points with last scanning data.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Connect_ALIGN(); // Connect to the Scanner and setup the auto align  related parameters...

	/// Get plane parameters for background cancellation.
	/// @param[out] data buffer for background plane. plane order is [nx,ny,nz,x0,y0,z0] that includes plane normals and center coordinates, which size is 6*float.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_DelBackground(float **plane = nullptr); // delete all back points after setting back plane (plane order is [nx,ny,nz,x0,y0,z0], plane normals and center 6*float)

	/// Remove all overlay 3D cloud points after scanning data in single snap and align mode.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_RemoveOverLayPoints(); // Execute and remove all overlay points

	/// Check each point state (if reserved) by group id and point in single snap and align mode.
	/// @param[out] if reserved is true, or false for each point.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetTotalPointState(bool **state); // Get each point state (if reserved) by group id and point id..

	/// Clear and remove all group and 3D points in single snap and align mode.
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_ClearAllData(); // clear all data after a cycle Align Scanning

	/// Get OdoMeter [R T] in align mode
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_GetOdoMeterRotationTrans(double **para = nullptr); // clear all data after a cycle Align Scanning
	DLL_VST3D_API int VST3D_GetOdoMeterEularTrans(double **para = nullptr);	   // clear all data after a cycle Align Scanning

	/// Reset 3D scanner only for single snap and align mode. The function returns immediately.
	/// @param[in] VisionTOP studio.exe installation path. Default value is ok.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Reset_ALIGN(const char *exe_PathName = "C:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe");

	/// Check if Scanner is connected and setup related parameters for array snap mode (multiple scanner).
	/// @param[in]
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Connect_ARRAY(); // Connect to the Scanner and setup the ArrayScan parameters...

	/// Reset 3D scanner only for array snap mode. The function returns immediately.
	/// @param[in] VisionTOP studio.exe installation path. Default value is ok.
	/// @return VST3D_RESULT_OK, VST3D_RESULT_ERROR
	DLL_VST3D_API int VST3D_Reset_ARRAY(const char *exe_PathName = "C:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe");

	// Get 3D depth map
	DLL_VST3D_API int VST3D_Output_PointMap(float **_xMap, float **_yMap, float **_zMap, int &width, int &height, bool bSave = false);

	// Get Calibration Parameters
	DLL_VST3D_API int VST3D_Output_CalPara(double **calpara);

#ifdef __cplusplus
} // extern "C"
#endif

///////////////////////////////////////////////////////////////////////////////
// stdcall wrappers (for languages which require stdcall instead of cdecl)

#ifndef WINAPI
#define WINAPI _stdcall
#endif
extern "C"
{
	DLL_VST3D_API int _stdcall VST3Ds_Init(const char *exe_PathName, bool bShowWnd = true);						   // Starting VisenTOP Studio scanner software...
	DLL_VST3D_API int _stdcall VST3Ds_Exit();																	   // Shut down 3DBooster Scanner (release all resources and close process handle)
	DLL_VST3D_API int _stdcall VST3Ds_Connect();																   // Connect to the Scanner and setup the related parameters...
	DLL_VST3D_API int _stdcall VST3Ds_Connect_ALIGN();															   // Connect to the Scanner and setup the auto align  related parameters...
	DLL_VST3D_API int _stdcall VST3Ds_DelBackground();															   // delete all back points after setting back plane
	DLL_VST3D_API int _stdcall VST3Ds_Scan();																	   // Scanning...
	DLL_VST3D_API int _stdcall VST3Ds_GetPointClouds(int &nPts, VST3D_PT **pts);								   // Push all 3D Points in each scanning into the DataBuffer and get the num of 3D Points
	DLL_VST3D_API int _stdcall VST3Ds_GetEachPointByIndex(int nIndex, VST3D_PT **pt);							   // Get the point info by index from [0, nPts-1], Output points coordinate,normal and color value
	DLL_VST3D_API int _stdcall VST3Ds_GetNumPoints(int &nPts);													   // Get parameter: total num of 3D Points
	DLL_VST3D_API int _stdcall VST3Ds_GetImage(unsigned char **pImag, int &width, int &height, bool bLeft = true); // Get Camera image buffer and size in scanning. where bLeft == true(defalut) ? Left :Right
	DLL_VST3D_API int _stdcall VST3Ds_GetImageSize(int &width, int &height);									   // Get Camera image size in scanning...
}
