#include "VoiceManager.hpp"
#include "GameSound.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"


class CVoiceChooser
{
public:
    virtual void Initialize(void) {};
	virtual int32 GetVoiceID(PLAYERID::VALUE idPlayer) { return PLAYERID::VALUE(-1);  };

    static CVoiceChooser* Dummy(void)
    {
        static CVoiceChooser s_Dummy;
        return &s_Dummy;
    };
};


class CVoiceChooseTool
{
public:
    struct VOICELIST
    {
        int32 m_nVoiceID;
        PLAYERID::VALUE m_PlayerID;
    };
    
public:
    static void GetExistCharacter(int32* Count, PLAYERID::VALUE* idPlayerArray);
    static void MakeExistListFromVoiceList(int32* VoiceListSize, PLAYERID::VALUE idPlayer, VOICELIST* VoiceList);
    static void SetVoiceCharacter(PLAYERID::VALUE idPlayer);
    static PLAYERID::VALUE GetVoiceCharacter(void);
    static int32 GetVoiceID(int32* VoiceListSize, VOICELIST* VoiceList);

private:
    static PLAYERID::VALUE m_VoicePlayerID;
};


/*static*/ PLAYERID::VALUE CVoiceChooseTool::m_VoicePlayerID = PLAYERID::ID_LEO;


/*static*/ void CVoiceChooseTool::GetExistCharacter(int32* Count, PLAYERID::VALUE* idPlayerArray)
{
    int32 PlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < PlayerNum; ++i)
    {
        int32 PlayerChrNum = CGameProperty::Player(i).GetCharacterNum();
        for (int32 j = 0; j < PlayerChrNum; ++j)
            idPlayerArray[*Count++] = CGameProperty::Player(i).GetCharacterID(j);
    };
};


/*static*/ void CVoiceChooseTool::MakeExistListFromVoiceList(int32* VoiceListSize, PLAYERID::VALUE idPlayer, VOICELIST* VoiceList)
{
    PLAYERID::VALUE idExistPlayerArray[GAMETYPES::PLAYERS_MAX];
    int32 ExistPlayerNum = 0;

    if (idPlayer == -1)
    {
        GetExistCharacter(&ExistPlayerNum, idExistPlayerArray);
    }
    else
    {
        idExistPlayerArray[0] = idPlayer;
        ExistPlayerNum = 1;
    };

    //
    //  Math exist player id with voice list player id
    //  If player id in voice list is not exist - remove it from list
    //
    int32 VoiceListCnt = *VoiceListSize;
    for (int32 i = 0; i < VoiceListCnt; ++i)
    {
        VOICELIST* VoiceListRec = &VoiceList[i];
        bool PlayerExistInList = false;
        
        if (VoiceListRec->m_PlayerID != -1)
        {
            int32 ExistPlayerCnt = ExistPlayerNum;
            for (int32 j = 0; j < ExistPlayerCnt; ++j)
            {
                if (idExistPlayerArray[j] == VoiceListRec->m_PlayerID)
                    PlayerExistInList = true;
            };
        };

        if (!PlayerExistInList)
        {            
            for (int32 j = i; j < VoiceListCnt - 1; ++j)
                VoiceList[j] = VoiceList[j + 1];

            --VoiceListCnt;
        };
    };

    *VoiceListSize = VoiceListCnt;
};


/*static*/ void CVoiceChooseTool::SetVoiceCharacter(PLAYERID::VALUE idPlayer)
{
    m_VoicePlayerID = idPlayer;
};


/*static*/ PLAYERID::VALUE CVoiceChooseTool::GetVoiceCharacter(void)
{
    return m_VoicePlayerID;
};


/*static*/ int32 CVoiceChooseTool::GetVoiceID(int32* VoiceListSize, VOICELIST* VoiceList)
{
    int32 VoiceListCnt = *VoiceListSize;
    if (VoiceListCnt)
    {
        int32 ChosenVoice = Math::Rand() % VoiceListCnt;
        SetVoiceCharacter(VoiceList[ChosenVoice].m_PlayerID);
        return VoiceList[ChosenVoice].m_nVoiceID;
    };

    return -1;
};


