#include "GameSound.hpp"


/*static*/ inline void CGameSound::AttachCamera(RwCamera* pCamera)
{
	m_pCamera = pCamera;
};


/*static*/ inline void CGameSound::DetachCamera(void)
{
	m_pCamera = nullptr;
};


/*static*/ inline CGameSound::MODE CGameSound::GetMode(void)
{
	return m_mode;
};


/*static*/ inline void CGameSound::SetBgmConfig(int32 value)
{
	m_nBgmConfig = convertToRawConfig(value);
};


/*static*/ inline int32 CGameSound::GetBgmConfig(void)
{
	return convertToVolume(m_nBgmConfig);
};


/*static*/ inline void CGameSound::SetSeConfig(int32 value)
{
	m_nSeConfig = convertToRawConfig(value);
};


/*static*/ inline int32 CGameSound::GetSeConfig(void)
{
	return convertToVolume(m_nSeConfig);
};


/*static*/ inline void CGameSound::SetVoiceConfig(int32 value)
{
	m_nVoiceConfig = convertToRawConfig(value);
};


/*static*/ inline int32 CGameSound::GetVoiceConfig(void)
{
	return convertToVolume(m_nVoiceConfig);
};


/*static*/ inline int32 CGameSound::GetSeRaw(void)
{
	return m_nSeConfig;
};


/*static*/ inline int32 CGameSound::GetBgmRaw(void)
{
	return m_nBgmConfig;
};


/*static*/ inline int32 CGameSound::GetVoiceRaw(void)
{
	return m_nVoiceConfig;
};