/*
    Note: Before getting started, Basler recommends reading the "Programmer's Guide" topic
    in the pylon C++ API documentation delivered with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the "Migrating from Previous Versions" topic in the pylon C++ API documentation.

    This sample illustrates the use of a MFC GUI together with the pylon C++ API to enumerate the attached cameras, to
    configure a camera, to start and stop the grab and to display grabbed images.
    It shows how to use GUI controls to display and modify camera parameters.
*/

#pragma once

#include <afxdialogex.h>
#include "GuiCamera.h"

// CGuiSampleMultiCamDlg dialog
class CGuiSampleMultiCamDlg : public CDialogEx
{
// Construction
public:
    CGuiSampleMultiCamDlg(CWnd* pParent = NULL);	// Standard constructor
    ~CGuiSampleMultiCamDlg();
	void SetRotateImage1(bool value) {
		m_rotateImage1 = value;}
	void SetRotateImage2(bool value) {
		m_rotateImage2 = value;
	}

// Dialog data
    enum { IDD = IDD_GUI_SAMPLE_MULTI_CAM_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
    int EnumerateDevices();
    void UpdateSlider(int controlID, Pylon::IIntegerEx& integerParameter);
    void UpdateSliderText(int controlID, Pylon::IIntegerEx& integerParameter);
    void UpdateEnumeration(int controlID, Pylon::IEnumerationEx& enumParameter);
    void ClearSlider(int controlID, int valueControlID );
    void ClearEnumeration(int controlID);
    void OnScroll(CScrollBar* pScrollBar, int controlID, Pylon::IIntegerEx& integerParameter);
    void OnUpdateCombobox(int controlID, Pylon::IEnumerationEx& enumParameter);
    void UpdateCameraDialog(int cameraId);
	bool m_rotateImage1, m_rotateImage2;
    bool InternalOpenCamera(const Pylon::CDeviceInfo &devInfo, int cameraId);
    void InternalCloseCamera(int cameraId);

// Implementation
protected:
    HICON m_hIcon;
    CToolTipCtrl m_toolTip;
    Pylon::DeviceInfoList_t m_devices;
    static const int MaxCamera = 2;
    CGuiCamera m_camera[MaxCamera];

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    // custom messages CGuiCamrea sends to the main window to inform about CInstantCamera events
    afx_msg LRESULT OnNewGrabresult(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDeviceRemoved(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNodesUpdated(WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnGrabStateChanged(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

public:
    CComboBox m_cameraBox;
    // command handlers
    afx_msg void OnBnClickedButtonScan();
    afx_msg void OnBnClickedOpenSelected1();
    afx_msg void OnBnClickedOpenSelected2();
    afx_msg void OnBnClickedClose1();
    afx_msg void OnBnClickedClose2();
    afx_msg void OnBnClickedSingleshot1();
    afx_msg void OnBnClickedSingleshot2();
    afx_msg void OnBnClickedContinuous1();
    afx_msg void OnBnClickedContinuous2();
    afx_msg void OnBnClickedStop1();
    afx_msg void OnBnClickedStop2();
    afx_msg void OnCbnSelchangeCameralist();
    afx_msg void OnCbnSelchangePixelformat1();
    afx_msg void OnCbnSelchangePixelformat2();
    afx_msg void OnCbnSelchangeTriggermode1();
    afx_msg void OnCbnSelchangeTriggermode2();
    afx_msg void OnCbnSelchangeTriggersource1();
    afx_msg void OnCbnSelchangeTriggersource2();
    afx_msg void OnBnClickedSoftwaretrigger1();
    afx_msg void OnBnClickedSoftwaretrigger2();
    afx_msg void OnBnClickedInvertpixel1();
    afx_msg void OnBnClickedInvertpixel2();
	afx_msg void OnBnClickedRotateimage1();
	afx_msg void OnBnClickedRotateimage2();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedOpenBySn1();
    afx_msg void OnBnClickedOpenBySn2();
    afx_msg void OnBnClickedOpenByUserid1();
    afx_msg void OnBnClickedOpenByUserid2();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeEditSn1();
    afx_msg void OnEnChangeEditSn2();
    afx_msg void OnEnChangeEditUserid1();
    afx_msg void OnEnChangeEditUserid2();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
};
