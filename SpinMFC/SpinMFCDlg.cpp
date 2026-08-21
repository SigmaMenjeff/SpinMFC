
// SpinMFCDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SpinMFC.h"
#include "SpinMFCDlg.h"
#include "afxdialogex.h"
#include "SpinCamera.h"

using namespace Spinnaker::Video;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SystemPtr CameraSystem = nullptr;
CameraList camList;
CameraPtr pCam = nullptr;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSpinMFCDlg 对话框



CSpinMFCDlg::CSpinMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SPINMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpinMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_Button_Connect);
	//DDX_Control(pDX, IDC_STATIC_INFO, m_Static_DeviceInfo);
	DDX_Control(pDX, IDC_BUTTON_START, m_Button_Start);
	DDX_Control(pDX, IDC_SLIDER_EXPOSURE, m_Slider_Exposure);
	DDX_Control(pDX, IDC_EDIT_EXPOSURE, m_Edit_Exposure);
	DDX_Control(pDX, IDC_SLIDER_GAIN, m_Slider_Gain);
	DDX_Control(pDX, IDC_EDIT_GAIN, m_Edit_Gain);
	DDX_Control(pDX, IDC_COMBO_PIXELFORMAT, m_Combox_PixelFormat);
	DDX_Control(pDX, IDC_BUTTON_PICTURE, m_Button_PicturePath);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_Static_PicturePath);
	DDX_Control(pDX, IDC_BUTTON_VIDEO, m_Button_VideoPath);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_Static_VideoPath);
	DDX_Control(pDX, IDC_BUTTON_SAVEPICTURE, m_Button_SavePicture);
	DDX_Control(pDX, IDC_BUTTON_SAVEVIDEO, m_Button_SaveVideo);
	DDX_Control(pDX, IDC_COMBO_IMAGESIZE, m_Combox_ImageSize);
	DDX_Control(pDX, IDC_COMBO_IMAGEFORMAT, m_Combox_ImageFormat);
	DDX_Control(pDX, IDC_EDIT_DEVICEINFO, m_Edit_DeviceInfo);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_Static_ImageFrame);
	DDX_Control(pDX, IDC_COMBO_VIDEOFORMAT, m_Combox_VideoFormat);
}

BEGIN_MESSAGE_MAP(CSpinMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CSpinMFCDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_START, &CSpinMFCDlg::OnBnClickedButtonStart)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_PIXELFORMAT, &CSpinMFCDlg::OnCbnSelchangeComboFormat)
	ON_BN_CLICKED(IDC_BUTTON_PICTURE, &CSpinMFCDlg::OnBnClickedButtonPicture)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO, &CSpinMFCDlg::OnBnClickedButtonVideo)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPICTURE, &CSpinMFCDlg::OnBnClickedButtonSavepicture)
	ON_BN_CLICKED(IDC_BUTTON_SAVEVIDEO, &CSpinMFCDlg::OnBnClickedButtonSavevideo)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGESIZE, &CSpinMFCDlg::OnCbnSelchangeComboImagesize)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGEFORMAT, &CSpinMFCDlg::OnCbnSelchangeComboImageformat)
	ON_MESSAGE(WM_UPDATE_IMAGE, OnUpdateImage)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEOFORMAT, &CSpinMFCDlg::OnCbnSelchangeComboVideoformat)
	ON_MESSAGE(WM_RECORDING_ERROR, OnRecordingError)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSpinMFCDlg 消息处理程序

