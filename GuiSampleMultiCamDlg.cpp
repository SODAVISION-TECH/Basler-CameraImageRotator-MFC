/*
    Note: Before getting started, Basler recommends reading the "Programmer's Guide" topic
    in the pylon C++ API documentation delivered with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the "Migrating from Previous Versions" topic in the pylon C++ API documentation.

    This sample illustrates the use of a MFC GUI together with the pylon C++ API to enumerate the attached cameras, to
    configure a camera, to start and stop the grab and to display grabbed images.
    It shows how to use GUI controls to display and modify camera parameters.
*/

#include "stdafx.h"
#include "GuiSampleMultiCam.h"
#include "GuiSampleMultiCamDlg.h"
#include "afxdialogex.h"
#include "RotateRescale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About.
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGuiSampleMultiCamDlg dialog
// This is main windows for the application.
// All GUI-related functions are handled here.
CGuiSampleMultiCamDlg::CGuiSampleMultiCamDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CGuiSampleMultiCamDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    for (int i = 0; i < MaxCamera; i++)
    {
        m_camera[i].SetUserHint(i);
    }
}

// Make sure all cameras are closed.
CGuiSampleMultiCamDlg::~CGuiSampleMultiCamDlg()
{
    for (int i = 0; i < MaxCamera; i++)
    {
        m_camera[i].Close();
    }
}

void CGuiSampleMultiCamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CAMERALIST, m_cameraBox);
}

BEGIN_MESSAGE_MAP(CGuiSampleMultiCamDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_SCAN, &CGuiSampleMultiCamDlg::OnBnClickedButtonScan)
    ON_BN_CLICKED(IDC_OPEN_SELECTED_1, &CGuiSampleMultiCamDlg::OnBnClickedOpenSelected1)
    ON_BN_CLICKED(IDC_OPEN_SELECTED_2, &CGuiSampleMultiCamDlg::OnBnClickedOpenSelected2)
    ON_BN_CLICKED(IDC_CLOSE_1, &CGuiSampleMultiCamDlg::OnBnClickedClose1)
    ON_BN_CLICKED(IDC_CLOSE_2, &CGuiSampleMultiCamDlg::OnBnClickedClose2)
    ON_BN_CLICKED(IDC_SINGLESHOT_1, &CGuiSampleMultiCamDlg::OnBnClickedSingleshot1)
    ON_BN_CLICKED(IDC_SINGLESHOT_2, &CGuiSampleMultiCamDlg::OnBnClickedSingleshot2)
    ON_BN_CLICKED(IDC_CONTINUOUS_1, &CGuiSampleMultiCamDlg::OnBnClickedContinuous1)
    ON_BN_CLICKED(IDC_CONTINUOUS_2, &CGuiSampleMultiCamDlg::OnBnClickedContinuous2)
    ON_BN_CLICKED(IDC_STOP_1, &CGuiSampleMultiCamDlg::OnBnClickedStop1)
    ON_BN_CLICKED(IDC_STOP_2, &CGuiSampleMultiCamDlg::OnBnClickedStop2)
    ON_CBN_SELCHANGE( IDC_CAMERALIST, &CGuiSampleMultiCamDlg::OnCbnSelchangeCameralist )
    ON_CBN_SELCHANGE( IDC_PIXELFORMAT_1, &CGuiSampleMultiCamDlg::OnCbnSelchangePixelformat1 )
    ON_CBN_SELCHANGE(IDC_PIXELFORMAT_2, &CGuiSampleMultiCamDlg::OnCbnSelchangePixelformat2)
    ON_CBN_SELCHANGE(IDC_TRIGGERMODE_1, &CGuiSampleMultiCamDlg::OnCbnSelchangeTriggermode1)
    ON_CBN_SELCHANGE(IDC_TRIGGERMODE_2, &CGuiSampleMultiCamDlg::OnCbnSelchangeTriggermode2)
    ON_CBN_SELCHANGE(IDC_TRIGGERSOURCE_1, &CGuiSampleMultiCamDlg::OnCbnSelchangeTriggersource1)
    ON_CBN_SELCHANGE(IDC_TRIGGERSOURCE_2, &CGuiSampleMultiCamDlg::OnCbnSelchangeTriggersource2)
    ON_BN_CLICKED(IDC_SOFTWARETRIGGER_1, &CGuiSampleMultiCamDlg::OnBnClickedSoftwaretrigger1)
    ON_BN_CLICKED(IDC_SOFTWARETRIGGER_2, &CGuiSampleMultiCamDlg::OnBnClickedSoftwaretrigger2)
    ON_BN_CLICKED(IDC_INVERTPIXEL_1, &CGuiSampleMultiCamDlg::OnBnClickedInvertpixel1)
    ON_BN_CLICKED(IDC_ROTATEIMAGE_1, &CGuiSampleMultiCamDlg::OnBnClickedRotateimage1)
	ON_BN_CLICKED(IDC_ROTATEIMAGE_2, &CGuiSampleMultiCamDlg::OnBnClickedRotateimage2)
    ON_BN_CLICKED(IDC_INVERTPIXEL_2, &CGuiSampleMultiCamDlg::OnBnClickedInvertpixel2)
    ON_BN_CLICKED(IDC_OPEN_BY_SN_1, &CGuiSampleMultiCamDlg::OnBnClickedOpenBySn1)
    ON_BN_CLICKED(IDC_OPEN_BY_SN_2, &CGuiSampleMultiCamDlg::OnBnClickedOpenBySn2)
    ON_BN_CLICKED(IDC_OPEN_BY_USERID_1, &CGuiSampleMultiCamDlg::OnBnClickedOpenByUserid1)
    ON_BN_CLICKED(IDC_OPEN_BY_USERID_2, &CGuiSampleMultiCamDlg::OnBnClickedOpenByUserid2)
    ON_EN_CHANGE(IDC_EDIT_SN_1, &CGuiSampleMultiCamDlg::OnEnChangeEditSn1)
    ON_EN_CHANGE(IDC_EDIT_SN_2, &CGuiSampleMultiCamDlg::OnEnChangeEditSn2)
    ON_EN_CHANGE(IDC_EDIT_USERID_1, &CGuiSampleMultiCamDlg::OnEnChangeEditUserid1)
    ON_EN_CHANGE(IDC_EDIT_USERID_2, &CGuiSampleMultiCamDlg::OnEnChangeEditUserid2)
    ON_MESSAGE(WM_NEW_GRABRESULT, &CGuiSampleMultiCamDlg::OnNewGrabresult)
    ON_MESSAGE(WM_DEVICE_REMOVED, &CGuiSampleMultiCamDlg::OnDeviceRemoved)
    ON_MESSAGE(WM_NODES_UPDATED, &CGuiSampleMultiCamDlg::OnNodesUpdated)
    ON_MESSAGE( WM_GRAB_STATE_CHANGED, &CGuiSampleMultiCamDlg::OnGrabStateChanged )
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// This is called once before the dialog is shown.
// Here, we create our tooltips as they are not implemented by default in MFC dialog applications.
// We enable/disable the controls and set up a timer for our FPS measurement.
BOOL CGuiSampleMultiCamDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
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

    // Set the icon for this dialog. The framework does this automatically
    //  when the application's main window is not a dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
    SetIcon(hIcon, TRUE);         // Set big icon.
    SetIcon(hIcon, FALSE);        // Set small icon.

    // TODO: Add extra initialization here.
    
    // Create tooltip and set tooltip texts.
    if (!m_toolTip.Create(this))
    {
        TRACE0("Unable to create the ToolTip!");
    }
    else
    {
        m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SN_1), _T("Check for a camera serial number in the camera list and enter it here."));
        m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SN_2), _T("Check for a camera serial number in the camera list and enter it here."));
        m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USERID_1), _T("Check for a device user ID in the camera list and enter it here. You may need to specify a device user ID in the pylon Viewer first."));
        m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USERID_2), _T("Check for a device user ID in the camera list and enter it here. You may need to specify a device user ID in the pylon Viewer first."));
        m_toolTip.Activate(TRUE);
    }

    // Enable/disable controls.
    for (int i = 0; i < MaxCamera; i++)
    {
        UpdateCameraDialog(i);
    }

    // Set timer for status bar (1000 ms update interval).
    SetTimer(100, 1000, NULL);

    // Simulate the user clicked on the scan button.
    // Use post message so it will be executed after the dialog ist fully initialized.
    PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_SCAN, BN_CLICKED ), (LPARAM)GetDlgItem( IDC_BUTTON_SCAN )->GetSafeHwnd() );

    return TRUE;  // return TRUE unless you set the focus to a control.
}


void CGuiSampleMultiCamDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


// This will be called by Windows when the window is to be painted on screen.
// We use the Pylon::CPylonBitmapImage and blit the image onto the dialog.
// Since there is no built-in function to display an image in a dialog,
// we use the IDC_IMAGE control as a placeholder to define the size and position
// of the image in the dialog.
void CGuiSampleMultiCamDlg::OnPaint()
{
    // If you add a minimize button to your dialog, you will need the code below
    // to draw the icon. For MFC applications using the document/view model,
    // this is automatically done for you by the framework.
    if (IsIconic())
    {
        CPaintDC dc(this); // Device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle.
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon.
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();

        // Paint image 0.
        if(m_camera[0].IsOpen())
        {
            // Make sure the bitmap won't be modified by the grab thread while painting.
            CSingleLock lockBmp( m_camera[0].GetBmpLock(), TRUE );

            const Pylon::CPylonBitmapImage& pylonBitmapImage = m_camera[0].GetBitmapImage();
            if (pylonBitmapImage.IsValid())
            {
                CWnd* pWindow = GetDlgItem( IDC_IMAGE_1 );

                CRect rectClient;
                pWindow->GetClientRect(&rectClient);

                CClientDC windowDC( pWindow );

                // If the grabbed image will be scaled down by StretchBlt() below,
                // we need to set the StretchBlit mode. The default mode 
                // will result in unwanted artifacts.
                const bool scaledDown = (uint32_t)rectClient.Width() < pylonBitmapImage.GetWidth() || (uint32_t)rectClient.Height() < pylonBitmapImage.GetHeight();
                if (scaledDown)
                {
                    windowDC.SetStretchBltMode(COLORONCOLOR);
                }

				if (m_rotateImage1)
				{
					CDC dcBitmap;
					dcBitmap.CreateCompatibleDC(&windowDC);
					HGDIOBJ oldhBmp = dcBitmap.SelectObject((HGDIOBJ)(HBITMAP)pylonBitmapImage);
					windowDC.StretchBlt(0, 0, rectClient.Height(), rectClient.Width(), &dcBitmap, 0, 0, pylonBitmapImage.GetWidth(), pylonBitmapImage.GetHeight(), SRCCOPY);
				}
				else
				{
					CDC dcBitmap;
					dcBitmap.CreateCompatibleDC(&windowDC);
					HGDIOBJ oldhBmp = dcBitmap.SelectObject((HGDIOBJ)(HBITMAP)pylonBitmapImage);
					windowDC.StretchBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBitmap, 0, 0, pylonBitmapImage.GetWidth(), pylonBitmapImage.GetHeight(), SRCCOPY);

				}
				
            }
        }

        // Paint image 1.
        if (m_camera[1].IsOpen())
        {
            // Make sure the bitmap won't be modified by the grab thread while painting.
            CSingleLock lockBmp( m_camera[1].GetBmpLock(), TRUE );

            const Pylon::CPylonBitmapImage& pylonBitmapImage = m_camera[1].GetBitmapImage();
            if (pylonBitmapImage.IsValid())
            {
                CWnd* pWindow = GetDlgItem( IDC_IMAGE_2 );

                CRect rectClient;
                pWindow->GetClientRect( &rectClient );

                CClientDC windowDC( pWindow );

                // If the grabbed image will be scaled down by StretchBlt() below,
                // we need to set the StretchBlit mode. The default mode 
                // will create artefacts create unwanted artefacts.
                const bool scaleDown = (uint32_t) rectClient.Width() < pylonBitmapImage.GetWidth() || (uint32_t) rectClient.Height() < pylonBitmapImage.GetHeight();
                if (scaleDown)
                {
                    windowDC.SetStretchBltMode( COLORONCOLOR );
                }

                CDC dcBitmap;
                dcBitmap.CreateCompatibleDC( &windowDC );
                HGDIOBJ oldhBmp = dcBitmap.SelectObject( (HGDIOBJ) (HBITMAP) pylonBitmapImage );
                windowDC.StretchBlt( 0, 0, rectClient.Width(), rectClient.Height(), &dcBitmap, 0, 0, pylonBitmapImage.GetWidth(), pylonBitmapImage.GetHeight(), SRCCOPY );
            }
        }
    }
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGuiSampleMultiCamDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


