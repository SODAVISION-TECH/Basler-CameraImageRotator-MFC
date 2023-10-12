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
#include <afxshellmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGuiSampleMultiCamApp

BEGIN_MESSAGE_MAP(CGuiSampleMultiCamApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGuiSampleMultiCamApp construction

CGuiSampleMultiCamApp::CGuiSampleMultiCamApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance.
}


// The one and only CGuiSampleMultiCamApp object

CGuiSampleMultiCamApp theApp;


// CGuiSampleMultiCamApp initialization
// Before using any pylon methods we must initialize pylon
// by calling Pylon::PylonIntialize().
BOOL CGuiSampleMultiCamApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Before using any pylon methods. The pylon runtime must be initialized.
    Pylon::PylonInitialize();

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    CShellManager *pShellManager = new CShellManager;

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("Basler\\pylon\\Samples"));

    CGuiSampleMultiCamDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK.
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel.
    }

    // Delete the shell manager created above.
    if (pShellManager != NULL)
    {
        delete pShellManager;
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

// This will be called when the application exits.
// We call Pylon::PylonTerminate() to release all pylon resources.
int CGuiSampleMultiCamApp::ExitInstance()
{
    // Releases all pylon resources.
    Pylon::PylonTerminate();

    return CWinApp::ExitInstance();
}