BOOL CSpinMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	// TODO: 在此添加额外的初始化代码
	//曝光滑动条、编辑框初始化
	m_Slider_Exposure.SetRange(20, 20000);
	m_Slider_Exposure.SetTicFreq(1000);
	m_Slider_Exposure.SetPos(10000);
	m_Slider_Exposure.SetLineSize(1);
	m_Slider_Exposure.SetPageSize(1);
	m_Edit_Exposure.SetWindowTextW(TEXT("10000"));
	//增益滑动条、编辑框初始化
	m_Slider_Gain.SetRange(20, 47);
	m_Slider_Gain.SetTicFreq(10);
	m_Slider_Gain.SetPos(25);
	m_Slider_Gain.SetLineSize(1);
	m_Slider_Gain.SetPageSize(1);
	m_Edit_Gain.SetWindowTextW(TEXT("25"));
	//图像像素格式下拉框初始化
	m_Combox_PixelFormat.AddString(TEXT("BGR8"));
	m_Combox_PixelFormat.AddString(TEXT("Mono8"));
	m_Combox_PixelFormat.SetCurSel(0);
	//CurrentPixelFormat = TEXT("BGR8");
	//图像尺寸下拉框初始化
	m_Combox_ImageSize.AddString(TEXT("2448x2048"));
	m_Combox_ImageSize.AddString(TEXT("2048x2048"));
	m_Combox_ImageSize.AddString(TEXT("1920x1080"));
	m_Combox_ImageSize.AddString(TEXT("1120x1120"));
	m_Combox_ImageSize.AddString(TEXT("1024x1224"));
	m_Combox_ImageSize.AddString(TEXT("1024x1024"));
	m_Combox_ImageSize.AddString(TEXT("1280x720"));
	m_Combox_ImageSize.SetCurSel(0);
	//图像格式下拉框初始化
	m_Combox_ImageFormat.AddString(TEXT("JPG"));
	m_Combox_ImageFormat.AddString(TEXT("BMP"));
	m_Combox_ImageFormat.AddString(TEXT("PNG"));
	m_Combox_ImageFormat.SetCurSel(0);
	//视频格式下拉框初始化
	m_Combox_VideoFormat.AddString(TEXT("UNCOMPRESSED-AVI"));
	m_Combox_VideoFormat.AddString(TEXT("MJPG-AVI"));
	m_Combox_VideoFormat.AddString(TEXT("H.264-AVI"));
	m_Combox_VideoFormat.AddString(TEXT("H.264-MP4"));
	m_Combox_VideoFormat.SetCurSel(1);
	//相框宽高初始化
	lastDrawWidth = 0;
	lastDrawHeight = 0;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSpinMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSpinMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSpinMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




//连接按钮回调函数
void CSpinMFCDlg::OnBnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取控件描述
	CString str, DeviceInfo;
	m_Button_Connect.GetWindowTextW(str);
	if (str == TEXT("连接相机"))
	{
		//连接相机
		int num;
		CString Error = CameraConnect(CameraSystem, num);
		if (!Error.IsEmpty() && num == 0)
		{
			MessageBox(Error); return;
		}
		//打印设备信息
		CString Info1 = GetSDKVersion(CameraSystem);
		CString Info2 = GetDevieInfo(pCam, 50);
		DeviceInfo = Info1 + Info2;
		m_Edit_DeviceInfo.SetWindowTextW(DeviceInfo);
		//修改控件描述
		m_Button_Connect.SetWindowTextW(TEXT("断开相机"));
	}
	else if (str == TEXT("断开相机"))
	{
		//断开相机
		CString Error = CameraDisconnect(pCam);
		if (!Error.IsEmpty())
		{
			MessageBox(Error); return;
		}
		//清空设备信息
		m_Edit_DeviceInfo.SetWindowTextW(DeviceInfo);
		//修改控件描述
		m_Button_Connect.SetWindowTextW(TEXT("连接相机"));
	}
}

