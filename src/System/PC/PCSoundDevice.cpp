#include "PCSoundDevice.hpp"
#include "PCSpecific.hpp"

#include "Sound/SdDrv.hpp"

#include "System/Common/File/Filename.hpp"
#include "System/Common/Configure.hpp"

#include "cri_adxcs.h"
#include "adx_dsound8.h"

#include <ks.h>
#include <ksmedia.h>
#include <adx_pc.h>


CPCSoundDevice::CPCSoundDevice(void)
: m_pDs(nullptr)
, m_pDsBuffer(nullptr)
, m_pSoundHeap(nullptr)
{
    ;
};


bool CPCSoundDevice::Initialize(void)
{
	if (FAILED(DirectSoundCreate8(0, &m_pDs, 0)))
		return false;

	if (FAILED(m_pDs->SetCooperativeLevel(CPCSpecific::m_hWnd, DSSCL_PRIORITY)))
		return false;
	
	DSBUFFERDESC DsBufferDesc;
	std::memset(&DsBufferDesc, 0x00, sizeof(DsBufferDesc));
	DsBufferDesc.dwSize = sizeof(DsBufferDesc);
	DsBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if (FAILED(m_pDs->CreateSoundBuffer(&DsBufferDesc, &m_pDsBuffer, 0)))
		return false;

	WAVEFORMATEX WaveFormatEx;
	std::memset(&WaveFormatEx, 0x00, sizeof(WaveFormatEx));
	WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormatEx.nChannels = 2;
	WaveFormatEx.nSamplesPerSec = 48000;
	WaveFormatEx.wBitsPerSample = 16;
	WaveFormatEx.nBlockAlign = 4;
	WaveFormatEx.nAvgBytesPerSec = (WaveFormatEx.nSamplesPerSec * WaveFormatEx.nBlockAlign);

	if (FAILED(m_pDsBuffer->SetFormat(&WaveFormatEx)))
	{
		if ((WaveFormatEx.nSamplesPerSec != 48000))
		{
			WaveFormatEx.nSamplesPerSec = 44100;
			WaveFormatEx.nAvgBytesPerSec = (WaveFormatEx.nSamplesPerSec * WaveFormatEx.nBlockAlign);

			if (FAILED(m_pDsBuffer->SetFormat(&WaveFormatEx)))
				return false;
		};

		return false;
	};

	ADXPC_SetupSoundDirectSound8(m_pDs);
	ADXPC_SetDsbCapsGlobalFocus(0);
	ADXPC_SetOutputStereo(1);

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