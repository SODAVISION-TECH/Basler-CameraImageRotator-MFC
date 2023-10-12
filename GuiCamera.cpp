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
#include "GuiCamera.h"
#include "RotateRescale.h"

// CGuiCamera Implementation

CGuiCamera::CGuiCamera() :
    m_cntGrabbedImages( 0 )
    , m_cntSkippedImages( 0 )
    , m_cntGrabErrors( 0 )
    , m_cntLastGrabbedImages( 0 )
    , m_userHint( -1 )
	, m_rotateImage1(false)
	, m_rotateImage2(false)
    , m_invertImage( false )
{
    // Register this object as an image event handler in order to get notified of new images.
    // See Pylon::CImageEventHandler for details.
    m_camera.RegisterImageEventHandler( this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None );

    // Register this object as a configuration event handler in order to get notified of camera state changes.
    // See Pylon::CConfigurationEventHandler for details.
    m_camera.RegisterConfiguration( new Pylon::CAcquireContinuousConfiguration(), Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete );
    m_camera.RegisterConfiguration( this, Pylon::RegistrationMode_Append, Pylon::Cleanup_None );
    
}


CGuiCamera::~CGuiCamera()
{
    Close();
}


void CGuiCamera::SetUserHint( int userHint )
{
    CSingleLock lock( &m_MemberLock, TRUE );
    m_userHint = userHint;
}


int CGuiCamera::GetUserHint() const
{
    return m_userHint;
}

// Creates and opens the camera specified in deviceInfo.
// Initializes the member variables to access camera features.
// Registers camera event handler for camera features, so we'll get notfied when a features changes.
// Registers an image event handler, so we'll get notifed when a new image has been grabbed.
// Registers a configuration event so we can configure the camrea when a grab is being started.
void CGuiCamera::Open( const Pylon::CDeviceInfo& deviceInfo )
{
    CSingleLock lock( &m_MemberLock, TRUE );

    try
    {
        // Add the AutoPacketSizeConfiguration and let pylon delete it when not needed anymore.
        // m_camera.RegisterConfiguration(new CAutoPacketSizeConfiguration(), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

        // Create the device and attach it to CInstantCamera.
        // Let CInstantCamera take care of destroying the device.
        Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateDevice(deviceInfo);
        m_camera.Attach(pDevice, Pylon::Cleanup_Delete);

        // Open camera.
        m_camera.Open();

        // Get the ExposureTime feature.
        // On GigE cameras, the feature is called 'ExposureTimeRaw'.
        // On USB cameras, it is called 'ExposureTime'.
        if (m_camera.ExposureTime.IsValid())
        {
            // We need the integer representation because the GUI controls can only use integer values.
            // If it doesn't exist, return an empty parameter.
            m_camera.ExposureTime.GetAlternativeIntegerRepresentation( m_exposureTime );
        }
        else if (m_camera.ExposureTimeRaw.IsValid())
        {
            m_exposureTime.Attach( m_camera.ExposureTimeRaw.GetNode() );
        }

        // Get the Gain feature.
        // On GigE cameras, the feature is called 'GainRaw'.
        // On USB cameras, it is called 'Gain'.
        if (m_camera.Gain.IsValid())
        {
            // We need the integer representation for the this sample
            // If it doesn't exist, return an empty parameter.
            m_camera.Gain.GetAlternativeIntegerRepresentation( m_gain );
        }
        else if (m_camera.GainRaw.IsValid())
        {
            m_gain.Attach( m_camera.GainRaw.GetNode() );
        }

        // Add event handlers that will be called when the feature changes.

        if (m_exposureTime.IsValid())
        {
            // If we must use the alternative integer representation, we don't know the name of the node as it defined by the camera
            m_camera.RegisterCameraEventHandler( this, m_exposureTime.GetNode()->GetName(), 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional );
        }
        
        if (m_gain.IsValid())
        {
            // If we must use the alternative integer representation, we don't know the name of the node as it defined by the camera
            m_camera.RegisterCameraEventHandler( this, m_gain.GetNode()->GetName(), 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional );
        }

        m_camera.RegisterCameraEventHandler( this, "PixelFormat", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional );

        m_camera.RegisterCameraEventHandler( this, "TriggerMode", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional );

        m_camera.RegisterCameraEventHandler( this, "TriggerSource", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional );


    }
    catch (const Pylon::GenericException& e)
    {
        UNUSED(e);
        TRACE(CUtf82W(e.GetDescription()));

        // Undo everthing.
        Close();

        throw;
    }
}


