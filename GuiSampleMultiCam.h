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

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// Main symbols


// CGuiSampleMultiCamApp:
// See GuiSampleMultiCam.cpp for the implementation of this class.
class CGuiSampleMultiCamApp : public CWinApp
{
public:
    CGuiSampleMultiCamApp();

// Overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation

    DECLARE_MESSAGE_MAP()
};

extern CGuiSampleMultiCamApp theApp;