// Update the camera dialog by enabling/disabling control elements according to the camera state.
void CGuiSampleMultiCamDlg::UpdateCameraDialog(int cameraId)
{
    bool isCameraSelected = (m_cameraBox.GetCurSel() != -1);
    bool isOpen = m_camera[cameraId].IsOpen();
    bool isGrabbing = isOpen && m_camera[cameraId].IsGrabbing();

    if (cameraId == 0)
    {
        GetDlgItem(IDC_OPEN_SELECTED_1)->EnableWindow(!isOpen && isCameraSelected);
        GetDlgItem(IDC_EDIT_SN_1)->EnableWindow(!isOpen);
        GetDlgItem(IDC_EDIT_USERID_1)->EnableWindow(!isOpen);
        GetDlgItem(IDC_CLOSE_1)->EnableWindow(isOpen);
        GetDlgItem(IDC_SINGLESHOT_1)->EnableWindow(isOpen && !isGrabbing);
        GetDlgItem(IDC_CONTINUOUS_1)->EnableWindow(isOpen && !isGrabbing);
        GetDlgItem(IDC_STOP_1)->EnableWindow(isGrabbing);
        GetDlgItem(IDC_SOFTWARETRIGGER_1)->EnableWindow(isOpen);
        GetDlgItem(IDC_INVERTPIXEL_1)->EnableWindow(isOpen);
		GetDlgItem(IDC_ROTATEIMAGE_1)->EnableWindow(isOpen);
		GetDlgItem(IDC_ROTATEIMAGE_2)->EnableWindow(isOpen);

        // Disable these controls when a camera is open. Otherwise, check input.
        if (isOpen)
        {
            GetDlgItem(IDC_OPEN_BY_SN_1)->EnableWindow(!isOpen);
            GetDlgItem(IDC_OPEN_BY_USERID_1)->EnableWindow(!isOpen);
        }
        else
        {
            OnEnChangeEditSn1();
            OnEnChangeEditUserid1();

            // Clear feature controls.
            ClearSlider( IDC_EXPOSURE_1, IDC_EXPOSURE_STATIC_1 );
            ClearSlider( IDC_GAIN_1, IDC_GAIN_STATIC_1 );
            ClearEnumeration( IDC_PIXELFORMAT_1 );
            ClearEnumeration( IDC_TRIGGERMODE_1 );
            ClearEnumeration( IDC_TRIGGERSOURCE_1 );
        }
    }
    else if (cameraId == 1)
    {
        GetDlgItem(IDC_OPEN_SELECTED_2)->EnableWindow(!isOpen && isCameraSelected);
        GetDlgItem(IDC_EDIT_SN_2)->EnableWindow(!isOpen);
        GetDlgItem(IDC_EDIT_USERID_2)->EnableWindow(!isOpen);
        GetDlgItem(IDC_CLOSE_2)->EnableWindow(isOpen);
        GetDlgItem(IDC_SINGLESHOT_2)->EnableWindow(isOpen && !isGrabbing);
        GetDlgItem(IDC_CONTINUOUS_2)->EnableWindow(isOpen && !isGrabbing);
        GetDlgItem(IDC_STOP_2)->EnableWindow(isGrabbing);
        GetDlgItem(IDC_SOFTWARETRIGGER_2)->EnableWindow(isOpen);
        GetDlgItem(IDC_INVERTPIXEL_2)->EnableWindow(isOpen);
        
        // Disable these controls when a camera is open. Otherwise, check input.
        if (isOpen)
        {
            GetDlgItem(IDC_OPEN_BY_SN_2)->EnableWindow(!isOpen);
            GetDlgItem(IDC_OPEN_BY_USERID_2)->EnableWindow(!isOpen);
        }
        else
        {
            OnEnChangeEditSn2();
            OnEnChangeEditUserid2();

            // Clear feature controls.
            ClearSlider( IDC_EXPOSURE_2, IDC_EXPOSURE_STATIC_2 );
            ClearSlider( IDC_GAIN_2, IDC_GAIN_STATIC_2 );
            ClearEnumeration( IDC_PIXELFORMAT_2 );
            ClearEnumeration( IDC_TRIGGERMODE_2 );
            ClearEnumeration( IDC_TRIGGERSOURCE_2 );
        }
    }
    else
    {
        _ASSERT(false);
    }
}


// Helper function to get a list of all attached devices and store it in m_devices.
int CGuiSampleMultiCamDlg::EnumerateDevices()
{
    Pylon::DeviceInfoList_t devices;
    try
    {
        // Get the transport layer factory.
        Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();

        // Get all attached cameras.
        TlFactory.EnumerateDevices(devices);
    }
    catch (const Pylon::GenericException& e)
    {
        UNUSED(e);
        devices.clear();

        TRACE(CUtf82W(e.GetDescription()));
    }

    m_devices = devices;

    // When calling this function, make sure to update the device list control,
    // because its items store pointers to elements in the m_devices list.
    return (int)m_devices.size();
}