// Perform cleanup and undo everything we did in Open():
// - Stop the grab.
// - Release all images.
// - Deregister event handlers
// - Free the camera.
void CGuiCamera::Close()
{
    CSingleLock lock( &m_MemberLock, TRUE );

    // Stop the grab, so the grab thread will not set new m_ptrGrabResult.
    StopGrab();

    // Free the converted bitmap, if present.
    CSingleLock lockBmp( &m_bmpLock, TRUE );
    m_bitmapImage.Release();
    lockBmp.Unlock();

    // Free the grab result, if present.
    m_ptrGrabResult.Release();

    // Remove the event handlers that will be called when the feature changes.
    m_camera.DeregisterCameraEventHandler( this, "TriggerSource" );
    
    m_camera.DeregisterCameraEventHandler( this, "TriggerMode" );
    
    m_camera.DeregisterCameraEventHandler( this, "PixelFormat" );

    if (m_gain.IsValid())
    {
        // If we must use the alternative integer representation, we don't know the name of the node as it defined by the camera
        m_camera.DeregisterCameraEventHandler( this, m_gain.GetNode()->GetName() );
    }

    if (m_exposureTime.IsValid())
    {
        // If we must use the alternative integer representation, we don't know the name of the node as it defined by the camera
        m_camera.DeregisterCameraEventHandler( this, m_exposureTime.GetNode()->GetName() );
    }

    // Clear the pointers to the features we set manually in Open().
    m_exposureTime.Release();
    m_gain.Release();

    // Close the camera and free all ressources.
    // This will also unregister all 
    m_camera.DestroyDevice();
}


// Pylon::CImageEventHandler functions
void CGuiCamera::OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    CSingleLock lock( &m_MemberLock, TRUE );
    ++m_cntSkippedImages;

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}