//开启按钮回调函数
void CSpinMFCDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	//未连接相机则不操作，直接返回
	m_Button_Connect.GetWindowTextW(str);
	if (str == TEXT("连接相机"))
	{
		MessageBox(TEXT("未连接相机"));return;
	}

	m_Button_Start.GetWindowTextW(str);
	if (str == TEXT("开启相机"))
	{
		//获图像大小、曝光时间、增益
		CString ImageSize, ExposureTime, Gain;
		m_Combox_ImageSize.GetWindowTextW(ImageSize);
		m_Edit_Exposure.GetWindowTextW(ExposureTime);
		m_Edit_Gain.GetWindowTextW(Gain);
		//相机开启操作
		CString Error = CameraStart(pCam, ImageSize);
		if (!Error.IsEmpty())
		{
			MessageBox(Error); return;
		}
		//设置曝光时间
		Error = SetCameraExposure(pCam, _tstof(ExposureTime));
		if (!Error.IsEmpty())
		{
			MessageBox(Error); return;
		}
		//设置增益
		Error = SetCameraGain(pCam, _tstof(Gain));
		if (!Error.IsEmpty())
		{
			MessageBox(Error); return;
		}

		//创建线程
		Flag_Running = true;
		m_pThread = AfxBeginThread(ImageAcquisitionThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if (m_pThread)
		{
			m_pThread->m_bAutoDelete = false;
			m_pThread->ResumeThread();
		}
		//开启相机期间，连接按钮、图像像素格式、图像尺寸不可用
		m_Button_Connect.EnableWindow(false);
		m_Combox_PixelFormat.EnableWindow(false);
		m_Combox_ImageSize.EnableWindow(false);
		//修改控件描述
		m_Button_Start.SetWindowTextW(TEXT("关闭相机"));
	}
	else if (str == TEXT("关闭相机"))
	{
		//相机关闭操作
		CString Error = CameraStop(pCam);
		if (!Error.IsEmpty())
		{
			MessageBox(Error); return;
		}
		//注销线程
		Flag_Running = false;
		if (m_pThread)
		{
			DWORD dwWait = WaitForSingleObject(m_pThread->m_hThread, 1000);
			if (dwWait == WAIT_TIMEOUT)
			{
				TerminateThread(m_pThread->m_hThread, 0);
			}
			delete m_pThread;
			m_pThread = nullptr;
		}
		//恢复连接按钮、图像像素格式、图像尺寸
		m_Button_Connect.EnableWindow(true);
		m_Combox_PixelFormat.EnableWindow(true);
		m_Combox_ImageSize.EnableWindow(true);
		//修改控件描述
		m_Button_Start.SetWindowTextW(TEXT("开启相机"));
	}
}

//参数滑动条回调函数
void CSpinMFCDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_EXPOSURE)
	{
		switch (nSBCode)
		{
			case SB_THUMBTRACK://正在拖动
			{
				int pos = m_Slider_Exposure.GetPos();
				str.Format(TEXT("%d"), pos);
				//同步曝光时间编辑框
				m_Edit_Exposure.SetWindowTextW(str);
				//修改相机曝光时间
				if (pCam == nullptr) return;
				CString Error = SetCameraExposure(pCam, pos);
				if (!Error.IsEmpty()) MessageBox(Error); return;
			}
			case SB_THUMBPOSITION://完成拖动，松开鼠标
			case SB_ENDSCROLL://滚动操作结束
			case SB_LINELEFT:
			case SB_LINERIGHT:
			case SB_PAGELEFT:
			case SB_PAGERIGHT:
			default:
				break;
		}
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_GAIN)
	{
		switch (nSBCode)
		{
			case SB_THUMBTRACK://正在拖动
			{
				int pos = m_Slider_Gain.GetPos();
				str.Format(TEXT("%d"), pos);
				//同步增益编辑框
				m_Edit_Gain.SetWindowTextW(str);
				//修改相机增益
				if (pCam == nullptr) return;
				CString Error = SetCameraGain(pCam, pos);
				if (!Error.IsEmpty()) MessageBox(Error); return;
			}
			case SB_THUMBPOSITION://完成拖动，松开鼠标
			case SB_ENDSCROLL://滚动操作结束
			case SB_LINELEFT:
			case SB_LINERIGHT:
			case SB_PAGELEFT:
			case SB_PAGERIGHT:
			default:
				break;
		}
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

//参数编辑框回调函数
BOOL CSpinMFCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CString value;
		//曝光时间编辑框
		if (GetFocus() == GetDlgItem(IDC_EDIT_EXPOSURE))
		{
			m_Edit_Exposure.GetWindowTextW(value);
			int pos = _ttoi(value);
			//同步曝光时间编辑框
			m_Slider_Exposure.SetPos(pos);
			//修改曝光时间操作
			if (pCam == nullptr) return false;
			CString Error = SetCameraExposure(pCam, pos);
			if (!Error.IsEmpty())
			{
				MessageBox(Error);return false;
			}
			return true;
		}
		else if (GetFocus() == GetDlgItem(IDC_EDIT_GAIN))
		{
			m_Edit_Gain.GetWindowTextW(value);
			int pos = _ttoi(value);
			//同步增益编辑框
			m_Slider_Gain.SetPos(pos);
			//修改增益操作
			if (pCam == nullptr) return false;
			CString Error = SetCameraGain(pCam, pos);
			if (!Error.IsEmpty())
			{
				MessageBox(Error);return false;
			}
			return true;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//图像像素格式下拉框回调函数
void CSpinMFCDlg::OnCbnSelchangeComboFormat()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_Combox_PixelFormat.GetCurSel();
	CString FormatInfo;
	m_Combox_PixelFormat.GetLBText(index, FormatInfo);
	//CurrentPixelFormat = FormatInfo;
	CString str = TEXT("已修改图像像素格式为：");
	str += FormatInfo;
	MessageBox(str);
}

//修改图片保存路径回调函数
void CSpinMFCDlg::OnBnClickedButtonPicture()
{
	// TODO: 在此添加控件通知处理程序代码
	CFolderPickerDialog PictureFolder(NULL, 0, this, 0);
	if (PictureFolder.DoModal() == IDOK)
	{
		CString PicturePath = PictureFolder.GetPathName();
		m_Static_PicturePath.SetWindowTextW(PicturePath);
	}
}

//修改视频保存路径按钮回调函数
void CSpinMFCDlg::OnBnClickedButtonVideo()
{
	// TODO: 在此添加控件通知处理程序代码
	CFolderPickerDialog VideoFolder(NULL, 0, this, 0);
	if (VideoFolder.DoModal() == IDOK)
	{
		CString VideoPath = VideoFolder.GetPathName();
		m_Static_VideoPath.SetWindowTextW(VideoPath);
	}
}

//保存图片按钮回调函数
void CSpinMFCDlg::OnBnClickedButtonSavepicture()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_Button_Connect.GetWindowTextW(str);
	if (str == TEXT("连接相机"))
	{
		MessageBox(TEXT("未连接相机"));return;
	}
	else if (str == TEXT("断开相机"))
	{
		m_Button_Start.GetWindowTextW(str);
		if (str == TEXT("开启相机"))
		{
			MessageBox(TEXT("未开启相机"));return;
		}
		else if (str == TEXT("关闭相机"))
		{
			//图片保存操作
			CString PicturePath, PixelFormat, ImageFormat;
			m_Static_PicturePath.GetWindowTextW(PicturePath);
			m_Combox_PixelFormat.GetWindowTextW(PixelFormat);
			m_Combox_ImageFormat.GetWindowTextW(ImageFormat);
			CString Error = CameraSavePicture(pCam, PicturePath, PixelFormat, ImageFormat);
			if (!Error.IsEmpty())
			{
				MessageBox(Error);return;
			}
			MessageBox(TEXT("图片保存成功"));
		}
	}
}

