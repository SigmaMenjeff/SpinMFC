#include "pch.h"
#include "SpinCamera.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//连接相机操作
CString CameraConnect(SystemPtr& system, int& numCameras)
{
	try
	{
		if (system == nullptr)
		{
			system = System::GetInstance();//返回一个智能指针用于管理所有相机和接口
		}
		camList = system->GetCameras();
		numCameras = camList.GetSize();
		if (numCameras == 0)//没有检测到相机
		{
			return TEXT("未检测到相机");
		}
		else
		{
			pCam = camList.GetByIndex(0);//只有一个相机
			pCam->Init();
			return TEXT("");//正常返回
		}
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//断开相机操作
CString CameraDisconnect(CameraPtr& pCam)
{
	try
	{
		if (pCam != nullptr && pCam->IsInitialized())//相机连接中
		{
			pCam->DeInit();//反初始化相机
			pCam = nullptr;
			return TEXT("");//正常返回
		}
		else return TEXT("相机未连接或相机未初始化");
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//应用程序退出
CString ApplicationExit(SystemPtr& system, CameraPtr& pCam, CameraList& camList)
{
	CString Error = TEXT("");

	try
	{
		//恢复默认曝光模式（自动曝光）
		//Error = ResetCameraExposure(pCam);
		if (!Error.IsEmpty()) return Error;
		//恢复默认增益模式（自动增益）
		//Error = ResetCameraGain(pCam);
		if (!Error.IsEmpty()) return Error;
		//断开相机
		//Error = CameraDisconnect(pCam);
		if (!Error.IsEmpty()) return Error;

		camList.Clear();
		if (system != nullptr)
		{
			system->ReleaseInstance();
			system = nullptr;
		}
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//获取SDK版本
CString GetSDKVersion(SystemPtr system)
{
	if (system == nullptr)
	{
		return TEXT("SDK版本：系统未初始化\n");
	}
	const LibraryVersion Version = system->GetLibraryVersion();

	CString SDKVersion;
	SDKVersion.Format(TEXT("SDK版本：%d.%d.%d.%d\r\n"), 
		Version.major, 
		Version.minor,
		Version.type,
		Version.build);

	return SDKVersion;
}
 
//获取设备信息
CString GetDevieInfo(CameraPtr pCam, int length)
{
	if (pCam == nullptr)
	{
		return TEXT("相机未连接\n");
	}

	CString DeviceInfo = TEXT("");

	try
	{
		//获取传输层（TL）设备节点映射表
		INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

		CCategoryPtr category = nodeMapTLDevice.GetNode("DeviceInformation");
		if (!IsReadable(category))
		{
			return TEXT("无法读取设备信息节点\n");
		}

		FeatureList_t features;
		category->GetFeatures(features);

		FeatureList_t::const_iterator it;
		for (it = features.begin(); it != features.end(); ++it)
		{
			CNodePtr pfeatureNode = *it;
			if (pfeatureNode == nullptr) continue;

			//gcstring转Cstring
			DeviceInfo += CA2W(pfeatureNode->GetName().c_str());
			DeviceInfo += TEXT(": ");

			CValuePtr pValue = (CValuePtr)(pfeatureNode);
			if (pValue != nullptr && IsReadable(pValue))
			{
				CString value = CA2W(pValue->ToString().c_str());
				if (value.GetLength() > length)
				{
					value = value.Left(length) + TEXT("...");
				}
				DeviceInfo += value;
			}
			else DeviceInfo += TEXT("该节点不可读");
			DeviceInfo += TEXT("\r\n");
		}
	}
	catch (Spinnaker::Exception& e)
	{
		DeviceInfo = TEXT("读取设备信息失败：");
		DeviceInfo += CA2W(e.what());
		DeviceInfo += TEXT("\r\n");
	}

	return DeviceInfo;
}

//开启相机
CString CameraStart(CameraPtr& pCam, CString ImageSize)
{
	if (pCam == nullptr || !pCam->IsInitialized()) 
		return TEXT("相机未连接或相机未初始化");

	CString Error = TEXT("");
	try
	{
		//相机参数设置预处理
		INodeMap& nodeMap = pCam->GetNodeMap();
		//设置采集模式为连续模式
		Error = SetCameraAcquisitionMode(nodeMap);
		if (!Error.IsEmpty()) return Error;
		//设置图像显示和保存大小
		Error = SetImageSize(nodeMap, ImageSize);
		if (!Error.IsEmpty()) return Error;
		//设置图像
		Error = SetCameraBufferMode(pCam);
		if (!Error.IsEmpty()) return Error;

		//开启采集图像
		pCam->BeginAcquisition();

		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//设置相机为连续模式
CString SetCameraAcquisitionMode(INodeMap& nodeMap)
{
	try
	{
		//获取采集模式节点
		CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
		if (!IsReadable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
		{
			return TEXT("ptrAcquisitionMode不可读或不可写");
		}

		//获取连续采集模式节点
		CEnumEntryPtr ptrContinuousMode = ptrAcquisitionMode->GetEntryByName("Continuous");
		if (!IsReadable(ptrContinuousMode))
		{
			return TEXT("ptrContinuousMode不可读");
		}

		//设置为连续采集模式
		const int64_t ContinuousMode = ptrContinuousMode->GetValue();
		ptrAcquisitionMode->SetIntValue(ContinuousMode);
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		 return CString(CA2W(e.what(), CP_UTF8));
	}
}

//设置图像像素格式，目前不需要了
CString SetCameraPixelFormat(INodeMap& nodeMap)
{
	try
	{
		CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");
		if (!IsReadable(ptrPixelFormat) || !IsWritable(ptrPixelFormat))
		{
			return TEXT("ptrPixelFormat不可读或不可写");
		}

		CEnumEntryPtr ptrPixelFormatMono8 = ptrPixelFormat->GetEntryByName("Mono8");
		if (!IsReadable(ptrPixelFormatMono8)) return false;

		int64_t PixelFormatMono8 = ptrPixelFormatMono8->GetValue();
		ptrPixelFormat->SetIntValue(PixelFormatMono8);
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//设置图像尺寸
CString SetImageSize(INodeMap& nodeMap, CString ImageSize)
{
	try
	{
		CIntegerPtr ptrOffsetX = nodeMap.GetNode("OffsetX");
		if (!IsReadable(ptrOffsetX) || !IsWritable(ptrOffsetX))
			return TEXT("OffsetX节点不可读或不可写");

		CIntegerPtr ptrOffsetY = nodeMap.GetNode("OffsetY");
		if (!IsReadable(ptrOffsetY) || !IsWritable(ptrOffsetY))
			return TEXT("OffsetY节点不可读或不可写");

		CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
		if (!IsReadable(ptrWidth) || !IsWritable(ptrWidth))
			return TEXT("Width节点不可读或不可写");

		CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
		if (!IsReadable(ptrHeight) || !IsWritable(ptrHeight))
			return TEXT("Height节点不可读不写");

		int64_t OffsetX, OffsetY, Width, Height;
		if (ImageSize == TEXT("2448x2048"))
		{
			OffsetX = 0, OffsetY = 0;
			Width = 2448, Height = 2048;
		}
		else if (ImageSize == TEXT("2048x2048"))
		{
			OffsetX = 200; OffsetY = 0;
			Width = 2048; Height = 2048;
		}
		else if (ImageSize == TEXT("1920x1080"))
		{
			OffsetX = 264; OffsetY = 484;
			Width = 1920; Height = 1080;
		}
		else if (ImageSize == TEXT("1120x1120"))
		{
			OffsetX = 664; OffsetY = 464;
			Width = 1120; Height = 1120;
		}
		else if (ImageSize == TEXT("1024x1224"))
		{
			OffsetX = 712; OffsetY = 412;
			Width = 1024; Height = 1224;
		}
		else if (ImageSize == TEXT("1024x1024"))
		{
			OffsetX = 712; OffsetY = 512;
			Width = 1024; Height = 1024;
		}
		else if (ImageSize == TEXT("1280x720"))
		{
			OffsetX = 584; OffsetY = 664;
			Width = 1280; Height = 720;
		}

		ptrWidth->SetValue(Width);
		ptrHeight->SetValue(Height);
		ptrOffsetX->SetValue(OffsetX);
		ptrOffsetY->SetValue(OffsetY);

		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//设置图像缓冲区处理模式
CString SetCameraBufferMode(CameraPtr& pCam)
{
	if (pCam == nullptr) return TEXT("未连接相机");

	try
	{
		INodeMap& sNodeMap = pCam->GetTLStreamNodeMap();

		CEnumerationPtr ptrHandlingMode = sNodeMap.GetNode("StreamBufferHandlingMode");
		if (!IsWritable(ptrHandlingMode)) return TEXT("ptrHandlingMode不可写");

		CEnumEntryPtr ptrNewestOnly = ptrHandlingMode->GetEntryByName("NewestOnly");
		if (!IsReadable(ptrNewestOnly)) return TEXT("ptrNewestOnly不可读");

		ptrHandlingMode->SetIntValue(ptrNewestOnly->GetValue());
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//关闭相机
CString CameraStop(CameraPtr& pCam)
{
	if (pCam == nullptr || !pCam->IsInitialized())
		return TEXT("相机未连接或相机未初始化");

	try
	{
		pCam->EndAcquisition();
		return TEXT("");
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//调节曝光时间
CString SetCameraExposure(CameraPtr& pCam, double TargetTime)
{
	if (pCam == nullptr) return TEXT("相机未连接");

	try
	{
		INodeMap& nodeMap = pCam->GetNodeMap();

		//获取自动曝光节点
		CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
		if (!IsReadable(ptrExposureAuto) || !IsWritable(ptrExposureAuto))
		{
			return TEXT("ptrExposureAuto不可读或不可写");
		}

		//判断是否关闭自动曝光
		CEnumEntryPtr ptrCurrentExposureAuto = ptrExposureAuto->GetCurrentEntry();
		if (!ptrCurrentExposureAuto || !IsReadable(ptrCurrentExposureAuto))
		{
			return TEXT("ptrCurrentExpousreAuto为空指针或不可读");
		}
		gcstring CurrentExposureMode = ptrCurrentExposureAuto->GetSymbolic();

		//关闭自动曝光
		if (CurrentExposureMode != "Off")
		{
			CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
			if (IsReadable(ptrExposureAutoOff))
			{
				ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
			}
		}

		//手动设置曝光时间（单位：微秒）
		CFloatPtr ptrExposureTime = nodeMap.GetNode("ExposureTime");
		if (!IsReadable(ptrExposureTime) || !IsWritable(ptrExposureTime))
		{
			return TEXT("ptrExposureTime不可读或不可写");
		}

		const double ExposureTimeMax = ptrExposureTime->GetMax();
		const double ExposureTimeMin = ptrExposureTime->GetMin();
		if (TargetTime > ExposureTimeMax)
		{
			TargetTime = ExposureTimeMax;
		}
		if (TargetTime < ExposureTimeMin)
		{
			TargetTime = ExposureTimeMin;
		}
		ptrExposureTime->SetValue(TargetTime);
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//调节增益
CString SetCameraGain(CameraPtr& pCam, double TargetGain)
{
	if (pCam == nullptr) return TEXT("相机未连接");

	try
	{
		INodeMap& nodeMap = pCam->GetNodeMap();

		//获取自动增益节点
		CEnumerationPtr ptrGainAuto = nodeMap.GetNode("GainAuto");
		if (!IsReadable(ptrGainAuto) || !IsWritable(ptrGainAuto))
		{
			return TEXT("ptrGainAuto不可读或不可写");
		}

		//判断是否关闭自动增益
		CEnumEntryPtr ptrCurrentGainAuto = ptrGainAuto->GetCurrentEntry();
		if (!ptrCurrentGainAuto || !IsReadable(ptrCurrentGainAuto))
		{
			return TEXT("ptrCurrentGainAuto为空指针或不可读");
		}
		gcstring CurrentGainMode = ptrCurrentGainAuto->GetSymbolic();

		//关闭自动增益
		if (CurrentGainMode != "Off")
		{
			CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
			if (IsReadable(ptrGainAutoOff))
			{
				ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
			}
		}
		
		//手动设置增益
		CFloatPtr ptrGain = nodeMap.GetNode("Gain");
		if (!IsReadable(ptrGain) || !IsWritable(ptrGain))
		{
			return TEXT("ptrGain不可读或不可写");
		}

		const double GainMax = ptrGain->GetMax();
		const double GainMin = ptrGain->GetMin();
		if (TargetGain > GainMax)
		{
			TargetGain = GainMax;
		}
		if (TargetGain < GainMin)
		{
			TargetGain = GainMin;
		}
		ptrGain->SetValue(TargetGain);
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//恢复自动曝光
CString ResetCameraExposure(CameraPtr& pCam)
{
	CString Error = TEXT("");
	if (pCam == nullptr) return TEXT("相机未连接");

	try
	{
		INodeMap& nodeMap = pCam->GetNodeMap();

		CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
		if (!IsReadable(ptrExposureAuto) || !IsWritable(ptrExposureAuto))
		{
			return TEXT("ptrExposureAuto不可读或不可写");
		}

		CEnumEntryPtr ptrExposureAutoContinuous = ptrExposureAuto->GetEntryByName("Continuous");
		if (!IsReadable(ptrExposureAutoContinuous))
		{
			return TEXT("ptrExposureAutoContinuous不可读");
		}

		ptrExposureAuto->SetIntValue(ptrExposureAutoContinuous->GetValue());
		return TEXT("");
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//恢复自动增益
CString ResetCameraGain(CameraPtr& pCam)
{
	if (pCam == nullptr) return TEXT("相机未连接");

	try
	{
		INodeMap& nodeMap = pCam->GetNodeMap();

		CEnumerationPtr ptrGainAuto = nodeMap.GetNode("GainAuto");
		if (!IsReadable(ptrGainAuto) || !IsWritable(ptrGainAuto))
		{
			return TEXT("ptrGainAuto不可读或不可写");
		}

		CEnumEntryPtr ptrGainAutoContinuous = ptrGainAuto->GetEntryByName("Continuous");
		if (!IsReadable(ptrGainAutoContinuous))
		{
			return TEXT("ptrGainAutoContinuous不可读");
		}

		ptrGainAuto->SetIntValue(ptrGainAutoContinuous->GetValue());
		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	} 
}

//保存图片
CString CameraSavePicture(CameraPtr& pCam, CString PicturePath, CString PixelFormat, CString ImageFormat)
{
	if (pCam == nullptr || PicturePath == TEXT("请设置图片保存路径"))
	{
		return TEXT("相机未连接或未正确设置图片保存路径");
	}

	if (PicturePath.Right(1) != TEXT("\\"))
	{
		PicturePath += TEXT("\\");
	}

	try
	{
		//计算超时时间
		INodeMap& nodeMap = pCam->GetNodeMap();
		CFloatPtr ExposureTime = nodeMap.GetNode("ExposureTime");
		if (!IsReadable(ExposureTime))
		{
			return TEXT("曝光时间节点不可读");
		}
		uint64_t timeout = static_cast<uint64_t>(ExposureTime->GetValue() / 1000 + 1000);

		ImagePtr image = pCam->GetNextImage(timeout);
		if (image->IsIncomplete())
		{
			return TEXT("图像不完整");
		}

		// 创建图像处理器
		ImageProcessor processor;
		processor.SetColorProcessing(SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR);

		//转换图像像素格式
		ImagePtr ConvertImage = image;
		if (PixelFormat == TEXT("Mono8"))
		{
			ConvertImage = processor.Convert(image, PixelFormat_Mono8);
		}
		else if (PixelFormat == TEXT("RGB8"))
		{
			ConvertImage = processor.Convert(image, PixelFormat_RGB8);
		}
		else TEXT("图像格式设置错误");

		//获取当前时间戳
		CTime time = CTime::GetCurrentTime();
		CString strTime;
		strTime.Format(TEXT("%d-%d-%d_%d-%d-%d"), time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());

		//根据图像格式保存图片
		CString FilePath = PicturePath + strTime;
		Spinnaker::ImageFileFormat saveFormat;
		if (ImageFormat == TEXT("JPG"))
		{
			FilePath += TEXT(".jpg");
			saveFormat = SPINNAKER_IMAGE_FILE_FORMAT_JPEG;
		}
		else if (ImageFormat == TEXT("BMP"))
		{
			FilePath += TEXT(".bmp");
			saveFormat = SPINNAKER_IMAGE_FILE_FORMAT_BMP;
		}
		else if (ImageFormat == TEXT("PNG"))
		{
			FilePath += TEXT(".png");
			saveFormat = SPINNAKER_IMAGE_FILE_FORMAT_PNG;
		}
		else return TEXT("图像保存格式设置错误");

		string stdPath = CT2A(FilePath);
		ConvertImage->Save(stdPath.c_str(), saveFormat);
		if(PathFileExists(FilePath))
			return TEXT("");//正常返回
		else {
			// 文件不存在，返回错误信息
			CString errMsg = TEXT("保存操作完成但文件未找到，路径：");
			errMsg += FilePath;
			return errMsg;
		}
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

//录制视频
CString CameraSaveVideo(CameraPtr& pCam, SpinVideo& VideoWriter, CString VideoPath, CString VideoFormat, CString ImageSize)
{
	if (pCam == nullptr || VideoPath == TEXT("请设置视频保存路径"))
	{
		return TEXT("相机未连接或未正确设置视频保存路径");
	}

	if (VideoPath.Right(1) != TEXT("\\"))
	{
		VideoPath += TEXT("\\");
	}

	//获取当前时间戳作为文件名，得到完整路径
	CTime time = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(TEXT("%d-%d-%d_%d-%d-%d"), time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	CString FilePath = VideoPath + strTime;

	//确定图像尺寸
	int Width = 0, Height = 0;
	if (ImageSize == TEXT("2448x2048"))      Width = 2448, Height = 2048;
	else if (ImageSize == TEXT("2048x2048")) Width = 2048, Height = 2048;
	else if (ImageSize == TEXT("1920x1048")) Width = 1920, Height = 1048;
	else if (ImageSize == TEXT("1120x1120")) Width = 1120, Height = 1120;
	else if (ImageSize == TEXT("1024x1224")) Width = 1024, Height = 1224;
	else if (ImageSize == TEXT("1024x1024")) Width = 1024, Height = 1024;
	else if (ImageSize == TEXT("1280x720"))  Width = 1280, Height = 720;

	try
	{
		float FrameRate = 15;

		if (VideoFormat == TEXT("UNCOMPRESSED-AVI"))
		{
			Video::AVIOption option;

			option.frameRate = FrameRate;
			option.width = Width;
			option.height = Height;

			VideoWriter.Open(CT2A(FilePath), option);
		}
		else if (VideoFormat == TEXT("MJPG-AVI"))
		{
			Video::MJPGOption option;

			option.frameRate = FrameRate;
			option.width = Width;
			option.height = Height;
			option.quality = 75;//0~100，值越大画质越好

			VideoWriter.Open(CT2A(FilePath), option);
		}
		else if (VideoFormat == TEXT("H.264-AVI") || VideoFormat == TEXT("H.264-MP4"))
		{
			Video::H264Option option;

			option.frameRate = FrameRate;
			option.width = Width;
			option.height = Height;
			option.bitrate = 1000000;
			option.useMP4 = (VideoFormat == TEXT("H.264-MP4"));
			option.crf = 23;//0~51，值越小质量越高

			VideoWriter.Open(CT2A(FilePath), option);
		}

		return TEXT("");//正常返回
	}
	catch (Spinnaker::Exception& e)
	{
		return CString(CA2W(e.what(), CP_UTF8));
	}
}