// This function is called from the CInstantCamera grab thread when a new image is available.
// Perform your image processing here.
// After this, we'll convert the image to a RGB bitmap and inform the GUI about the new image.
// 
// NOTE: You must not access any UI objects, since this function is not called from the GUI thread.
//       To update the GUI we will post a message to the main window at the end of this function.
//       The GUI thread will process the message and update the GUI.
void CGuiCamera::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult)
{
	// The following line is commented out as this function will be called very often
	// filling up the debug output.
	//TRACE(_T("%s\n"), __FUNCTIONW__);

	// When overwriting the current CGrabResultPtr, the previous result will automatically be
	// released and reused by CInstantCamera.
	CSingleLock lock(&m_MemberLock, TRUE);
	m_ptrGrabResult = grabResult;
	lock.Unlock();

	// First check whether the smart pointer is valid.
	// Then call GrabSucceeded() on the CGrabResultData to test whether the grab resulut conatains
	// an sucessfully grabbed image.
	// In case of i.e. transmission errors the result may be invalid
	if (grabResult.IsValid() && grabResult->GrabSucceeded())
	{
		// Convert the processed image to a bmp so we can display it on the screen.
		// We must lock the bitmap, so we don't modify the pixels while the GUI thread is painting.
		CSingleLock lockBmp(&m_bmpLock, TRUE);
		m_bitmapImage.CopyImage(grabResult);
		lockBmp.Unlock();

		// This is where you would do image processing
		// and do other tasks.
		// --- Start of sample image processing --- (only works for 8-bit formats)
		if (m_invertImage && Pylon::BitDepth(grabResult->GetPixelType()) == 8)
		{
			size_t imageSize = Pylon::ComputeBufferSize(grabResult->GetPixelType(), grabResult->GetWidth(), grabResult->GetHeight(), grabResult->GetPaddingX());

			uint8_t* p = reinterpret_cast<uint8_t*>(grabResult->GetBuffer());
			uint8_t* const pEnd = p + (imageSize / sizeof(uint8_t));
			for (; p < pEnd; ++p)
			{
				*p = 255 - *p; //For demonstration purposes only, invert the image.
			}


			// Convert the processed image to a bmp so we can display it on the screen.
		// We must lock the bitmap, so we don't modify the pixels while the GUI thread is painting.
			CSingleLock lockBmp(&m_bmpLock, TRUE);
			m_bitmapImage.CopyImage(grabResult);
			lockBmp.Unlock();

			CSingleLock lock(&m_MemberLock, TRUE);
			++m_cntGrabbedImages;
			lock.Unlock();

		}

		// Check if the image1 should be rotated 90deg (2023.10 Chris implemented)
		if (m_rotateImage1 && Pylon::BitDepth(grabResult->GetPixelType()) == 8)
		{
			lockBmp.Lock();
			Pylon::CPylonImage tempImage;
			tempImage.CopyImage(grabResult);
			Pylon::CPylonImage rotatedImage = RotateRescale::rotate90(tempImage);
			m_bitmapImage.CopyImage(rotatedImage);  // Update the m_bitmapImage with the rotated image
			lockBmp.Unlock();

			CSingleLock lock(&m_MemberLock, TRUE);
			++m_cntGrabbedImages;
			lock.Unlock();
		}

		// Check if the image1 should be rotated 270deg (2023.10 Chris implemented)
		if (m_rotateImage2 && Pylon::BitDepth(grabResult->GetPixelType()) == 8)
		{
			lockBmp.Lock();
			Pylon::CPylonImage tempImage;
			tempImage.CopyImage(grabResult);
			Pylon::CPylonImage rotatedImage = RotateRescale::rotate270(tempImage);
			m_bitmapImage.CopyImage(rotatedImage);  // Update the m_bitmapImage with the rotated image
			lockBmp.Unlock();

			CSingleLock lock(&m_MemberLock, TRUE);
			++m_cntGrabbedImages;
			lock.Unlock();
		}
	}
	else
	{
		// If the grab result is invalid, we also mark the bitmap as invalid.
		CSingleLock lockBmp(&m_bmpLock, TRUE);
		m_bitmapImage.Release();
		lockBmp.Unlock();

		// The some TLs provide an error code why the grab failed.
		TRACE(_T("%s Grab failed. Error code. %i\n"), __FUNCTIONW__, (int)grabResult->GetErrorCode());

		CSingleLock lock(&m_MemberLock, TRUE);
		++m_cntGrabErrors;
		lock.Unlock();
	}

	// Tell the main window that there is a new image available so it can update the image window.
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	if (pWnd != NULL)
	{
		// You must use PostMessage here to separate the grab thread from the GUI thread.
		pWnd->PostMessage(WM_NEW_GRABRESULT, m_userHint);
	}

	// Prevent unused variable warning.
	UNUSED_ALWAYS(camera);
}

// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnAttach(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnAttached(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnDetach(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnDetached(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnDestroy(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnDestroyed(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnOpen(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s - '%s'\n"), __FUNCTIONW__, (LPCWSTR)CUtf82W( camera.GetDeviceInfo().GetFriendlyName().c_str() ));

    // Prevent unused variable warning.
    UNUSED( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnOpened(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // In this sample we configure only the trigger to start the acquisition.
    // Depending on your camera model, it may be called FrameStart or AcquisitionStart.
    if (!m_camera.TriggerSelector.TrySetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart))
    {
        m_camera.TriggerSelector.TrySetValue(Basler_UniversalCameraParams::TriggerSelector_AcquisitionStart);
    }

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnClose(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnClosed(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s - '%s'\n"), __FUNCTIONW__, (LPCWSTR)CUtf82W( camera.GetDeviceInfo().GetFriendlyName().c_str() ));

    // Prevent unused variable warning.
    UNUSED( camera );
}


// We'll reset the statistical values when the grab is about to start.
void CGuiCamera::OnGrabStart(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // This function may be called from another thread by InstantCamera while holding the camera lock.

    CSingleLock lock( &m_MemberLock, TRUE );

    // Reset statistics.
    m_cntGrabbedImages = 0;
    m_cntSkippedImages = 0;
    m_cntGrabErrors = 0;
    m_cntLastGrabbedImages = 0;

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnGrabStarted(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // This function may be called from another thread by InstantCamera while holding the camera lock.

    CWnd* pWnd = AfxGetApp()->GetMainWnd();
    if (pWnd != NULL)
    {
        // Update the start/stop buttons.
        // You must use PostMessage here to separate the thread from the GUI thread.
        pWnd->PostMessage( WM_GRAB_STATE_CHANGED, m_userHint, 1 );
    }

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnGrabStop(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // This function may be called from another thread by InstantCamera while holding the camera lock.

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// We'll set the last grabbed images counter to the current counter after the grab is finished.
// This will set the FPS value to 0.
void CGuiCamera::OnGrabStopped(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s Grabbed: %I64u; Errors: %I64u\n"), __FUNCTIONW__, m_cntGrabbedImages, m_cntGrabErrors);

    // This function may be called from another thread by InstantCamera while holding the camera lock.
    
    // Reset the FPS counter.
    m_cntLastGrabbedImages = m_cntGrabbedImages;

    CWnd* pWnd = AfxGetApp()->GetMainWnd();
    if (pWnd != NULL)
    {
        // Update the start/stop buttons.
        // You must use PostMessage here to separate the thread from the GUI thread.
        pWnd->PostMessage( WM_GRAB_STATE_CHANGED, m_userHint, 0 );
    }
    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CConfigurationEventHandler Implementation.
// See the documentation for Pylon::CConfigurationEventHandler information in the pylon C++ API documentation
// for more information when these are called.
void CGuiCamera::OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // This function may be called from another thread by InstantCamera while holding the camera lock.

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// This will be called when the camera has been removed/disconnected.
// We'll post a message to the GUI about this event and return.
// In response to this message, the GUI will be updated and the camera will be closed.
void CGuiCamera::OnCameraDeviceRemoved(Pylon::CInstantCamera& camera)
{
    TRACE(_T("%s\n"), __FUNCTIONW__);

    // This function will be called from another thread by InstantCamera while holding the camera lock.
    // You should not wait on any lock here. Instead relay the info to the message loop of the main window.
    CWnd* pWnd = AfxGetApp()->GetMainWnd();
    if (pWnd != NULL)
    {
        // Do a refresh to update the list of devices and the GUI.
        // You must use PostMessage here to separate the thread from the GUI thread.
        pWnd->PostMessage(WM_DEVICE_REMOVED, m_userHint);
    }

    // Prevent unused variable warning.
    UNUSED_ALWAYS( camera );
}


// Pylon::CCameraEventHandler Implementation.
// See the documentation for Pylon::CCameraEventHandler in the pylon C++ API documentation
// for more information when these are called.
// This will be called when a parameter we registered in Open() changes its attributes or value.
// We'll inform the GUI to update the controls.
void CGuiCamera::OnCameraEvent(Pylon::CInstantCamera& camera, intptr_t userProvidedId, GenApi::INode* pNode)
{
    if (pNode == NULL)
    {
        return;
    }

    // Uncomment the following line if you want to see which nodes are getting callbacks.
    // TRACE(_T("Node changed: %s\n"), (LPCWSTR)CUtf82W(pNode->GetName().c_str()));

    // Tell the main window that some camera features must be updated.
    CWnd* pWnd = AfxGetApp()->GetMainWnd();
    // When the application shuts down, the window may already be gone.
    if (pWnd != NULL)
    {
        // You must use PostMessage here to separate the grab thread from the GUI thread.
        pWnd->PostMessage(WM_NODES_UPDATED, m_userHint, reinterpret_cast<LPARAM>(pNode));
    }
}


// Grab a single image.
void CGuiCamera::SingleGrab()
{
    // Camera may have been disconnected.
    if (!m_camera.IsOpen() || m_camera.IsGrabbing())
    {
        return;
    }

    // Try set single frame mode if available
    m_camera.AcquisitionMode.TrySetValue(Basler_UniversalCameraParams::AcquisitionMode_SingleFrame);

    // Grab one image.
    // When the image is received, pylon will call the OnImageGrab() handler.
    m_camera.StartGrabbing(1, Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}


// Start a continuous grab on the camera.
void CGuiCamera::ContinuousGrab()
{
    // Camera may have been disconnected.
    if (!m_camera.IsOpen() || m_camera.IsGrabbing())
    {
        return;
    }

    // Try set continuous frame mode if available
    m_camera.AcquisitionMode.TrySetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);

    // Start grabbing until StopGrabbing() is called.
    m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}


// Stop the continuous grab on the camera.
void CGuiCamera::StopGrab()
{
    m_camera.StopGrabbing();
}


// Return the converted bitmap.
// Called by the GUI to display the image on the screen.
const Pylon::CPylonBitmapImage& CGuiCamera::GetBitmapImage() const
{
    // No need to protect this member as it will only be accessed from the GUI thread.
    return m_bitmapImage;
}


// Execute a software trigger.
void CGuiCamera::ExecuteSoftwareTrigger()
{
    if (!IsGrabbing())
    {
        return;
    }

    // Only wait if software trigger is currently turned on.
    if (m_camera.TriggerSource.GetValue() == Basler_UniversalCameraParams::TriggerSource_Software
        && m_camera.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On)
    {
        // If the camera is currently processing a previous trigger command,
        // it will silently discard trigger commands.
        // We wait until the camera is ready to process the next trigger.
        m_camera.WaitForFrameTriggerReady( 3000, Pylon::TimeoutHandling_ThrowException );
    }
    // Send trigger
    m_camera.ExecuteSoftwareTrigger();
}


// Turn our sample image processing on or off.
void CGuiCamera::SetInvertImage(bool enable)
{
    CSingleLock lock( &m_MemberLock, TRUE );
    m_invertImage = enable;
}

void CGuiCamera::SetRotateImage1(bool enable)
{
	CSingleLock lock(&m_MemberLock, TRUE);
	m_rotateImage1 = enable;
}

void CGuiCamera::SetRotateImage2(bool enable)
{
	CSingleLock lock(&m_MemberLock, TRUE);
	m_rotateImage2 = enable;
}

// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IIntegerEx& CGuiCamera::GetExposureTime()
{
    return m_exposureTime;
}


// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IIntegerEx& CGuiCamera::GetGain()
{
    return m_gain;
}


// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IEnumParameterT<Basler_UniversalCameraParams::PixelFormatEnums>& CGuiCamera::GetPixelFormat()
{
    return m_camera.PixelFormat;
}


// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IEnumParameterT<Basler_UniversalCameraParams::TriggerModeEnums>& CGuiCamera::GetTriggerMode()
{
    return m_camera.TriggerMode;
}


// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IEnumParameterT<Basler_UniversalCameraParams::TriggerSourceEnums>& CGuiCamera::GetTriggerSource()
{
    return m_camera.TriggerSource;
}

// This GUI needs to lock the bitmap image while painting it to the screen.
CSyncObject* CGuiCamera::GetBmpLock() const
{
    return &m_bmpLock;
}


// Returns statistical values for the GUI.
uint64_t CGuiCamera::GetGrabbedImages() const
{
    // We must protect this member as it will be accessed from the grab thread and the GUI thread.
    CSingleLock lock(&m_MemberLock, TRUE);
    return m_cntGrabbedImages;
}


// Returns the number of images grabbed since the last call to this function.
// This is used to calculate the FPS received.
uint64_t CGuiCamera::GetGrabbedImagesDiff()
{
    // We must protect these members as they will be accessed from the grab thread and the GUI thread.
    CSingleLock lock( &m_MemberLock, TRUE );
    uint64_t delta = m_cntGrabbedImages - m_cntLastGrabbedImages;
    m_cntLastGrabbedImages = m_cntGrabbedImages;
    return delta;
}


// Returns statistical values for the GUI.
uint64_t CGuiCamera::GetGrabErrors() const
{
    // We must protect this member as it will be accessed from the grab thread and the GUI thread.
    CSingleLock lock(&m_MemberLock, TRUE);
    return m_cntGrabErrors;
}


// Returns true if the device has been removed/disconnected.
bool CGuiCamera::IsCameraDeviceRemoved() const
{
    return m_camera.IsCameraDeviceRemoved();
}


// Returns true if the device is currently opened.
bool CGuiCamera::IsOpen() const
{
    return m_camera.IsOpen();
}


// Returns true if the device is currently grabbing.
bool CGuiCamera::IsGrabbing() const
{
    return m_camera.IsGrabbing();
}