//录制视频按钮回调函数
void CSpinMFCDlg::OnBnClickedButtonSavevideo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_Button_Connect.GetWindowTextW(str);
	if (str == TEXT("连接相机"))
	{
		MessageBox(TEXT("未连接相机")); return;
	}
	else if (str == TEXT("断开相机"))
	{
		m_Button_Start.GetWindowTextW(str);
		if (str == TEXT("开启相机"))
		{
			MessageBox(TEXT("未开启相机")); return;
		}
		else if (str == TEXT("关闭相机"))
		{
			m_Button_SaveVideo.GetWindowTextW(str);
			if (str == TEXT("录制视频"))
			{
				//开启录制视频操作
				CString VideoPath, VideoFormat, ImageSize;
				m_Static_VideoPath.GetWindowTextW(VideoPath);
				m_Combox_VideoFormat.GetWindowTextW(VideoFormat);
				m_Combox_ImageSize.GetWindowTextW(ImageSize);
				CString Error = CameraSaveVideo(pCam, VideoWriter, VideoPath, VideoFormat, ImageSize);
				if (!Error.IsEmpty())
				{
					MessageBox(Error); return;
				}
				Flag_Recording = true;//设置标志位
				//录制视频期间，开启按钮、图片保存按钮、图像像素格式、图像尺寸、视频格式均不可用
				m_Button_Start.EnableWindow(false);
				m_Button_SavePicture.EnableWindow(false);
				m_Combox_PixelFormat.EnableWindow(false);
				m_Combox_ImageSize.EnableWindow(false);
				m_Combox_VideoFormat.EnableWindow(false);
				//修改控件描述
				str = TEXT("停止录制");
				m_Button_SaveVideo.SetWindowTextW(str);
			}
			else if (str == TEXT("停止录制"))
			{
				//保存视频操作
				Flag_Recording = false;
				VideoWriter.Close();
				MessageBox(TEXT("视频录制成功"));
				//恢复开启按钮、图片保存、图像像素格式、图像尺寸、视频格式
				m_Button_Start.EnableWindow(true);
				m_Button_SavePicture.EnableWindow(true);
				m_Combox_PixelFormat.EnableWindow(true);
				m_Combox_ImageSize.EnableWindow(true);
				m_Combox_VideoFormat.EnableWindow(true);
				//修改控件描述
				str = TEXT("录制视频");
				m_Button_SaveVideo.SetWindowTextW(str);
			}
		}
	}
}

