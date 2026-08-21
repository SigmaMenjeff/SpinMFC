
// SpinMFCDlg.h: 头文件
//

#pragma once

#include <vector>
#include "SpinCamera.h"

// 自定义消息：用于工作线程向主线程发送图像数据
#define WM_UPDATE_IMAGE (WM_USER + 100)

// 自定义消息，用于通知录制错误
#define WM_RECORDING_ERROR (WM_USER + 101)

//图像数据包，用于线程间传输
struct ImagePacket
{
	int width;
	int height;
	int bpp;
	std::vector<BYTE> data;
};

// CSpinMFCDlg 对话框
class CSpinMFCDlg : public CDialogEx
{
// 构造
public:
	CSpinMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPINMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//CImage m_testImage;
	CWinThread* m_pThread;
	volatile bool Flag_Running;

	CImage m_ShowImage;
	afx_msg LRESULT OnUpdateImage(WPARAM wParam, LPARAM lParam);
	static UINT ImageAcquisitionThread(LPVOID pParam);

	//CString CurrentPixelFormat;
	int lastDrawWidth;
	int lastDrawHeight;

	bool Flag_Recording;
	Spinnaker::Video::SpinVideo VideoWriter;
	afx_msg LRESULT OnRecordingError(WPARAM wParam, LPARAM lParam);
private:
	// 连接相机按钮
	CButton m_Button_Connect;
	// 打印设备信息
	//CStatic m_Static_DeviceInfo;
public:
	afx_msg void OnBnClickedButtonConnect();
private:
	// 开启相机按钮
	CButton m_Button_Start;
public:
	afx_msg void OnBnClickedButtonStart();
private:
	// 曝光时间滑动条
	CSliderCtrl m_Slider_Exposure;
	// 曝光时间编辑框
	CEdit m_Edit_Exposure;
	// 增益滑动条
	CSliderCtrl m_Slider_Gain;
	// 增益编辑框
	CEdit m_Edit_Gain;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	// 图像像素格式下拉框
	CComboBox m_Combox_PixelFormat;
public:
	afx_msg void OnCbnSelchangeComboFormat();
private:
	// 修改图片保存路径按钮
	CButton m_Button_PicturePath;
	// 显示图片保存路径
	CStatic m_Static_PicturePath;
public:
	afx_msg void OnBnClickedButtonPicture();
private:
	// 修改视频路径按钮
	CButton m_Button_VideoPath;
public:
	// 打印视频路径
	CStatic m_Static_VideoPath;
	afx_msg void OnBnClickedButtonVideo();
private:
	// 保存图片按钮
	CButton m_Button_SavePicture;
public:
	afx_msg void OnBnClickedButtonSavepicture();
private:
	// 录制视频按钮
	CButton m_Button_SaveVideo;
public:
	afx_msg void OnBnClickedButtonSavevideo();
private:
	// 图像尺寸下拉框
	CComboBox m_Combox_ImageSize;
public:
	afx_msg void OnCbnSelchangeComboImagesize();
private:
	// 图像格式
	CComboBox m_Combox_ImageFormat;
public:
	afx_msg void OnCbnSelchangeComboImageformat();
private:
	// 设备信息显示编辑框
	CEdit m_Edit_DeviceInfo;
	// 图像显示框架
	CStatic m_Static_ImageFrame;
public:
	afx_msg void OnDestroy();
private:
	// 视频保存格式
	CComboBox m_Combox_VideoFormat;
public:
	afx_msg void OnCbnSelchangeComboVideoformat();
	afx_msg void OnClose();
};
