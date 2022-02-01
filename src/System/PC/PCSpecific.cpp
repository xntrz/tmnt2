#include "PCSpecific.hpp"
#include "PCFramework.hpp"


/*static*/ HINSTANCE CPCSpecific::m_hInstance = NULL;
/*static*/ HWND CPCSpecific::m_hWnd = NULL;


/*static*/ void CPCSpecific::SetFlipEnable(bool bEnable)
{
	CPCFramework::Instance().SetFlipEnable(bEnable);
};


/*static*/ bool CPCSpecific::SetVideomode(int32 iVideomodeIndex)
{
	return CPCFramework::Instance().SetVideomode(iVideomodeIndex);
};


/*static*/ bool CPCSpecific::SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth)
{
	return CPCFramework::Instance().SetVideomode(iWidth, iHeight, iDepth);
};


/*static*/ void CPCSpecific::GetVideomode(int32 iVideomodeIndex, int32& iWidth, int32& iHeight, int32& iDepth)
{
	CPCFramework::Instance().GetVideomode(iVideomodeIndex, iWidth, iHeight, iDepth);
};


/*static*/ const char* CPCSpecific::GetVideoDeviceName(void)
{
	return CPCFramework::Instance().GetVideoDeviceName();
};


/*static*/ int32 CPCSpecific::GetVideomodeNum(void)
{
	return CPCFramework::Instance().GetVideomodeNum();
};