//图像尺寸下拉框回调函数
void CSpinMFCDlg::OnCbnSelchangeComboImagesize()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_Combox_ImageSize.GetCurSel();
	CString FormatInfo;
	m_Combox_ImageSize.GetLBText(index, FormatInfo);
	CString str = TEXT("已修改图像尺寸为：");
	str += FormatInfo;
	MessageBox(str);
}

//图像格式下拉框回调函数
void CSpinMFCDlg::OnCbnSelchangeComboImageformat()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_Combox_ImageFormat.GetCurSel();
	CString FormatInfo;
	m_Combox_ImageFormat.GetLBText(index, FormatInfo);
	CString str = TEXT("已修改图像格式为：");
	str += FormatInfo;
	MessageBox(str);
}

//线程操作函数
UINT CSpinMFCDlg::ImageAcquisitionThread(LPVOID pParam)
{
	CSpinMFCDlg* pDlg = (CSpinMFCDlg*)pParam;

	Spinnaker::ImageProcessor processor;
	processor.SetColorProcessing(SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR);

	while (pDlg->Flag_Running)
	{
		try
		{
			ImagePtr pImage = pCam->GetNextImage(2000);
			if (pImage == nullptr || pImage->IsIncomplete()) continue;

			CString PixelFormat;
			//PixelFormat = pDlg->CurrentPixelFormat;
			pDlg->m_Combox_PixelFormat.GetWindowTextW(PixelFormat);
			if (PixelFormat == TEXT("BGR8"))
			{
				pImage = processor.Convert(pImage, PixelFormat_BGR8);//这里直接用BGR
			}
			else if (PixelFormat == TEXT("Mono8"))
			{
				pImage = processor.Convert(pImage, PixelFormat_Mono8);
			}

			//录制视频
			if (pDlg->Flag_Recording)
			{
				try
				{
					pDlg->VideoWriter.Append(pImage);//这个地方经常出问题
				}
				catch (Spinnaker::Exception& )
				{
					pDlg->Flag_Recording = false;
					pDlg->VideoWriter.Close();
					// 发送错误消息到 UI 线程
					::PostMessage(pDlg->m_hWnd, WM_RECORDING_ERROR, 0, 0);
				}
			}

			//将图像数据整合成数据包
			ImagePacket* pPacket = new ImagePacket();
			pPacket->width = static_cast<int>(pImage->GetWidth());
			pPacket->height = static_cast<int>(pImage->GetHeight());
			//根据格式设置每像素字节数
			if (PixelFormat == TEXT("BGR8")) pPacket->bpp = 3;
			else if (PixelFormat == TEXT("Mono8")) pPacket->bpp = 1;
			//计算数据总大小并分配内存
			size_t dataSize = pPacket->width * pPacket->height * pPacket->bpp;
			pPacket->data.resize(dataSize);
			memcpy(pPacket->data.data(), pImage->GetData(), dataSize);
			//调用图像更新回调函数
			::PostMessage(pDlg->m_hWnd, WM_UPDATE_IMAGE, 0, (LPARAM)pPacket);
			pImage->Release();
		}
		catch (Spinnaker::Exception& )
		{
			
		}
	}
	return 0;
}