// This will be executed when the Scan button has been clicked.
// We will get the list of attached devices and fill m_cameraBox list.
void CGuiSampleMultiCamDlg::OnBnClickedButtonScan()
{
    // Remove all items from the combo box.
    m_cameraBox.ResetContent();

    CWaitCursor waitCur;
    // Enumerate devices.
    int deviceCount = EnumerateDevices();
    waitCur.Restore();

    if (deviceCount == 0)
    {
        OnCbnSelchangeCameralist();
        AfxMessageBox(_T("No camera found."));
        return;
    }

    // Fill the combo box.
    for (Pylon::DeviceInfoList_t::const_iterator it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        // Get the pointer to the current device info.
        const Pylon::CDeviceInfo* const pDeviceInfo = &(*it);

        // Add the friendly name to the list. 
        Pylon::String_t friendlyName = pDeviceInfo->GetFriendlyName();
        int index = m_cameraBox.AddString( CUtf82W( friendlyName ) );
        // Add a pointer to CDeviceInfo as item data so we can use it later.
        m_cameraBox.SetItemData( index, (DWORD_PTR) pDeviceInfo );
    }

    // Select first item.
    m_cameraBox.SetCurSel( 0 );
    
    // Enable/disable controls.
    OnCbnSelchangeCameralist();
}


// Open the currently selected camera.
void CGuiSampleMultiCamDlg::OnBnClickedOpenSelected1()
{
    int index = m_cameraBox.GetCurSel();
    if (index == CB_ERR)
    {
        return;
    }

    // Get the pointer to Pylon::CDeviceInfo selected.
    const Pylon::CDeviceInfo* pDeviceInfo = (const Pylon::CDeviceInfo*)m_cameraBox.GetItemData( index );

    // Open the camera.
    InternalOpenCamera( *pDeviceInfo, 0 );
}


// Open the currently selected camera.
void CGuiSampleMultiCamDlg::OnBnClickedOpenSelected2()
{
    int index = m_cameraBox.GetCurSel();
    if (index == CB_ERR)
    {
        return;
    }

    // Get the pointer to Pylon::CDeviceInfo selected.
    const Pylon::CDeviceInfo* pDeviceInfo = (const Pylon::CDeviceInfo*)m_cameraBox.GetItemData( index );

    // Open the camera.
    InternalOpenCamera( *pDeviceInfo, 1 );
}


// Enable the Open by Serial Number button if a serial number has been entered.
void CGuiSampleMultiCamDlg::OnEnChangeEditSn1()
{
    CString serialNumber;
    GetDlgItem(IDC_EDIT_SN_1)->GetWindowText(serialNumber);
    serialNumber.Trim();
    bool enableButton = ! serialNumber.IsEmpty();
    GetDlgItem(IDC_OPEN_BY_SN_1)->EnableWindow(enableButton);
}


// Enable the Open by Serial Number button if a serial number has been entered.
void CGuiSampleMultiCamDlg::OnEnChangeEditSn2()
{
    CString serialNumber;
    GetDlgItem(IDC_EDIT_SN_2)->GetWindowText(serialNumber);
    serialNumber.Trim();
    bool enableButton = ! serialNumber.IsEmpty();
    GetDlgItem(IDC_OPEN_BY_SN_2)->EnableWindow(enableButton);
}


// Open the camera by using its serial number.
void CGuiSampleMultiCamDlg::OnBnClickedOpenBySn1()
{
    Pylon::CDeviceInfo devInfo;
    CString serialNumber;
    GetDlgItem(IDC_EDIT_SN_1)->GetWindowText(serialNumber);
    serialNumber.Trim();
    devInfo.SetSerialNumber(GetString_t(serialNumber));

    InternalOpenCamera(devInfo, 0);
}


// Open the camera by using its serial number.
void CGuiSampleMultiCamDlg::OnBnClickedOpenBySn2()
{
    Pylon::CDeviceInfo devInfo;
    CString serialNumber;
    GetDlgItem(IDC_EDIT_SN_2)->GetWindowText(serialNumber);
    serialNumber.Trim();
    devInfo.SetSerialNumber(GetString_t(serialNumber));

    InternalOpenCamera(devInfo, 1);
}


// Enable the Open by User ID button if a user ID has been entered.
void CGuiSampleMultiCamDlg::OnEnChangeEditUserid1()
{
    CString userId;
    GetDlgItem(IDC_EDIT_USERID_1)->GetWindowText(userId);
    userId.Trim();
    bool enableButton = ! userId.IsEmpty();
    GetDlgItem(IDC_OPEN_BY_USERID_1)->EnableWindow(enableButton);
}


// Enable the Open by User ID Defined Name button if a user ID has been entered.
void CGuiSampleMultiCamDlg::OnEnChangeEditUserid2()
{
    CString userId;
    GetDlgItem(IDC_EDIT_USERID_2)->GetWindowText(userId);
    userId.Trim();
    bool enableButton = ! userId.IsEmpty();
    GetDlgItem(IDC_OPEN_BY_USERID_2)->EnableWindow(enableButton);
}


// Open the camera by using a user ID.
void CGuiSampleMultiCamDlg::OnBnClickedOpenByUserid1()
{
    Pylon::CDeviceInfo devInfo;
    CString userId;
    GetDlgItem(IDC_EDIT_USERID_1)->GetWindowText(userId);
    userId.Trim();
    devInfo.SetUserDefinedName(GetString_t(userId));

    InternalOpenCamera(devInfo, 0);
}


// Open the camera by using a user ID.
void CGuiSampleMultiCamDlg::OnBnClickedOpenByUserid2()
{
    Pylon::CDeviceInfo devInfo;
    CString userId;
    GetDlgItem(IDC_EDIT_USERID_2)->GetWindowText(userId);
    userId.Trim();
    devInfo.SetUserDefinedName(GetString_t(userId));

    InternalOpenCamera(devInfo, 1);
}