#define CREATE_CHOOSER(No, ...)                                                             \
class CVoiceChooserGroup##No : public CVoiceChooser                                         \
{                                                                                           \
public:                                                                                     \
    virtual int32 GetVoiceID(PLAYERID::VALUE idPlayer) override                             \
    {                                                                                       \
        CVoiceChooseTool::VOICELIST VoiceList[] =  { ##__VA_ARGS__ };                       \
        int32 VoiceListSize = COUNT_OF(VoiceList);                                          \
        CVoiceChooseTool::MakeExistListFromVoiceList(&VoiceListSize, idPlayer, VoiceList);  \
        return CVoiceChooseTool::GetVoiceID(&VoiceListSize, VoiceList);                     \
    };                                                                                      \
                                                                                            \
    static CVoiceChooser* Instance(void)                                                    \
    {                                                                                       \
        static CVoiceChooserGroup##No s_VoiceChooserGroup##No;                              \
        return &s_VoiceChooserGroup##No;                                                    \
    };                                                                                      \
}


#define CREATE_CHOOSER_EX(No, VoiceCode, ...)                                       \
class CVoiceChooserGroupEx##No : public CVoiceChooser                               \
{                                                                                   \
public:                                                                             \
    virtual int32 GetVoiceID(PLAYERID::VALUE idPlayer) override                     \
    {                                                                               \
        PLAYERID::VALUE idExistPlayerArray[GAMETYPES::PLAYERS_MAX];                 \
        int32 ExistPlayerNum = 0;                                                   \
        PLAYERID::VALUE idCheckPlayerArray[] = ##__VA_ARGS__;                       \
        int32 CheckCnt = 0;                                                         \
        CVoiceChooseTool::GetExistCharacter(&ExistPlayerNum, idExistPlayerArray);   \
        int32 ExistPlayerCnt = ExistPlayerNum;                                      \
        for (int32 i = 0; i < ExistPlayerCnt; ++i)                                  \
        {                                                                           \
            if (idExistPlayerArray[i] == idCheckPlayerArray[i])                     \
                ++CheckCnt;                                                         \
        };                                                                          \
        CVoiceChooseTool::SetVoiceCharacter(PLAYERID::VALUE(-1));                   \
        if (CheckCnt == COUNT_OF(idCheckPlayerArray))                               \
            return VoiceCode;                                                       \
        else                                                                        \
            return -1;                                                              \
    };                                                                              \
                                                                                    \
    static CVoiceChooser* Instance(void)                                            \
    {                                                                               \
        static CVoiceChooserGroupEx##No s_VoiceChooserGroupEx##No;                  \
        return &s_VoiceChooserGroupEx##No;                                          \
    };                                                                              \
}


#define GET_CHOOSER_INSTANCE(No) CVoiceChooserGroup##No::Instance()


#define GET_CHOOSER_EX_INSTANCE(No) CVoiceChooserGroupEx##No::Instance()


CREATE_CHOOSER(
    1,
    { 0x5115, PLAYERID::ID_MIC },
    { 0x5116, PLAYERID::ID_MIC },
    { 0x5117, PLAYERID::ID_RAP },
    { 0x5115, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    2,
    { 0x5076, PLAYERID::ID_LEO },
    { 0x5077, PLAYERID::ID_LEO },
    { 0x5078, PLAYERID::ID_RAP },
    { 0x5079, PLAYERID::ID_RAP },
    { 0x507A, PLAYERID::ID_DON },
    { 0x507B, PLAYERID::ID_DON },
    { 0x507C, PLAYERID::ID_MIC },
    { 0x507D, PLAYERID::ID_MIC },
    { 0x507E, PLAYERID::ID_CAS },
    { 0x507F, PLAYERID::ID_CAS },
    { 0x5080, PLAYERID::ID_SPL },
    { 0x5081, PLAYERID::ID_SPL },
    { 0x5082, PLAYERID::ID_KAR },
    { 0x5083, PLAYERID::ID_KAR },
    { 0x5084, PLAYERID::ID_SLA },
    { 0x5085, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    3,
    { 0x5086, PLAYERID::ID_LEO },
    { 0x5087, PLAYERID::ID_LEO },
    { 0x5088, PLAYERID::ID_RAP },
    { 0x5089, PLAYERID::ID_RAP },
    { 0x508A, PLAYERID::ID_DON },
    { 0x508B, PLAYERID::ID_DON },
    { 0x508C, PLAYERID::ID_MIC },
    { 0x508D, PLAYERID::ID_MIC },
    { 0x508E, PLAYERID::ID_CAS },
    { 0x508F, PLAYERID::ID_CAS },
    { 0x5090, PLAYERID::ID_SPL },
    { 0x5091, PLAYERID::ID_SPL },
    { 0x5092, PLAYERID::ID_KAR },
    { 0x5093, PLAYERID::ID_KAR },
    { 0x5094, PLAYERID::ID_SLA },
    { 0x5095, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    4,
    { 0x50FE, PLAYERID::ID_DON },
    { 0x50FF, PLAYERID::ID_RAP },
    { 0x5100, PLAYERID::ID_LEO },
    { 0x5101, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    5,
    { 0x5102, PLAYERID::ID_RAP },
    { 0x5103, PLAYERID::ID_RAP },
    { 0x5104, PLAYERID::ID_RAP },
    { 0x5105, PLAYERID::ID_CAS },
    { 0x5106, PLAYERID::ID_CAS },
    { 0x5107, PLAYERID::ID_CAS }
);

//
//  TODO voice choose grp
//

CREATE_CHOOSER_EX(
    167,
    0x5001,
    { PLAYERID::ID_LEO }
);


CREATE_CHOOSER_EX(
    170,
    0x5004,
    { PLAYERID::ID_MIC, PLAYERID::ID_LEO, PLAYERID::ID_RAP, PLAYERID::ID_DON }
);


CREATE_CHOOSER_EX(
    173,
    0x5007,
    { PLAYERID::ID_MIC, PLAYERID::ID_LEO }
);


static CVoiceChooser* s_apVoiceChooserList[SEGROUPID::ID_MAX] = { nullptr };


/*static*/ void CVoiceManager::Initialize(void)
{
    for (int32 i = 0; i < COUNT_OF(s_apVoiceChooserList); ++i)
        s_apVoiceChooserList[i] = CVoiceChooser::Dummy();
};


/*static*/ void CVoiceManager::Terminate(void)
{
    ;
};


/*static*/ void CVoiceManager::Period(void)
{
    ;
};


/*static*/ bool CVoiceManager::SetVoice(SEGROUPID::VALUE idSeGroup, bool bIsHelpID)
{
    return SetVoice(idSeGroup, PLAYERID::VALUE(-1), bIsHelpID);
};


/*static*/ bool CVoiceManager::SetVoice(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer, bool bIsHelpID)
{
    ASSERT(idGroup >= 0 && idGroup < SEGROUPID::ID_MAX);

    CVoiceChooser* pVoiceChooser = s_apVoiceChooserList[idGroup];
    ASSERT(pVoiceChooser);    
    int32 nVoiceCode = pVoiceChooser->GetVoiceID(idPlayer);
    return PlayVoice(nVoiceCode, bIsHelpID);
};


/*static*/ bool CVoiceManager::PlayVoice(int32 nVoiceCode, bool bIsHelpID)
{
    if (nVoiceCode == -1 || (!bIsHelpID && CGameSound::IsVoicePlaying()))
    {
        CVoiceChooseTool::SetVoiceCharacter(PLAYERID::VALUE(-1));
        return false;
    }
    else
    {
        CGameSound::PlayVoice(nVoiceCode, CVoiceChooseTool::GetVoiceCharacter());
        CVoiceChooseTool::SetVoiceCharacter(PLAYERID::VALUE(-1));
        return true;
    };
};