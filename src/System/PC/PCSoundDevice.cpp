#include "PCSoundDevice.hpp"
#include "PCSpecific.hpp"

#include "Sound/SdDrv.hpp"

#include "System/Common/File/Filename.hpp"
#include "System/Common/Configure.hpp"

#include "cri_adxt.h"
#include "adx_dsound8.h"

#include <ks.h>
#include <ksmedia.h>


CPCSoundDevice::CPCSoundDevice(void)
: m_pSoundHeap(nullptr)
, m_pDs(nullptr)
, m_pDsBuffer(nullptr)
{
    ;
};


bool CPCSoundDevice::Initialize(void)
{
    DWORD level = DSSCL_PRIORITY;

    if (FAILED(DirectSoundCreate8(0, &m_pDs, 0)))
        return false;

    if (FAILED(m_pDs->SetCooperativeLevel(CPCSpecific::m_hWnd, level)))
        return false;

    DSBUFFERDESC dsbd;
    std::memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    
    if (FAILED(m_pDs->CreateSoundBuffer(&dsbd, &m_pDsBuffer, 0)))
        return false;

    if (level == DSSCL_PRIORITY)
    {
        WORD numChannels = 2;
        
        WAVEFORMATEX wfex;
        std::memset(&wfex, 0, sizeof(wfex));
        wfex.wFormatTag = WAVE_FORMAT_PCM;
        wfex.nChannels = numChannels;
        wfex.nBlockAlign = (numChannels * 2);
        wfex.nSamplesPerSec = 48000;
        wfex.wBitsPerSample = 16;
        wfex.nAvgBytesPerSec = (wfex.nSamplesPerSec * wfex.nBlockAlign);

        if (FAILED(m_pDsBuffer->SetFormat(&wfex)) || (wfex.nSamplesPerSec != 48000))
        {
            wfex.nSamplesPerSec = 44100;
            wfex.nAvgBytesPerSec = (wfex.nSamplesPerSec * wfex.nBlockAlign);

            if (FAILED(m_pDsBuffer->SetFormat(&wfex)))
                return false;
        };
    };

    ADXPC_SetupSound(m_pDs);    
    ADXPC_SetDsbCapsGlobalFocus(FALSE);
    ADXPC_SetDsbCapsCtrl3D(FALSE);
    ADXPC_SetDsbCapsCtrlFx(FALSE); 
    ADXPC_SetDsbNumChannels(2);

    return true;
};


void CPCSoundDevice::Terminate(void)
{
    ADXPC_ShutdownSound();
    
    if (m_pDsBuffer)
    {
        m_pDsBuffer->Release();
        m_pDsBuffer = nullptr;
    };

    if (m_pDs)
    {
        m_pDs->Release();
        m_pDs = nullptr;
    };
};


bool CPCSoundDevice::InitializeLib(void)
{
    //
    //	Init path
    //
    char szPath[MAX_PATH];
    szPath[0] = '\0';
    
    const char* pszSdPath = nullptr;	
    if (CConfigure::CheckArgValue("afspath", &pszSdPath))
    {
        std::strcpy(szPath, pszSdPath);
        CFilename::ConvPathPlatform(szPath);		
    };

    if (szPath[0] == '\0')
        GetModulePath(szPath);

    //
    //	Init options
    //
    uint32 OptFlag = 0;

    if (CConfigure::CheckArg("novag"))
        OptFlag |= 0x1;
    
    if (CConfigure::CheckArg("novox"))
        OptFlag |= 0x2;

    if (CConfigure::CheckArg("noseq"))
        OptFlag |= 0x4;

    if (CConfigure::CheckArg("sdlog"))
        OptFlag |= 0x8;

    //
    //	Init heap
    //
    void* HeapPtr = nullptr;
    uint32 HeapSize = 0;

    //
    //	Init drv
    //
    return SdDrvInitialize(CPCSpecific::m_hWnd, szPath, HeapPtr, HeapSize, OptFlag);
};


void CPCSoundDevice::TerminateLib(void)
{
    SdDrvTerminate();

    if (m_pSoundHeap)
    {
        delete[] m_pSoundHeap;
        m_pSoundHeap = nullptr;
    };
};