// Helper function to open a CGuiCamera and update controls.
// After the camera has been opened, we adjust the controls to configure
// the camera features. Sliders ranges are set and drop-down lists are filled
// with enumeration entries.
bool CGuiSampleMultiCamDlg::InternalOpenCamera(const Pylon::CDeviceInfo &devInfo, int cameraId)
{
    try
    {
        // Open() may throw exceptions
        m_camera[cameraId].Open( devInfo );
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not open camera!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );

        return false;
    }

    try
    {
        // Update controls.
        UpdateCameraDialog(cameraId);
        if (cameraId == 0)
        {
            UpdateSlider(IDC_EXPOSURE_1, m_camera[cameraId].GetExposureTime());
            UpdateSliderText(IDC_EXPOSURE_STATIC_1, m_camera[cameraId].GetExposureTime());
            UpdateSlider(IDC_GAIN_1, m_camera[cameraId].GetGain());
            UpdateSliderText(IDC_GAIN_STATIC_1, m_camera[cameraId].GetGain());
            UpdateEnumeration(IDC_PIXELFORMAT_1, m_camera[cameraId].GetPixelFormat());
            UpdateEnumeration(IDC_TRIGGERMODE_1, m_camera[cameraId].GetTriggerMode());
            UpdateEnumeration(IDC_TRIGGERSOURCE_1, m_camera[cameraId].GetTriggerSource());
        }
        else if (cameraId == 1)
        {
            UpdateSlider(IDC_EXPOSURE_2, m_camera[cameraId].GetExposureTime());
            UpdateSliderText(IDC_EXPOSURE_STATIC_2, m_camera[cameraId].GetExposureTime());
            UpdateSlider(IDC_GAIN_2, m_camera[cameraId].GetGain());
            UpdateSliderText(IDC_GAIN_STATIC_2, m_camera[cameraId].GetGain());
            UpdateEnumeration(IDC_PIXELFORMAT_2, m_camera[cameraId].GetPixelFormat());
            UpdateEnumeration(IDC_TRIGGERMODE_2, m_camera[cameraId].GetTriggerMode());
            UpdateEnumeration(IDC_TRIGGERSOURCE_2, m_camera[cameraId].GetTriggerSource());
        }
        else
        {
            _ASSERT(false);
        }

        return true;
    }
    catch (const Pylon::GenericException& e)
    {
        UNUSED_ALWAYS( e );
        return false;
    }
}


// Helper function to close a CGuiCamera and update controls.
// After the camera has been closed, we disable controls to configure.
// Sliders ranges are reset and drop-down lists are cleared.
void CGuiSampleMultiCamDlg::InternalCloseCamera(int cameraId)
{
    try
    {
        m_camera[cameraId].Close();

        // Enable/disable controls.
        UpdateCameraDialog(cameraId);
    }
    catch (const Pylon::GenericException& e)
    {
        UNUSED_ALWAYS( e );
    }
}


// Close the camera.
void CGuiSampleMultiCamDlg::OnBnClickedClose1()
{
    InternalCloseCamera( 0 );

    Invalidate();
}


// Close the camera.
void CGuiSampleMultiCamDlg::OnBnClickedClose2()
{
    InternalCloseCamera( 1 );

    Invalidate();
}


// Grab a single image.
void CGuiSampleMultiCamDlg::OnBnClickedSingleshot1()
{
    try
    {
        m_camera[0].SingleGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format ( _T( "Could not start grab!\n%s" ), (LPWSTR)(CUtf82W(e.GetDescription())) );
        AfxMessageBox(strMessage);
    }
}


