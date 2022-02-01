#pragma once


class CMenuSound
{
public:
    enum MENUSOUND_ID
    {
        SOUND_ID_SELECT = 0,
        SOUND_ID_OK,
        SOUND_ID_CANCEL,
        SOUND_ID_BAD,
        SOUND_ID_START,
        
        SOUND_ID_MAX,
    };
    
public:
    static void PlaySE(MENUSOUND_ID id);
    
    CMenuSound(void);
    ~CMenuSound(void);
    bool CallSound(int32 index);
    void CheckSoundLabelList(Rt2dMaestro* pMaestro);

private:
    static const char* m_apszSoundLabel[];
    static int32 m_anSoundTable[];
    int32 m_ansoundLabelID[SOUND_ID_MAX];
};