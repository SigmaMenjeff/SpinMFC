#pragma once
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <iostream>
#include <sstream>
#include <atltime.h>
#include <SpinVideo.h>
#include <chrono>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;
using namespace std;

//Spinnaker系统单例
extern SystemPtr CameraSystem;
//相机列表
extern CameraList camList;
//相机指针
extern CameraPtr pCam;

CString CameraConnect(SystemPtr& system, int& numCameras);
CString CameraDisconnect(CameraPtr& pCam);
CString ApplicationExit(SystemPtr& system, CameraPtr& pCam, CameraList& camList);

CString GetSDKVersion(SystemPtr system);
CString GetDevieInfo(CameraPtr pCam, int length);

CString CameraStart(CameraPtr& pCam, CString ImageSize);
CString CameraStop(CameraPtr& pCam);

CString SetCameraAcquisitionMode(INodeMap& nodeMap);
CString SetCameraPixelFormat(INodeMap& nodeMap);
CString SetImageSize(INodeMap& nodeMap, CString ImageSize);
CString SetCameraBufferMode(CameraPtr& pCam);

CString SetCameraExposure(CameraPtr& pCam, double TargetTime);
CString SetCameraGain(CameraPtr& pCam, double TargetGain);

CString ResetCameraExposure(CameraPtr& pCam);
CString ResetCameraGain(CameraPtr& pCam);

CString CameraSavePicture(CameraPtr& pCam, CString PicturePath, CString PixelFormat, CString ImageFormat);
CString CameraSaveVideo(CameraPtr& pCam, SpinVideo& VideoWriter, CString VideoPath, CString VideoFormat, CString ImageSize);