//图像更新回调函数
LRESULT CSpinMFCDlg::OnUpdateImage(WPARAM wParam, LPARAM lParam)
{
	if (Flag_Running == false || !IsWindow(m_hWnd))
	{
		delete (ImagePacket*)lParam; return 0;
	}

	ImagePacket* pPacket = (ImagePacket*)lParam;
	if (pPacket == nullptr) return 0;

	if (!IsWindow(m_Static_ImageFrame.GetSafeHwnd()))
	{
		delete pPacket; return 0;
	}

	m_ShowImage.Destroy();
	CString PixelFormat;
	m_Combox_PixelFormat.GetWindowText(PixelFormat);
	m_ShowImage.Create(pPacket->width, pPacket->height, 24, 0);//创建24位位图，B-G-R（小端存储）
	if (m_ShowImage.IsNull())
	{
		delete pPacket; return 0;
	}

	BYTE* pDest = (BYTE*)m_ShowImage.GetBits();//获取位图指针，指向像素缓冲区
	if (pDest)
	{
		int stride = m_ShowImage.GetPitch();//获取每一行的字节数，发现为负数
		int pitch = abs(stride);//全部采用取绝对值后的值
		if (pPacket->bpp == 3)//BGR8
		{
			for (int row = 0; row < pPacket->height; ++row)//逐行处理
			{
				BYTE* pRowDest = pDest + row * stride;//目标行的起始地址
				BYTE* pRowSrc = pPacket->data.data() + row * pPacket->width * 3;//原数据行起始地址
				memcpy(pRowDest, pRowSrc, pPacket->width * pPacket->bpp);//直接复制
			}
		}
		else if (pPacket->bpp == 1)
		{
			for (int row = 0; row < pPacket->height; ++row)
			{
				BYTE* pRowDest = pDest + row * stride;
				BYTE* pRowSrc = pPacket->data.data() + row * pPacket->width;
				for (int col = 0; col < pPacket->width; ++col)
				{
					BYTE gray = pRowSrc[col];
					pRowDest[col * 3 + 0] = gray;
					pRowDest[col * 3 + 1] = gray;
					pRowDest[col * 3 + 2] = gray;
				}
			}
		}
	}
	else
	{
		delete pPacket; return 0;
	}

	//获取图像宽高
	int imageWidth = pPacket->width;
	int imageHeight = pPacket->height;

	// 计算等比例缩放(1:2)
	double scale = 0.4;
	int drawWidth = (int)(imageWidth * scale);
	int drawHeight = (int)(imageHeight * scale);

	if (drawWidth != lastDrawWidth || drawHeight != lastDrawHeight)
	{
		m_Static_ImageFrame.SetWindowPos(nullptr, 0, 0, drawWidth, drawHeight, SWP_NOMOVE | SWP_NOZORDER);

		// 更新记录
		lastDrawWidth = drawWidth;
		lastDrawHeight = drawHeight;
	}
	
	// 4. 高质量缩放绘制
	CClientDC dc(&m_Static_ImageFrame);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	// 获取 CImage 的 HBITMAP 句柄（隐式转换）
	HBITMAP hBitmap = (HBITMAP)m_ShowImage;
	if (hBitmap == nullptr)
	{
		delete pPacket;
		return 0;
	}

	HGDIOBJ hOldBmp = memDC.SelectObject(hBitmap);

	// 设置高质量缩放模式（抖动+双线性插值）
	dc.SetStretchBltMode(HALFTONE);
	// 从内存 DC 拉伸到目标 DC（目标区域即为整个控件）
	dc.StretchBlt(0, 0, drawWidth, drawHeight, &memDC, 0, 0, pPacket->width, pPacket->height, SRCCOPY);

	memDC.SelectObject(hOldBmp);

	delete pPacket;
	return 0;
}

//线程终止函数
void CSpinMFCDlg::OnDestroy()
{
	if (m_pThread)
	{
		Flag_Running = false;
		WaitForSingleObject(m_pThread->m_hThread, 3000);
		delete m_pThread;
		m_pThread = nullptr;
	}

	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

//视频保存格式下拉框回调函数
void CSpinMFCDlg::OnCbnSelchangeComboVideoformat()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_Combox_VideoFormat.GetCurSel();
	CString FormatInfo;
	m_Combox_VideoFormat.GetLBText(index, FormatInfo);
	CString str = TEXT("已修改视频保存格式为：");
	str += FormatInfo;
	MessageBox(str);
}

LRESULT CSpinMFCDlg::OnRecordingError(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox(TEXT("视频录制出现错误，已自动停止。"));
	return 0;
}


void CSpinMFCDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString Error = ApplicationExit(CameraSystem, pCam, camList);
	if (!Error.IsEmpty())
	{
		MessageBox(Error);
	}

	CDialogEx::OnClose();
}