// Grab a single image.
void CGuiSampleMultiCamDlg::OnBnClickedSingleshot2()
{
    try
    {
        m_camera[1].SingleGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not start grab!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// Start a continuous grab.
void CGuiSampleMultiCamDlg::OnBnClickedContinuous1()
{
    try
    {
        m_camera[0].ContinuousGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not start grab!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// Start a continuous grab.
void CGuiSampleMultiCamDlg::OnBnClickedContinuous2()
{
    try
    {
        m_camera[1].ContinuousGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not start grab!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// Stop a continuous grab.
void CGuiSampleMultiCamDlg::OnBnClickedStop1()
{
    try
    {
        m_camera[0].StopGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not stop grab!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// Stop a continuous grab.
void CGuiSampleMultiCamDlg::OnBnClickedStop2()
{
    try
    {
        m_camera[1].StopGrab();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not stop grab!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// Enable/Disable buttons when the user selects a camera in the m_cameraBox.
void CGuiSampleMultiCamDlg::OnCbnSelchangeCameralist()
{
    // The combo box affects both open buttons.
    UpdateCameraDialog( 0 );
    UpdateCameraDialog( 1 );
}


// This will be called in response to the WM_NEW_GRABRESULT message posted by
// CGuiCamera::OnNewGrabResult() when a new grab result has been received.
// This function is called in the GUI thread, so you can access GUI elements.
LRESULT CGuiSampleMultiCamDlg::OnNewGrabresult(WPARAM wParam, LPARAM lParam)
{
    UNUSED(lParam);

    if ((wParam == 0) && m_camera[0].IsOpen())
    {
        // Make sure the image control will be repainted.
        // The actual drawing is done in OnPaint.

        CRect rect;
        GetDlgItem( IDC_IMAGE_1 )->GetWindowRect( &rect );
        ScreenToClient( rect );

        // We do not need to erase the background as we're overwriting
        // the entire area with the new image.
        InvalidateRect( &rect, FALSE );
    }
    
    if ((wParam == 1) && m_camera[1].IsOpen())
    {
        // Make sure the image control will be repainted.
        // The actual drawing is done in OnPaint.

        CRect rect;
        GetDlgItem( IDC_IMAGE_2 )->GetWindowRect( &rect );
        ScreenToClient( rect );

        // We do not need to erase the background as we're overwriting
        // the entire area with the new image.
        InvalidateRect( &rect, FALSE );
    }

    return 0;
}


// This will be called in response to the WM_DEVICE_REMOVED message posted by
// CGuiCamera::OnCameraDeviceRemoved() when the camera has been dssconnected.
// This function is called in the GUI thread, so you can access GUI elements.
LRESULT CGuiSampleMultiCamDlg::OnDeviceRemoved(WPARAM wParam, LPARAM lParam)
{
    UNUSED(lParam);

    InternalCloseCamera((int)wParam);

    Invalidate();

    AfxMessageBox( _T( "A camera device has been disconnected." ) );

    // Scan for camera devices and refill the list of devies.
    OnBnClickedButtonScan();

    return 0;
}


// This will be called in response to the WM_NODES_UPDATED message posted by
// CGuiCamera::OnCameraEvent() when a camera parameter changes its attributes or value.
// This function is called in the GUI thread, so you can access GUI elements.
LRESULT CGuiSampleMultiCamDlg::OnNodesUpdated(WPARAM wParam, LPARAM lParam)
{
    // lParam contains a pointer to the INode changed
    GenApi::INode* pNode = reinterpret_cast<GenApi::INode*>(lParam);

    // Display the current values.
    if ((wParam == 0) && m_camera[0].IsOpen() && (!m_camera[0].IsCameraDeviceRemoved()))
    {
        if (m_camera[wParam].GetExposureTime().GetNode() == pNode)
        {
            UpdateSlider( IDC_EXPOSURE_1, m_camera[wParam].GetExposureTime() );
            UpdateSliderText( IDC_EXPOSURE_STATIC_1, m_camera[wParam].GetExposureTime() );
        }

        if (m_camera[wParam].GetGain().GetNode() == pNode)
        {
            UpdateSlider( IDC_GAIN_1, m_camera[wParam].GetGain() );
            UpdateSliderText( IDC_GAIN_STATIC_1, m_camera[wParam].GetGain() );
        }

        if (m_camera[wParam].GetPixelFormat().GetNode() == pNode)
        {
            UpdateEnumeration( IDC_PIXELFORMAT_1, m_camera[wParam].GetPixelFormat() );
        }

        if (m_camera[wParam].GetTriggerMode().GetNode() == pNode)
        {
            UpdateEnumeration( IDC_TRIGGERMODE_1, m_camera[wParam].GetTriggerMode() );
        }

        if (m_camera[wParam].GetTriggerSource().GetNode() == pNode)
        {
            UpdateEnumeration(IDC_TRIGGERSOURCE_1, m_camera[wParam].GetTriggerSource());
        }
    }
    else if ((wParam == 1) && m_camera[1].IsOpen() && (! m_camera[1].IsCameraDeviceRemoved()))
    {
        if (m_camera[wParam].GetExposureTime().GetNode() == pNode)
        {
            UpdateSlider( IDC_EXPOSURE_2, m_camera[wParam].GetExposureTime() );
            UpdateSliderText( IDC_EXPOSURE_STATIC_2, m_camera[wParam].GetExposureTime() );
        }

        if (m_camera[wParam].GetGain().GetNode() == pNode)
        {
            UpdateSlider( IDC_GAIN_2, m_camera[wParam].GetGain() );
            UpdateSliderText( IDC_GAIN_STATIC_2, m_camera[wParam].GetGain() );
        }

        if (m_camera[wParam].GetPixelFormat().GetNode() == pNode)
        {
            UpdateEnumeration( IDC_PIXELFORMAT_2, m_camera[wParam].GetPixelFormat() );
        }

        if (m_camera[wParam].GetTriggerMode().GetNode() == pNode)
        {
            UpdateEnumeration( IDC_TRIGGERMODE_2, m_camera[wParam].GetTriggerMode() );
        }

        if (m_camera[wParam].GetTriggerSource().GetNode() == pNode)
        {
            UpdateEnumeration( IDC_TRIGGERSOURCE_2, m_camera[wParam].GetTriggerSource() );
        }
    }

    return 0;
}


// This will be called in response to the WM_GRAB_STATE_CHANGED message posted by
// CGuiCamera when the grab is started or stopped.
// This function is called in the GUI thread, so you can access GUI elements.
LRESULT CGuiSampleMultiCamDlg::OnGrabStateChanged( WPARAM wParam, LPARAM lParam )
{
    UNUSED(lParam);

    // enable/disable start stop buttons
    UpdateCameraDialog(static_cast<int>(wParam));

    return 0;
}


// Called to update value of slider.
void CGuiSampleMultiCamDlg::UpdateSlider(int controlID, Pylon::IIntegerEx& integerParameter)
{
    CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(controlID);
    if (pCtrl == NULL)
    {
        TRACE( _T( "Invalid controlID" ) );
        return;
    }

    if (integerParameter.IsReadable())
    {
        int64_t minimum = integerParameter.GetMin();
        int64_t maximum = integerParameter.GetMax();
        int64_t value = integerParameter.GetValue();

        // NOTE:
        // Possible loss of data because Windows controls only support
        // 32-bit values while GenApi supports 64-bit values.
        pCtrl->SetRange(static_cast<int>(minimum), static_cast<int>(maximum));
        pCtrl->SetPos(static_cast<int>(value));
    }
    pCtrl->EnableWindow(integerParameter.IsWritable());
}


// Update the control with the value of a camera parameter.
void CGuiSampleMultiCamDlg::UpdateSliderText(int controlID, Pylon::IIntegerEx& integerParameter)
{
    CStatic* pString = (CStatic*)GetDlgItem(controlID);
    if (pString == NULL)
    {
        TRACE( _T( "Invalid controlID" ) );
        return;
    }

    if (integerParameter.IsReadable())
    {
        // Set the value as a string in wide character format.
        pString->SetWindowText(CUtf82W(integerParameter.ToString().c_str()));
    }
    else
    {
        pString->SetWindowText(_T("n/a"));
    }
    pString->EnableWindow(integerParameter.IsWritable());
}


// Stores GenApi enumeration items into CComboBox.
void CGuiSampleMultiCamDlg::UpdateEnumeration(int controlID, Pylon::IEnumerationEx& enumParameter)
{
    CComboBox* pCtrl = (CComboBox*)GetDlgItem(controlID);
    if (pCtrl == NULL)
    {
        TRACE( _T( "Invalid controlID" ) );
        return;
    }

    if (enumParameter.IsReadable())
    {
        // Enum entries can become invalid when the camera reconfigured,
        // so we may have to remove existing entries.
        // By iterating from the end to the beginning we don't have to adjust the
        // index when removing an entry.
        for (int index = pCtrl->GetCount(); --index >= 0; /* empty intentionally */)
        {
            GenApi::IEnumEntry* pEntry = reinterpret_cast<GenApi::IEnumEntry*>(pCtrl->GetItemData(index));
            if (!Pylon::CParameter( pEntry ).IsReadable())
            {
                // Entry in control is not valid enum entry anymore, so we remove it.
                pCtrl->DeleteString( index );
            }
        }

        // remember the current entry so we can select it later
        GenApi::IEnumEntry* pCurrentEntry = enumParameter.GetCurrentEntry();
        int currentIndex = CB_ERR;

        // Retrieve the list of entries.
        Pylon::StringList_t symbolics;
        enumParameter.GetSettableValues(symbolics);

        // Add items if not already present.
        for (GenApi::StringList_t::iterator it = symbolics.begin(), end = symbolics.end(); it != end; ++it)
        {
            const Pylon::String_t symbolic = *it;
            GenApi::IEnumEntry* pEntry = enumParameter.GetEntryByName(symbolic);
            if (pEntry != NULL && Pylon::CParameter( pEntry ).IsReadable())
            {
                // Show the human readble display name in the gui
                const CString displayName = CUtf82W(pEntry->GetNode()->GetDisplayName());
                
                int index = pCtrl->FindStringExact( -1, displayName );
                if (index == CB_ERR)
                {
                    // The entry doesn't exist. Add it to the list.
                    // Set the name in wide character format.
                    index = pCtrl->AddString( displayName );
                    // Store the pointer for easy node access.
                    pCtrl->SetItemData( index, reinterpret_cast<DWORD_PTR>(pEntry) );
                }

                // If it is the current entry, select it in the list.
                if (pEntry == pCurrentEntry)
                {
                    pCtrl->SetCurSel( index );
                }
            }
        }
    }

    // Enable/disable control depending on the state of the enum parameter.
    // Also disable if there no valid entries.
    pCtrl->EnableWindow( enumParameter.IsWritable() && pCtrl->GetCount() > 0);
}


// Reset a slider control to default values.
void CGuiSampleMultiCamDlg::ClearSlider(int controlID, int valueControlID )
{
    CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(controlID);
    pCtrl->SetPos(0);
    pCtrl->SetRange(0, 0);
    pCtrl->EnableWindow(FALSE);

    CStatic* pString = (CStatic*) GetDlgItem( valueControlID );
    pString->SetWindowText( _T( "" ) );
}


// Called to update the enumeration in a combo box.
void CGuiSampleMultiCamDlg::ClearEnumeration(int controlID)
{
    CComboBox* pCtrl = (CComboBox*)GetDlgItem(controlID);
    pCtrl->ResetContent();
    pCtrl->EnableWindow(FALSE);
}


// Update a camera parameter when the user selects an entry in a drop-down list.
void CGuiSampleMultiCamDlg::OnUpdateCombobox(int controlID, Pylon::IEnumerationEx& enumParameter)
{
    // Update the camera with the value selected.
    CComboBox* pCtrl = (CComboBox*)GetDlgItem(controlID);
    const int index = pCtrl->GetCurSel();

    if (index == CB_ERR)
    {
        TRACE( "No item selected." );
        return;
    }

    // Get the pointer to the enum entry from the item data.
    GenApi::IEnumEntry* pEntry = reinterpret_cast<GenApi::IEnumEntry*>(pCtrl->GetItemDataPtr(index));

    if (enumParameter.IsWritable())
    {
        // Try to update the pixel format.
        try
        {
            enumParameter.SetIntValue(pEntry->GetValue());
        }
        catch (const GenICam::GenericException& e)
        {
            UNUSED(e);
            TRACE("Failed to set '%hs':%hs", enumParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str(), e.GetDescription());
        }
        catch (...)
        {
            TRACE("Failed to set '%hs'", enumParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str());
        }
    }
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangePixelformat1()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_PIXELFORMAT_1, m_camera[0].GetPixelFormat());
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangePixelformat2()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_PIXELFORMAT_2, m_camera[1].GetPixelFormat());
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangeTriggermode1()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_TRIGGERMODE_1, m_camera[0].GetTriggerMode());
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangeTriggermode2()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_TRIGGERMODE_2, m_camera[1].GetTriggerMode());
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangeTriggersource1()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_TRIGGERSOURCE_1, m_camera[0].GetTriggerSource());
}


// User selected an item in the drop-down list.
void CGuiSampleMultiCamDlg::OnCbnSelchangeTriggersource2()
{
    // Update the camera with the value selected.
    OnUpdateCombobox(IDC_TRIGGERSOURCE_2, m_camera[1].GetTriggerSource());
}


// User clicked on execute software trigger button.
void CGuiSampleMultiCamDlg::OnBnClickedSoftwaretrigger1()
{
    try
    {
        m_camera[0].ExecuteSoftwareTrigger();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not execute software trigger!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// User clicked on execute software trigger button.
void CGuiSampleMultiCamDlg::OnBnClickedSoftwaretrigger2()
{
    try
    {
        m_camera[1].ExecuteSoftwareTrigger();
    }
    catch (const Pylon::GenericException& e)
    {
        CString strMessage;
        // Attention: Format uses a variable argment list, which won't do automatic type conversion.
        // We must explicitly call the conversion parameter of the converter object CUtf82W.
        strMessage.Format( _T( "Could not execute software trigger!\n%s" ), (LPWSTR) (CUtf82W( e.GetDescription() )) );
        AfxMessageBox( strMessage );
    }
}


// User toggled the Invert Pixels checkbox.
void CGuiSampleMultiCamDlg::OnBnClickedInvertpixel1()
{
    CButton* checkBox = (CButton*) GetDlgItem( IDC_INVERTPIXEL_1 );
    bool isChecked = checkBox->GetCheck() == BST_CHECKED;

    m_camera[0].SetInvertImage(isChecked);
}

// User toggled the Rotate image checkbox1
void CGuiSampleMultiCamDlg::OnBnClickedRotateimage1()
{
	CButton* checkBox = (CButton*)GetDlgItem(IDC_ROTATEIMAGE_1);
	bool isChecked = checkBox->GetCheck() == BST_CHECKED;
	m_camera[0].SetRotateImage1(isChecked); 
}

// User toggled the Rotate image checkbox2
void CGuiSampleMultiCamDlg::OnBnClickedRotateimage2()
{
	CButton* checkBox = (CButton*)GetDlgItem(IDC_ROTATEIMAGE_2);
	bool isChecked = checkBox->GetCheck() == BST_CHECKED;
	m_camera[0].SetRotateImage2(isChecked);
}



// User toggled the Invert Pixels checkbox.
void CGuiSampleMultiCamDlg::OnBnClickedInvertpixel2()
{
    CButton* checkBox = (CButton*) GetDlgItem( IDC_INVERTPIXEL_2 );
    bool isChecked = checkBox->GetCheck() == BST_CHECKED;

    m_camera[1].SetInvertImage(isChecked);
}


// User changed a slider value.
void CGuiSampleMultiCamDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // Forward the scroll message to the slider controls.
    if (m_camera[0].IsOpen())
    {
        OnScroll(pScrollBar, IDC_EXPOSURE_1, m_camera[0].GetExposureTime());
        OnScroll(pScrollBar, IDC_GAIN_1, m_camera[0].GetGain());
    }
    
    if (m_camera[1].IsOpen())
    {
        OnScroll(pScrollBar, IDC_EXPOSURE_2, m_camera[1].GetExposureTime());
        OnScroll(pScrollBar, IDC_GAIN_2, m_camera[1].GetGain());
    }

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


// Some parameters allow only specific values and increments.
// The slider controls can return any value selected by the user.
// Round a value to the nearest valid parameter value.
int64_t RoundTo(int64_t newValue, int64_t oldValue, int64_t minimum, int64_t maximum, int64_t increment)
{
    const int Direction = (newValue - oldValue) > 0 ? 1 : -1;
    const int64_t nIncr = (newValue - minimum) / increment;

    switch (Direction)
    {
    case 1: // Up
        return min(maximum, minimum + nIncr * increment);
    case -1: // Down
        return max(minimum, minimum + nIncr * increment);
    }
    return newValue;
}


// Update a slider and set a valid value.
void CGuiSampleMultiCamDlg::OnScroll(CScrollBar* pScrollBar, int controlID, Pylon::IIntegerEx& integerParameter)
{
    CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(controlID);

    // Is the scroll 
    if (pScrollBar->GetSafeHwnd() != pCtrl->GetSafeHwnd())
    {
        return;
    }

    if (integerParameter.IsWritable())
    {
        // Fetch current value, range, and increment of the camera feature.
        int64_t value = integerParameter.GetValue();
        const int64_t minimum = integerParameter.GetMin();
        const int64_t maximum = integerParameter.GetMax();
        const int64_t increment = integerParameter.GetInc();

        // Adjust the pointer to the slider to get the correct position.
        int64_t newvalue = pCtrl->GetPos();

        // Round to the next valid value.
        int64_t roundvalue = RoundTo(newvalue, value, minimum, maximum, increment);
        if (roundvalue == value)
        {
            return;
        }

        // Try to set the value. If successful, update the scroll position.
        try
        {
            integerParameter.SetValue(roundvalue);
            // Set the value in the control again in case it was altered by roundValue.
            pCtrl->SetPos((int)roundvalue);
        }
        catch (const Pylon::GenericException& e)
        {
            TRACE("Failed to set '%hs':%hs", integerParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str(), e.GetDescription());
            UNUSED( e );
        }
        catch (...)
        {
            TRACE("Failed to set '%hs'", integerParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str());
        }
    }
}


// This gets called by a timer about every 1000 ms.
// We use this to update the status bar at regular intervals and calculate the FPS.
// You should not update the status bar on every image received.
// In case you have a fast camera, you might update the GUI 150 times per second or more.
// Whereas the screen will be updated 60 time per second.
// NOTE: Windows will discard WM_TIMER messages if there are to many messages in the message queue.
//       This may happen if you grab with a high frame rate.
void CGuiSampleMultiCamDlg::OnTimer(UINT_PTR nIDEvent)
{

    // Update the status bars.
    if (m_camera[0].IsOpen())
    {
        uint64_t imageCount = m_camera[0].GetGrabbedImages();
        uint64_t errorCount = m_camera[0].GetGrabErrors();
        // Very simple approximation. The timer is triggerd every second.
        double fpsEstimate = (double) m_camera[0].GetGrabbedImagesDiff();

        CString status;
        status.Format(_T("Frame rate: %6.1f fps\tImages: %I64u\tErrors: %I64u"), fpsEstimate, imageCount, errorCount);
        GetDlgItem(IDC_STATUSBAR_1)->SetWindowText(status);
    }
    else
    {
        GetDlgItem(IDC_STATUSBAR_1)->SetWindowText(_T(""));
    }

    if (m_camera[1].IsOpen())
    {
        uint64_t imageCount = m_camera[1].GetGrabbedImages();
        uint64_t errorCount = m_camera[1].GetGrabErrors();
        // Very simple approximation. The timer is triggerd every second.
        double fpsEstimate = (double) m_camera[1].GetGrabbedImagesDiff();

        CString status;
        status.Format(_T("Frame rate: %6.1f fps\tImages: %I64u\tErrors: %I64u"), fpsEstimate, imageCount, errorCount);
        GetDlgItem(IDC_STATUSBAR_2)->SetWindowText(status);
    }
    else
    {
        GetDlgItem(IDC_STATUSBAR_2)->SetWindowText(_T(""));
    }

    CDialogEx::OnTimer(nIDEvent);
}


// For dialog windows we must manually implement tooltips.
// You can remove this, if you don't need tooltips in your application.
BOOL CGuiSampleMultiCamDlg::PreTranslateMessage(MSG* pMsg)
{
    // Relay messages to tooltip.
    m_toolTip.RelayEvent(pMsg);

    return CDialogEx::PreTranslateMessage(pMsg);
}


// We use IDC_IMAGE_1 & IDC_IMAGE_2 to specify the position of the camera image in the dialog.
// When the dialog is painted, we must prevent the control from overwriting our image.
// So when the control starts to paint itself, it will ask the dialog which brush to use.
// We select the NULL_BRUSH making the control transparent.
HBRUSH CGuiSampleMultiCamDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    HBRUSH hbr = CDialogEx::OnCtlColor( pDC, pWnd, nCtlColor );

    // Returning the null brush prevents the static control to overwrite
    // the image we've blitted in OnDraw().
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        if (pWnd->GetSafeHwnd() == GetDlgItem( IDC_IMAGE_1 )->GetSafeHwnd() || pWnd->GetSafeHwnd() == GetDlgItem( IDC_IMAGE_2 )->GetSafeHwnd())
        {
            hbr = (HBRUSH)::GetStockObject( NULL_BRUSH );
        }
    }

    return hbr;
}







