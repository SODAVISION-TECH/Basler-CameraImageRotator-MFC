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

// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

// CGuiCamera is a thin wrapper of the Pylon::CInstantCamera class.
// It is used to handle camera and image event handlers and
// converts images from Pylon::GrabResult to a Pylon::CBitmapImage
// whichcan then be displayed on the screen.
class CGuiCamera : public CObject
    , public Pylon::CImageEventHandler             // Allows you to get notified about images received and grab errors.
    , public Pylon::CConfigurationEventHandler     // Allows you to get notified about device removal.
    , public Pylon::CCameraEventHandler            // Allows you to get notified about camera events and GenICam node changes.
{
public:
    // creation/destruction
    CGuiCamera();
    virtual ~CGuiCamera();

    // Attributes
    void SetUserHint(int userHint);
    int GetUserHint() const;

    void SetInvertImage( bool enable );
	void SetRotateImage1(bool enable);   //2023.10 Chris added
	void SetRotateImage2(bool enable);   //2023.10 Chris added

    const Pylon::CPylonBitmapImage& GetBitmapImage() const;

    bool IsCameraDeviceRemoved() const;
    bool IsOpen() const;
    bool IsGrabbing() const;

    // For GUI
    Pylon::IIntegerEx& GetExposureTime();
    Pylon::IIntegerEx& GetGain();
    Pylon::IEnumParameterT<Basler_UniversalCameraParams::PixelFormatEnums>& GetPixelFormat();
    Pylon::IEnumParameterT<Basler_UniversalCameraParams::TriggerModeEnums>& GetTriggerMode();
    Pylon::IEnumParameterT<Basler_UniversalCameraParams::TriggerSourceEnums>& GetTriggerSource();
    CSyncObject* GetBmpLock() const;

    // Statistics
    uint64_t GetGrabbedImages() const;
    uint64_t GetGrabbedImagesDiff();
    uint64_t GetGrabErrors() const;

    // operations
    void Open(const Pylon::CDeviceInfo& deviceInfo);
    void Close();

    void SingleGrab();
    void ContinuousGrab();
    void StopGrab();

    void ExecuteSoftwareTrigger();

    // Pylon::CImageEventHandler functions
    virtual void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages);
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult);

    // Pylon::CConfigurationEventHandler functions
    virtual void OnAttach(Pylon::CInstantCamera& camera);
    virtual void OnAttached(Pylon::CInstantCamera& camera);
    virtual void OnDetach(Pylon::CInstantCamera& camera);
    virtual void OnDetached(Pylon::CInstantCamera& camera);
    virtual void OnDestroy(Pylon::CInstantCamera& camera);
    virtual void OnDestroyed(Pylon::CInstantCamera& camera);
    virtual void OnOpen(Pylon::CInstantCamera& camera);
    virtual void OnOpened(Pylon::CInstantCamera& camera);
    virtual void OnClose(Pylon::CInstantCamera& camera);
    virtual void OnClosed(Pylon::CInstantCamera& camera);
    virtual void OnGrabStart(Pylon::CInstantCamera& camera);
    virtual void OnGrabStarted(Pylon::CInstantCamera& camera);
    virtual void OnGrabStop(Pylon::CInstantCamera& camera);
    virtual void OnGrabStopped(Pylon::CInstantCamera& camera);
    virtual void OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage);
    virtual void OnCameraDeviceRemoved(Pylon::CInstantCamera& camera);

    // Pylon::CCameraEventHandler function
    virtual void OnCameraEvent( Pylon::CInstantCamera& camera, intptr_t userProvidedId, GenApi::INode* pNode );

private:
    // Statistical values
    uint64_t m_cntGrabbedImages;
    uint64_t m_cntSkippedImages;
    uint64_t m_cntGrabErrors;
    uint64_t m_cntLastGrabbedImages;
    // Used to store the index of the camera.
    int m_userHint;
    // Will be set to toggle image processing on or off.
    bool m_invertImage;
	bool m_rotateImage1;
	bool m_rotateImage2;
    // Protects members.
    mutable CCriticalSection m_MemberLock;
    // Protects the converted bitmap.
    mutable CCriticalSection m_bmpLock;

private:
    // The pylon camera object.
    Pylon::CBaslerUniversalInstantCamera m_camera;
    // The grab result retrieved from the camera.
    Pylon::CGrabResultPtr m_ptrGrabResult;
    // The grab result as a windows DIB to be displayed on the screen.
    Pylon::CPylonBitmapImage m_bitmapImage;

    // Configurations to apply when starting single or continuous grab.
    Pylon::CAcquireSingleFrameConfiguration m_singleConfiguration;
    Pylon::CAcquireContinuousConfiguration m_continuousConfiguration;

    // Smart pointer to camera features
    Pylon::CIntegerParameter m_exposureTime;
    Pylon::CIntegerParameter m_gain;
};
