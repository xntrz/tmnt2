#include "MenuSound.hpp"

#include "Game/System/Sound/GameSound.hpp"


/*static*/ const char* CMenuSound::m_apszSoundLabel[] =
{
    "SoundStart",
    "SoundBad",
    "SoundCancel",
    "SoundOk",
    "SoundCursor",
};


/*static*/ int32 CMenuSound::m_anSoundTable[] =
{
    SDCODE_SE(0x1004),
    SDCODE_SE(0x1002),
    SDCODE_SE(0x1001),
    SDCODE_SE(0x1000),
    SDCODE_SE(0x1005),
};


/*static*/ void CMenuSound::PlaySE(MENUSOUND_ID id)
{
    CGameSound::PlaySE(m_anSoundTable[id]);
};


CMenuSound::CMenuSound(void)
{
    ;
};


CMenuSound::~CMenuSound(void)
{
    ;
};


bool CMenuSound::CallSound(int32 index)
{
    for (int32 i = 0; i < COUNT_OF(m_ansoundLabelID); ++i)
    {
        if (m_ansoundLabelID[i] == index)
        {
            CGameSound::PlaySE(m_anSoundTable[i]);
            return true;
        };
    };

    return false;
};


void CMenuSound::CheckSoundLabelList(Rt2dMaestro* pMaestro)
{
    for (int32 i = 0; i < COUNT_OF(m_ansoundLabelID); ++i)
    {
        int32 index = 0;
        if (Rt2dMaestroFindStringLabel(pMaestro, rt2dANIMLABELTYPEURL, m_apszSoundLabel[i], &index))
            m_ansoundLabelID[i] = index;
        else
            m_ansoundLabelID[i] = -1;
    };
};

