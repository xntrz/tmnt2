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
    static PLAYERID::VALUE m_VoicePlayerID;

public:
    static void GetExistCharacter(int32* Count, PLAYERID::VALUE* idPlayerArray);
    static void MakeExistListFromVoiceList(int32* VoiceListSize, PLAYERID::VALUE idPlayer, VOICELIST* VoiceList);
    static int32 GetVoiceID(int32* VoiceListSize, VOICELIST* VoiceList);
};


/*static*/ PLAYERID::VALUE CVoiceChooseTool::m_VoicePlayerID = PLAYERID::ID_LEO;


/*static*/ void CVoiceChooseTool::GetExistCharacter(int32* Count, PLAYERID::VALUE* idPlayerArray)
{
    int32 PlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < PlayerNum; ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(i);
        
        int32 PlayerChrNum = pGamePlayer->GetCharacterNum();
        for (int32 j = 0; j < PlayerChrNum; ++j)
            idPlayerArray[(*Count)++] = pGamePlayer->GetCharacterID(j);

        pGamePlayer->Release();
    };
};


/*static*/ void CVoiceChooseTool::MakeExistListFromVoiceList(int32* VoiceListSize, PLAYERID::VALUE idPlayer, VOICELIST* VoiceList)
{
    /**
     *  TODO: review that sometimes it gives wrong voice id (got it on race map with SE group id 157 got kar voice instead don)
     */

    PLAYERID::VALUE idExistPlayerArray[GAMETYPES::PLAYERS_MAX];
    int32 ExistPlayerNum = 0;

    if (idPlayer == PLAYERID::ID_INVALID)
    {
        GetExistCharacter(&ExistPlayerNum, idExistPlayerArray);
    }
    else
    {
        idExistPlayerArray[0] = idPlayer;
        ExistPlayerNum = 1;
    };

    //
    //  Match exist player id with voice list player id
    //  If player id in voice list is not exist - remove it from list
    //
    int32 VoiceListCnt = *VoiceListSize;
    for (int32 i = 0; i < VoiceListCnt; ++i)
    {
        VOICELIST* VoiceListRec = &VoiceList[i];

        if (VoiceListRec->m_PlayerID == PLAYERID::ID_INVALID)
            continue;

        bool PlayerExistInList = false;
        
        for (int32 j = 0; j < ExistPlayerNum; ++j)
        {
            if (idExistPlayerArray[j] == VoiceListRec->m_PlayerID)
                PlayerExistInList = true;
        };

        if (!PlayerExistInList)
        {            
            for (int32 j = i; j < VoiceListCnt - 1; ++j)
                VoiceList[j] = VoiceList[j + 1];

            --i;
            --VoiceListCnt;
        };
    };

    *VoiceListSize = VoiceListCnt;
};


/*static*/ int32 CVoiceChooseTool::GetVoiceID(int32* VoiceListSize, VOICELIST* VoiceList)
{
    int32 VoiceListCnt = *VoiceListSize;
    
    if (!VoiceListCnt)
        return -1;
    
    int32 ChosenVoice = Math::Rand() % VoiceListCnt;
    
    m_VoicePlayerID = VoiceList[ChosenVoice].m_PlayerID;
    
    return VoiceList[ChosenVoice].m_nVoiceID;
};


#define CREATE_CHOOSER(No, ...)                                                                 \
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


#define CREATE_CHOOSER_EX(No, VoiceCode, ...)                                           \
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
            CVoiceChooseTool::m_VoicePlayerID = PLAYERID::ID_INVALID;                   \
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


CREATE_CHOOSER(
    001,
    { 0x4000, PLAYERID::ID_MIC },
    { 0x4001, PLAYERID::ID_RAP },
    { 0x4002, PLAYERID::ID_RAP },
    { 0x4003, PLAYERID::ID_MIC },
    { 0x4004, PLAYERID::ID_RAP },
    { 0x4005, PLAYERID::ID_LEO },
    { 0x4006, PLAYERID::ID_DON },
    { 0x4007, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    002,
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
    003,
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
    004,
    { 0x50FE, PLAYERID::ID_DON },
    { 0x50FF, PLAYERID::ID_RAP },
    { 0x5100, PLAYERID::ID_LEO },
    { 0x5101, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    005,
    { 0x5102, PLAYERID::ID_RAP },
    { 0x5103, PLAYERID::ID_RAP },
    { 0x5104, PLAYERID::ID_RAP },
    { 0x5105, PLAYERID::ID_CAS },
    { 0x5106, PLAYERID::ID_CAS },
    { 0x5107, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    006,
    { 0x5108, PLAYERID::ID_LEO },
    { 0x5109, PLAYERID::ID_LEO },
    { 0x510A, PLAYERID::ID_SLA },
    { 0x510B, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    007,
    { 0x510C, PLAYERID::ID_LEO },
    { 0x510D, PLAYERID::ID_LEO },
    { 0x510E, PLAYERID::ID_LEO },
    { 0x510F, PLAYERID::ID_LEO },
    { 0x5110, PLAYERID::ID_SLA },
    { 0x5111, PLAYERID::ID_SLA },
    { 0x5112, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    008,
    { 0x5113, PLAYERID::ID_DON },
    { 0x5114, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    009,
    { 0x5115, PLAYERID::ID_MIC },
    { 0x5116, PLAYERID::ID_MIC },
    { 0x5117, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    010,
    { 0x5118, PLAYERID::ID_DON },
    { 0x5119, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    011,
    { 0x511A, PLAYERID::ID_MIC },
    { 0x511B, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    012,
    { 0x511C, PLAYERID::ID_MIC },
    { 0x511D, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    013,
    { 0x511E, PLAYERID::ID_MIC },
    { 0x511F, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    014,
    { 0x5120, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    015,
    { 0x5121, PLAYERID::ID_MIC },
    { 0x5122, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    016,
    { 0x5123, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    017,
    { 0x5124, PLAYERID::ID_MIC },
    { 0x5125, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    018,
    { 0x5126, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    019,
    { 0x5127, PLAYERID::ID_RAP },
    { 0x5128, PLAYERID::ID_LEO },
    { 0x5129, PLAYERID::ID_MIC },
    { 0x512A, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    020,
    { 0x512B, PLAYERID::ID_MIC },
    { 0x512C, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    021,
    { 0x512D, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    022,
    { 0x512E, PLAYERID::ID_MIC },
    { 0x512F, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    023,
    { 0x5130, PLAYERID::ID_LEO },
    { 0x5131, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    024,
    { 0x5132, PLAYERID::ID_MIC },
    { 0x5133, PLAYERID::ID_MIC },
    { 0x5134, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    025,
    { 0x5135, PLAYERID::ID_DON },
    { 0x5136, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    026,
    { 0x6000, PLAYERID::ID_INVALID },
    { 0x6001, PLAYERID::ID_INVALID },
    { 0x6002, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    027,
    { 0x6003, PLAYERID::ID_LEO },
    { 0x6004, PLAYERID::ID_LEO },
    { 0x6005, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    028,
    { 0x6006, PLAYERID::ID_RAP },
    { 0x6007, PLAYERID::ID_RAP },
    { 0x6008, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    029,
    { 0x6009, PLAYERID::ID_MIC },
    { 0x600A, PLAYERID::ID_MIC },
    { 0x600B, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    030,
    { 0x600C, PLAYERID::ID_DON },
    { 0x600D, PLAYERID::ID_DON },
    { 0x600E, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    031,
    { 0x600F, PLAYERID::ID_CAS },
    { 0x6010, PLAYERID::ID_CAS },
    { 0x6011, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    032,
    { 0x6012, PLAYERID::ID_KAR },
    { 0x6013, PLAYERID::ID_KAR },
    { 0x6014, PLAYERID::ID_KAR }
);


CREATE_CHOOSER(
    033,
    { 0x6015, PLAYERID::ID_SPL },
    { 0x6016, PLAYERID::ID_SPL },
    { 0x6017, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    034,
    { 0x6018, PLAYERID::ID_SLA },
    { 0x6019, PLAYERID::ID_SLA },
    { 0x601A, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    035,
    { 0x601B, PLAYERID::ID_CAS },
    { 0x601C, PLAYERID::ID_DON },
    { 0x601D, PLAYERID::ID_KAR },
    { 0x601E, PLAYERID::ID_LEO },
    { 0x601F, PLAYERID::ID_MIC },
    { 0x6020, PLAYERID::ID_RAP },
    { 0x6021, PLAYERID::ID_SLA },
    { 0x6022, PLAYERID::ID_SPL },
    { 0x6023, PLAYERID::ID_CAS },
    { 0x6024, PLAYERID::ID_DON },
    { 0x6025, PLAYERID::ID_KAR },
    { 0x6026, PLAYERID::ID_LEO },
    { 0x6027, PLAYERID::ID_MIC },
    { 0x6028, PLAYERID::ID_RAP },
    { 0x6029, PLAYERID::ID_SLA },
    { 0x602A, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    036,
    { 0x602B, PLAYERID::ID_CAS },
    { 0x602C, PLAYERID::ID_DON },
    { 0x602D, PLAYERID::ID_KAR },
    { 0x602E, PLAYERID::ID_LEO },
    { 0x602F, PLAYERID::ID_MIC },
    { 0x6030, PLAYERID::ID_RAP },
    { 0x6031, PLAYERID::ID_SLA },
    { 0x6032, PLAYERID::ID_SPL },
    { 0x6033, PLAYERID::ID_CAS },
    { 0x6034, PLAYERID::ID_DON },
    { 0x6035, PLAYERID::ID_KAR },
    { 0x6036, PLAYERID::ID_LEO },
    { 0x6037, PLAYERID::ID_MIC },
    { 0x6038, PLAYERID::ID_RAP },
    { 0x6039, PLAYERID::ID_SLA },
    { 0x603A, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    037,
    { 0x603B, PLAYERID::ID_LEO },
    { 0x603C, PLAYERID::ID_RAP },
    { 0x603D, PLAYERID::ID_MIC },
    { 0x603E, PLAYERID::ID_DON },
    { 0x603F, PLAYERID::ID_CAS },
    { 0x6040, PLAYERID::ID_KAR },
    { 0x6041, PLAYERID::ID_SPL },
    { 0x6042, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    038,
    { 0x6043, PLAYERID::ID_CAS },
    { 0x6044, PLAYERID::ID_DON },
    { 0x6045, PLAYERID::ID_KAR },
    { 0x6046, PLAYERID::ID_LEO },
    { 0x6047, PLAYERID::ID_MIC },
    { 0x6048, PLAYERID::ID_RAP },
    { 0x6049, PLAYERID::ID_SLA },
    { 0x604A, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    039,
    { 0x604B, PLAYERID::ID_CAS },
    { 0x604C, PLAYERID::ID_DON },
    { 0x604D, PLAYERID::ID_KAR },
    { 0x604E, PLAYERID::ID_LEO },
    { 0x604F, PLAYERID::ID_MIC },
    { 0x6050, PLAYERID::ID_RAP },
    { 0x6051, PLAYERID::ID_SLA },
    { 0x6052, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    040,
    { 0x6053, PLAYERID::ID_CAS },
    { 0x6054, PLAYERID::ID_DON },
    { 0x6055, PLAYERID::ID_KAR },
    { 0x6056, PLAYERID::ID_LEO },
    { 0x6057, PLAYERID::ID_MIC },
    { 0x6058, PLAYERID::ID_RAP },
    { 0x6059, PLAYERID::ID_SLA },
    { 0x605A, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    041,
    { 0x605B, PLAYERID::ID_CAS },
    { 0x605C, PLAYERID::ID_DON },
    { 0x605D, PLAYERID::ID_KAR },
    { 0x605E, PLAYERID::ID_LEO },
    { 0x605F, PLAYERID::ID_MIC },
    { 0x6060, PLAYERID::ID_RAP },
    { 0x6061, PLAYERID::ID_SLA },
    { 0x6062, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    042,
    { 0x6063, PLAYERID::ID_CAS },
    { 0x6064, PLAYERID::ID_DON },
    { 0x6065, PLAYERID::ID_KAR },
    { 0x6066, PLAYERID::ID_LEO },
    { 0x6067, PLAYERID::ID_MIC },
    { 0x6068, PLAYERID::ID_RAP },
    { 0x6069, PLAYERID::ID_SLA },
    { 0x606A, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    043,
    { 0x606B, PLAYERID::ID_CAS },
    { 0x606C, PLAYERID::ID_DON },
    { 0x606D, PLAYERID::ID_KAR },
    { 0x606E, PLAYERID::ID_LEO },
    { 0x606F, PLAYERID::ID_MIC },
    { 0x6070, PLAYERID::ID_RAP },
    { 0x6071, PLAYERID::ID_SLA },
    { 0x6072, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    045,
    { 0x607B, PLAYERID::ID_LEO },
    { 0x607C, PLAYERID::ID_RAP },
    { 0x607D, PLAYERID::ID_MIC },
    { 0x607E, PLAYERID::ID_DON },
    { 0x607F, PLAYERID::ID_CAS },
    { 0x6080, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    046,
    { 0x6081, PLAYERID::ID_LEO },
    { 0x6082, PLAYERID::ID_RAP },
    { 0x6083, PLAYERID::ID_MIC },
    { 0x6084, PLAYERID::ID_DON },
    { 0x6085, PLAYERID::ID_CAS },
    { 0x6086, PLAYERID::ID_KAR },
    { 0x6087, PLAYERID::ID_SPL },
    { 0x6088, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    047,
    { 0x6089, PLAYERID::ID_LEO },
    { 0x608A, PLAYERID::ID_LEO },
    { 0x608B, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    048,
    { 0x608C, PLAYERID::ID_RAP },
    { 0x608D, PLAYERID::ID_RAP },
    { 0x608E, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    049,
    { 0x608F, PLAYERID::ID_MIC },
    { 0x6090, PLAYERID::ID_MIC },
    { 0x6091, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    050,
    { 0x6092, PLAYERID::ID_DON },
    { 0x6093, PLAYERID::ID_DON },
    { 0x6094, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    051,
    { 0x6095, PLAYERID::ID_CAS },
    { 0x6096, PLAYERID::ID_CAS },
    { 0x6097, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    052,
    { 0x6098, PLAYERID::ID_KAR },
    { 0x6099, PLAYERID::ID_KAR },
    { 0x609A, PLAYERID::ID_KAR }
);


CREATE_CHOOSER(
    053,
    { 0x609B, PLAYERID::ID_SPL },
    { 0x609C, PLAYERID::ID_SPL },
    { 0x609D, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    054,
    { 0x609E, PLAYERID::ID_SLA },
    { 0x609F, PLAYERID::ID_SLA },
    { 0x60A0, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    055,
    { 0x4008, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    056,
    { 0x4009, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    057,
    { 0x400A, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    058,
    { 0x400B, PLAYERID::ID_INVALID },
    { 0x400C, PLAYERID::ID_INVALID },
    { 0x400D, PLAYERID::ID_INVALID },
    { 0x400E, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    059,
    { 0x400F, PLAYERID::ID_INVALID },
    { 0x4013, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    060,
    { 0x4017, PLAYERID::ID_INVALID },
    { 0x401B, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    061,
    { 0x401F, PLAYERID::ID_INVALID },
    { 0x4020, PLAYERID::ID_INVALID },
    { 0x4021, PLAYERID::ID_INVALID },
    { 0x4022, PLAYERID::ID_RAP },
    { 0x4023, PLAYERID::ID_LEO },
    { 0x4024, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    062,
    { 0x4025, PLAYERID::ID_CAS },
    { 0x4026, PLAYERID::ID_CAS },
    { 0x4027, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    063,
    { 0x4028, PLAYERID::ID_INVALID },
    { 0x4029, PLAYERID::ID_INVALID },
    { 0x402A, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    064,
    { 0x402B, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    065,
    { 0x402C, PLAYERID::ID_MIC },
    { 0x402D, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    066,
    { 0x402E, PLAYERID::ID_INVALID },
    { 0x402F, PLAYERID::ID_INVALID },
    { 0x4030, PLAYERID::ID_INVALID },
    { 0x4031, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    068,
    { 0x4032, PLAYERID::ID_LEO },
    { 0x4033, PLAYERID::ID_DON },
    { 0x4034, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    069,
    { 0x4035, PLAYERID::ID_MIC },
    { 0x4036, PLAYERID::ID_LEO },
    { 0x4037, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    070,
    { 0x4038, PLAYERID::ID_INVALID },
    { 0x4039, PLAYERID::ID_INVALID },
    { 0x403A, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    071,
    { 0x403B, PLAYERID::ID_SLA },
    { 0x403C, PLAYERID::ID_SLA },
    { 0x403D, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    072,
    { 0x403E, PLAYERID::ID_DON },
    { 0x403F, PLAYERID::ID_MIC },
    { 0x4040, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    073,
    { 0x4041, PLAYERID::ID_LEO },
    { 0x4042, PLAYERID::ID_DON },
    { 0x4043, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    074,
    { 0x4044, PLAYERID::ID_INVALID },
    { 0x4045, PLAYERID::ID_INVALID },
    { 0x4046, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    075,
    { 0x4047, PLAYERID::ID_RAP },
    { 0x4048, PLAYERID::ID_LEO },
    { 0x4049, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    076,
    { 0x404A, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    077,
    { 0x4050, PLAYERID::ID_MIC },
    { 0x4051, PLAYERID::ID_LEO },
    { 0x4052, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    078,
    { 0x4053, PLAYERID::ID_RAP },
    { 0x4054, PLAYERID::ID_RAP },
    { 0x4055, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    079,
    { 0x4056, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    080,
    { 0x4057, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    081,
    { 0x4058, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    082,
    { 0x4059, PLAYERID::ID_LEO },
    { 0x405A, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    083,
    { 0x405B, PLAYERID::ID_DON },
    { 0x405C, PLAYERID::ID_DON },
    { 0x405D, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    084,
    { 0x405E, PLAYERID::ID_MIC },
    { 0x405F, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    085,
    { 0x4060, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    086,
    { 0x4061, PLAYERID::ID_DON },
    { 0x4062, PLAYERID::ID_DON },
    { 0x4063, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    087,
    { 0x4064, PLAYERID::ID_INVALID },
    { 0x4065, PLAYERID::ID_INVALID },
    { 0x4066, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    088,
    { 0x4067, PLAYERID::ID_LEO },
    { 0x4068, PLAYERID::ID_LEO },
    { 0x4069, PLAYERID::ID_LEO },
    { 0x406A, PLAYERID::ID_LEO },
    { 0x406B, PLAYERID::ID_RAP },
    { 0x406C, PLAYERID::ID_MIC },
    { 0x406D, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    089,
    { 0x406E, PLAYERID::ID_INVALID },
    { 0x406F, PLAYERID::ID_INVALID },
    { 0x4070, PLAYERID::ID_INVALID },
    { 0x4071, PLAYERID::ID_LEO },
    { 0x4072, PLAYERID::ID_LEO },
    { 0x4073, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    091,
    { 0x4074, PLAYERID::ID_INVALID },
    { 0x4075, PLAYERID::ID_INVALID },
    { 0x4076, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    092,
    { 0x4077, PLAYERID::ID_DON },
    { 0x4078, PLAYERID::ID_DON },
    { 0x4079, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    093,
    { 0x407A, PLAYERID::ID_INVALID },
    { 0x407B, PLAYERID::ID_INVALID },
    { 0x407C, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    094,
    { 0x407D, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    095,
    { 0x407E, PLAYERID::ID_LEO },
    { 0x407F, PLAYERID::ID_LEO },
    { 0x4080, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    096,
    { 0x4081, PLAYERID::ID_INVALID },
    { 0x4082, PLAYERID::ID_INVALID },
    { 0x4083, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    097,
    { 0x4084, PLAYERID::ID_MIC },
    { 0x4085, PLAYERID::ID_MIC },
    { 0x4086, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    098,
    { 0x4087, PLAYERID::ID_MIC },
    { 0x4088, PLAYERID::ID_MIC },
    { 0x4089, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    099,
    { 0x408A, PLAYERID::ID_RAP },
    { 0x408B, PLAYERID::ID_RAP },
    { 0x408C, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    100,
    { 0x408D, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    101,
    { 0x408E, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    102,
    { 0x408F, PLAYERID::ID_MIC },
    { 0x4090, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    103,
    { 0x4091, PLAYERID::ID_INVALID },
    { 0x4092, PLAYERID::ID_INVALID },
    { 0x4093, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    105,
    { 0x4094, PLAYERID::ID_INVALID },
    { 0x4095, PLAYERID::ID_INVALID },
    { 0x4096, PLAYERID::ID_INVALID },
    { 0x4097, PLAYERID::ID_RAP },
    { 0x4098, PLAYERID::ID_RAP },
    { 0x4099, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    107,
    { 0x409A, PLAYERID::ID_INVALID },
    { 0x409B, PLAYERID::ID_INVALID },
    { 0x409C, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    108,
    { 0x409D, PLAYERID::ID_INVALID },
    { 0x409E, PLAYERID::ID_INVALID },
    { 0x409F, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    109,
    { 0x40A0, PLAYERID::ID_INVALID },
    { 0x40A1, PLAYERID::ID_INVALID },
    { 0x40A2, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    110,
    { 0x40A3, PLAYERID::ID_INVALID },
    { 0x40A4, PLAYERID::ID_INVALID },
    { 0x40A5, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    111,
    { 0x40A6, PLAYERID::ID_DON },
    { 0x40A7, PLAYERID::ID_DON },
    { 0x40A8, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    112,
    { 0x40A9, PLAYERID::ID_INVALID },
    { 0x40AA, PLAYERID::ID_INVALID },
    { 0x40AB, PLAYERID::ID_INVALID },
    { 0x40AC, PLAYERID::ID_MIC },
    { 0x40AD, PLAYERID::ID_MIC },
    { 0x40AE, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    114,
    { 0x40AF, PLAYERID::ID_INVALID },
    { 0x40B0, PLAYERID::ID_INVALID },
    { 0x40B1, PLAYERID::ID_INVALID },
    { 0x40B2, PLAYERID::ID_DON },
    { 0x40B3, PLAYERID::ID_RAP },
    { 0x40B4, PLAYERID::ID_MIC },
    { 0x40B5, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    115,
    { 0x40B6, PLAYERID::ID_SPL },
    { 0x40B7, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    116,
    { 0x40B8, PLAYERID::ID_INVALID },
    { 0x40B9, PLAYERID::ID_INVALID },
    { 0x40BA, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    117,
    { 0x40BB, PLAYERID::ID_INVALID },
    { 0x40BC, PLAYERID::ID_INVALID },
    { 0x40BD, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    118,
    { 0x40BE, PLAYERID::ID_RAP },
    { 0x40BF, PLAYERID::ID_RAP },
    { 0x40C0, PLAYERID::ID_RAP },
    { 0x40C1, PLAYERID::ID_INVALID },
    { 0x40C2, PLAYERID::ID_INVALID },
    { 0x40C3, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    119,
    { 0x40C4, PLAYERID::ID_INVALID },
    { 0x40C5, PLAYERID::ID_INVALID },
    { 0x40C6, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    120,
    { 0x40C7, PLAYERID::ID_LEO },
    { 0x40C8, PLAYERID::ID_LEO },
    { 0x40C9, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    121,
    { 0x40CA, PLAYERID::ID_SLA },
    { 0x40CB, PLAYERID::ID_SLA },
    { 0x40CC, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    122,
    { 0x40CD, PLAYERID::ID_RAP },
    { 0x40CE, PLAYERID::ID_RAP },
    { 0x40CF, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    123,
    { 0x40D0, PLAYERID::ID_DON },
    { 0x40D1, PLAYERID::ID_DON },
    { 0x40D2, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    124,
    { 0x40D3, PLAYERID::ID_MIC },
    { 0x40D4, PLAYERID::ID_MIC },
    { 0x40D5, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    125,
    { 0x40D6, PLAYERID::ID_LEO },
    { 0x40D7, PLAYERID::ID_LEO },
    { 0x40D8, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    126,
    { 0x40D9, PLAYERID::ID_INVALID },
    { 0x40DA, PLAYERID::ID_INVALID },
    { 0x40DB, PLAYERID::ID_INVALID },
    { 0x40DC, PLAYERID::ID_RAP },
    { 0x40DD, PLAYERID::ID_RAP },
    { 0x40DE, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    128,
    { 0x40DF, PLAYERID::ID_LEO },
    { 0x40E0, PLAYERID::ID_RAP },
    { 0x40E1, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    129,
    { 0x40E2, PLAYERID::ID_CAS },
    { 0x40E3, PLAYERID::ID_CAS },
    { 0x40E4, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    130,
    { 0x40E5, PLAYERID::ID_INVALID },
    { 0x40E6, PLAYERID::ID_INVALID },
    { 0x40E7, PLAYERID::ID_INVALID },
    { 0x40E8, PLAYERID::ID_LEO },
    { 0x40E9, PLAYERID::ID_LEO },
    { 0x40EA, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    132,
    { 0x40EB, PLAYERID::ID_KAR },
    { 0x40EC, PLAYERID::ID_KAR },
    { 0x40ED, PLAYERID::ID_KAR }
);


CREATE_CHOOSER(
    133,
    { 0x40EE, PLAYERID::ID_CAS },
    { 0x40EF, PLAYERID::ID_CAS }
);


CREATE_CHOOSER(
    134,
    { 0x40F0, PLAYERID::ID_INVALID },
    { 0x40F1, PLAYERID::ID_INVALID },
    { 0x40F2, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    135,
    { 0x40F3, PLAYERID::ID_INVALID },
    { 0x40F4, PLAYERID::ID_INVALID },
    { 0x40F5, PLAYERID::ID_INVALID },
    { 0x40F6, PLAYERID::ID_RAP },
    { 0x40F7, PLAYERID::ID_LEO },
    { 0x40F8, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    137,
    { 0x40F9, PLAYERID::ID_DON },
    { 0x40FA, PLAYERID::ID_DON },
    { 0x40FB, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    138,
    { 0x40FC, PLAYERID::ID_INVALID },
    { 0x40FD, PLAYERID::ID_INVALID },
    { 0x40FE, PLAYERID::ID_INVALID },
    { 0x40FF, PLAYERID::ID_LEO },
    { 0x4100, PLAYERID::ID_LEO },
    { 0x4101, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    143,
    { 0x4102, PLAYERID::ID_LEO },
    { 0x4103, PLAYERID::ID_LEO },
    { 0x4104, PLAYERID::ID_LEO },
    { 0x4105, PLAYERID::ID_INVALID },
    { 0x4106, PLAYERID::ID_INVALID },
    { 0x4107, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    144,
    { 0x4108, PLAYERID::ID_MIC },
    { 0x4109, PLAYERID::ID_MIC },
    { 0x410A, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    146,
    { 0x410E, PLAYERID::ID_INVALID },
    { 0x410F, PLAYERID::ID_INVALID },
    { 0x4110, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    147,
    { 0x4111, PLAYERID::ID_INVALID },
    { 0x4112, PLAYERID::ID_INVALID },
    { 0x4113, PLAYERID::ID_INVALID },
    { 0x4114, PLAYERID::ID_MIC },
    { 0x4115, PLAYERID::ID_MIC },
    { 0x4116, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    149,
    { 0x4117, PLAYERID::ID_INVALID },
    { 0x4118, PLAYERID::ID_INVALID },
    { 0x4119, PLAYERID::ID_INVALID },
    { 0x411A, PLAYERID::ID_INVALID },
    { 0x411B, PLAYERID::ID_INVALID },
    { 0x411C, PLAYERID::ID_INVALID },
    { 0x411D, PLAYERID::ID_RAP },
    { 0x411E, PLAYERID::ID_RAP },
    { 0x411F, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    150,
    { 0x4120, PLAYERID::ID_INVALID },
    { 0x4121, PLAYERID::ID_INVALID },
    { 0x4122, PLAYERID::ID_INVALID },
    { 0x4123, PLAYERID::ID_LEO },
    { 0x4124, PLAYERID::ID_LEO },
    { 0x4125, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    151,
    { 0x4126, PLAYERID::ID_DON },
    { 0x4127, PLAYERID::ID_DON },
    { 0x4128, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    152,
    { 0x4129, PLAYERID::ID_LEO },
    { 0x412A, PLAYERID::ID_LEO },
    { 0x412B, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    153,
    { 0x412C, PLAYERID::ID_INVALID },
    { 0x412D, PLAYERID::ID_INVALID },
    { 0x412E, PLAYERID::ID_INVALID },
    { 0x412F, PLAYERID::ID_INVALID },
    { 0x4130, PLAYERID::ID_INVALID },
    { 0x4131, PLAYERID::ID_INVALID },
    { 0x4132, PLAYERID::ID_RAP },
    { 0x4133, PLAYERID::ID_RAP },
    { 0x4134, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    154,
    { 0x4135, PLAYERID::ID_DON },
    { 0x4136, PLAYERID::ID_DON },
    { 0x4137, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    155,
    { 0x4138, PLAYERID::ID_MIC },
    { 0x4139, PLAYERID::ID_MIC },
    { 0x413A, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    156,
    { 0x413B, PLAYERID::ID_INVALID },
    { 0x413C, PLAYERID::ID_INVALID },
    { 0x413D, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    157,
    { 0x5096, PLAYERID::ID_CAS },
    { 0x5097, PLAYERID::ID_DON },
    { 0x5098, PLAYERID::ID_KAR },
    { 0x5099, PLAYERID::ID_LEO },
    { 0x509A, PLAYERID::ID_MIC },
    { 0x509B, PLAYERID::ID_RAP },
    { 0x509C, PLAYERID::ID_SLA },
    { 0x509D, PLAYERID::ID_SPL },
    { 0x509E, PLAYERID::ID_CAS },
    { 0x509F, PLAYERID::ID_DON },
    { 0x50A0, PLAYERID::ID_KAR },
    { 0x50A1, PLAYERID::ID_LEO },
    { 0x50A2, PLAYERID::ID_MIC },
    { 0x50A3, PLAYERID::ID_RAP },
    { 0x50A4, PLAYERID::ID_SLA },
    { 0x50A5, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    158,
    { 0x50A6, PLAYERID::ID_LEO },
    { 0x50A7, PLAYERID::ID_RAP },
    { 0x50A8, PLAYERID::ID_MIC },
    { 0x50A9, PLAYERID::ID_DON },
    { 0x50AA, PLAYERID::ID_CAS },
    { 0x50AB, PLAYERID::ID_KAR },
    { 0x50AC, PLAYERID::ID_SPL },
    { 0x50AD, PLAYERID::ID_SLA }
);


CREATE_CHOOSER(
    159,
    { 0x50AE, PLAYERID::ID_CAS },
    { 0x50AF, PLAYERID::ID_DON },
    { 0x50B0, PLAYERID::ID_KAR },
    { 0x50B1, PLAYERID::ID_LEO },
    { 0x50B2, PLAYERID::ID_MIC },
    { 0x50B3, PLAYERID::ID_RAP },
    { 0x50B4, PLAYERID::ID_SLA },
    { 0x50B5, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    160,
    { 0x50B6, PLAYERID::ID_CAS },
    { 0x50B7, PLAYERID::ID_DON },
    { 0x50B8, PLAYERID::ID_KAR },
    { 0x50B9, PLAYERID::ID_LEO },
    { 0x50BA, PLAYERID::ID_MIC },
    { 0x50BB, PLAYERID::ID_RAP },
    { 0x50BC, PLAYERID::ID_SLA },
    { 0x50BD, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    161,
    { 0x50BE, PLAYERID::ID_CAS },
    { 0x50BF, PLAYERID::ID_DON },
    { 0x50C0, PLAYERID::ID_KAR },
    { 0x50C1, PLAYERID::ID_LEO },
    { 0x50C2, PLAYERID::ID_MIC },
    { 0x50C3, PLAYERID::ID_RAP },
    { 0x50C4, PLAYERID::ID_SLA },
    { 0x50C5, PLAYERID::ID_SPL },
    { 0x50C6, PLAYERID::ID_CAS },
    { 0x50C7, PLAYERID::ID_DON },
    { 0x50C8, PLAYERID::ID_KAR },
    { 0x50C9, PLAYERID::ID_LEO },
    { 0x50CA, PLAYERID::ID_MIC },
    { 0x50CB, PLAYERID::ID_RAP },
    { 0x50CC, PLAYERID::ID_SLA },
    { 0x50CD, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    162,
    { 0x50CE, PLAYERID::ID_CAS },
    { 0x50CF, PLAYERID::ID_DON },
    { 0x50D0, PLAYERID::ID_KAR },
    { 0x50D1, PLAYERID::ID_LEO },
    { 0x50D2, PLAYERID::ID_MIC },
    { 0x50D3, PLAYERID::ID_RAP },
    { 0x50D4, PLAYERID::ID_SLA },
    { 0x50D5, PLAYERID::ID_SPL },
    { 0x50D6, PLAYERID::ID_CAS },
    { 0x50D7, PLAYERID::ID_DON },
    { 0x50D8, PLAYERID::ID_KAR },
    { 0x50D9, PLAYERID::ID_LEO },
    { 0x50DA, PLAYERID::ID_MIC },
    { 0x50DB, PLAYERID::ID_RAP },
    { 0x50DC, PLAYERID::ID_SLA },
    { 0x50DD, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    163,
    { 0x50DE, PLAYERID::ID_CAS },
    { 0x50DF, PLAYERID::ID_DON },
    { 0x50E0, PLAYERID::ID_KAR },
    { 0x50E1, PLAYERID::ID_LEO },
    { 0x50E2, PLAYERID::ID_MIC },
    { 0x50E3, PLAYERID::ID_RAP },
    { 0x50E4, PLAYERID::ID_SLA },
    { 0x50E5, PLAYERID::ID_SPL },
    { 0x50E6, PLAYERID::ID_CAS },
    { 0x50E7, PLAYERID::ID_DON },
    { 0x50E8, PLAYERID::ID_KAR },
    { 0x50E9, PLAYERID::ID_LEO },
    { 0x50EA, PLAYERID::ID_MIC },
    { 0x50EB, PLAYERID::ID_RAP },
    { 0x50EC, PLAYERID::ID_SLA },
    { 0x50ED, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    164,
    { 0x0, PLAYERID::ID_CAS },
    { 0x50EE, PLAYERID::ID_DON },
    { 0x0, PLAYERID::ID_KAR },
    { 0x50EF, PLAYERID::ID_LEO },
    { 0x50F0, PLAYERID::ID_MIC },
    { 0x50F1, PLAYERID::ID_RAP },
    { 0x0, PLAYERID::ID_SLA },
    { 0x0, PLAYERID::ID_SPL },
    { 0x0, PLAYERID::ID_CAS },
    { 0x50F2, PLAYERID::ID_DON },
    { 0x0, PLAYERID::ID_KAR },
    { 0x50F3, PLAYERID::ID_LEO },
    { 0x50F4, PLAYERID::ID_MIC },
    { 0x50F5, PLAYERID::ID_RAP },
    { 0x0, PLAYERID::ID_SLA },
    { 0x0, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    165,
    { 0x0, PLAYERID::ID_CAS },
    { 0x50F6, PLAYERID::ID_DON },
    { 0x0, PLAYERID::ID_KAR },
    { 0x50F7, PLAYERID::ID_LEO },
    { 0x50F8, PLAYERID::ID_MIC },
    { 0x50F9, PLAYERID::ID_RAP },
    { 0x0, PLAYERID::ID_SLA },
    { 0x0, PLAYERID::ID_SPL },
    { 0x0, PLAYERID::ID_CAS },
    { 0x50FA, PLAYERID::ID_DON },
    { 0x0, PLAYERID::ID_KAR },
    { 0x50FB, PLAYERID::ID_LEO },
    { 0x50FC, PLAYERID::ID_MIC },
    { 0x50FD, PLAYERID::ID_RAP },
    { 0x0, PLAYERID::ID_SLA },
    { 0x0, PLAYERID::ID_SPL }
);


CREATE_CHOOSER(
    166,
    { 0x5000, PLAYERID::ID_LEO }
);


CREATE_CHOOSER_EX(
    167,
    0x5001,
    { PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    168,
    { 0x5002, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    169,
    { 0x5003, PLAYERID::ID_DON }
);


CREATE_CHOOSER_EX(
    170,
    0x5004,
    { PLAYERID::ID_MIC, PLAYERID::ID_LEO, PLAYERID::ID_RAP, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    171,
    { 0x5005, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    172,
    { 0x5006, PLAYERID::ID_MIC }
);


CREATE_CHOOSER_EX(
    173,
    0x5007,
    { PLAYERID::ID_MIC, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    174,
    { 0x5008, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    175,
    { 0x5009, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    176,
    { 0x500A, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    177,
    { 0x500B, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    178,
    { 0x500C, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    179,
    { 0x500D, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    180,
    { 0x500E, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    181,
    { 0x500F, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    182,
    { 0x5010, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    183,
    { 0x5011, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    184,
    { 0x5012, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    185,
    { 0x5013, PLAYERID::ID_INVALID },
    { 0x5014, PLAYERID::ID_MIC },
    { 0x5015, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    187,
    { 0x501A, PLAYERID::ID_DON },
    { 0x501B, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    188,
    { 0x501C, PLAYERID::ID_LEO },
    { 0x501D, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    189,
    { 0x501E, PLAYERID::ID_INVALID },
    { 0x501F, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    190,
    { 0x5020, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    191,
    { 0x5021, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    192,
    { 0x5022, PLAYERID::ID_MIC },
    { 0x5023, PLAYERID::ID_RAP },
    { 0x5024, PLAYERID::ID_LEO },
    { 0x5025, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    193,
    { 0x5026, PLAYERID::ID_RAP },
    { 0x5027, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    194,
    { 0x5028, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    195,
    { 0x5029, PLAYERID::ID_MIC },
    { 0x502A, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    196,
    { 0x502B, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    197,
    { 0x502C, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    198,
    { 0x502D, PLAYERID::ID_LEO },
    { 0x502E, PLAYERID::ID_MIC },
    { 0x502F, PLAYERID::ID_RAP },
    { 0x5030, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    199,
    { 0x5031, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    200,
    { 0x5033, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    201,
    { 0x5034, PLAYERID::ID_DON },
    { 0x5035, PLAYERID::ID_RAP },
    { 0x5036, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    202,
    { 0x5037, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    203,
    { 0x5038, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    204,
    { 0x503B, PLAYERID::ID_LEO },
    { 0x503C, PLAYERID::ID_RAP },
    { 0x503D, PLAYERID::ID_MIC },
    { 0x503E, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    205,
    { 0x503F, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    206,
    { 0x5040, PLAYERID::ID_INVALID },
    { 0x5041, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    208,
    { 0x5044, PLAYERID::ID_DON },
    { 0x5045, PLAYERID::ID_MIC }
);


CREATE_CHOOSER(
    209,
    { 0x5046, PLAYERID::ID_MIC },
    { 0x5047, PLAYERID::ID_RAP }
);


CREATE_CHOOSER(
    210,
    { 0x5048, PLAYERID::ID_MIC },
    { 0x5049, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    211,
    { 0x504A, PLAYERID::ID_LEO },
    { 0x504B, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    212,
    { 0x504C, PLAYERID::ID_RAP },
    { 0x504D, PLAYERID::ID_INVALID },
    { 0x504E, PLAYERID::ID_INVALID }
);


CREATE_CHOOSER(
    213,
    { 0x504F, PLAYERID::ID_LEO },
    { 0x5050, PLAYERID::ID_RAP },
    { 0x5051, PLAYERID::ID_MIC },
    { 0x5052, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    214,
    { 0x5053, PLAYERID::ID_LEO },
    { 0x5054, PLAYERID::ID_RAP },
    { 0x5055, PLAYERID::ID_MIC },
    { 0x5056, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    215,
    { 0x5057, PLAYERID::ID_LEO },
    { 0x5058, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    216,
    { 0x5059, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    218,
    { 0x505B, PLAYERID::ID_LEO },
    { 0x505C, PLAYERID::ID_RAP },
    { 0x505D, PLAYERID::ID_MIC },
    { 0x505E, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    219,
    { 0x505F, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    220,
    { 0x5060, PLAYERID::ID_MIC },
    { 0x5061, PLAYERID::ID_RAP },
    { 0x5062, PLAYERID::ID_DON },
    { 0x5063, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    221,
    { 0x5064, PLAYERID::ID_LEO },
    { 0x5065, PLAYERID::ID_RAP },
    { 0x5066, PLAYERID::ID_DON },
    { 0x5067, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    222,
    { 0x5068, PLAYERID::ID_RAP },
    { 0x5069, PLAYERID::ID_MIC },
    { 0x506A, PLAYERID::ID_LEO },
    { 0x506B, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    223,
    { 0x506C, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    224,
    { 0x506D, PLAYERID::ID_RAP },
    { 0x506E, PLAYERID::ID_LEO }
);


CREATE_CHOOSER(
    225,
    { 0x506F, PLAYERID::ID_LEO },
    { 0x5070, PLAYERID::ID_RAP },
    { 0x5071, PLAYERID::ID_DON }
);


CREATE_CHOOSER(
    226,
    { 0x5072, PLAYERID::ID_MIC },
    { 0x5073, PLAYERID::ID_DON },
    { 0x5074, PLAYERID::ID_DON },
    { 0x5075, PLAYERID::ID_LEO }
);


static CVoiceChooser* s_apVoiceChooserList[SEGROUPID::ID_ONLYVOX_MAX] = { nullptr };


/*static*/ void CVoiceManager::Initialize(void)
{
    s_apVoiceChooserList[0] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[1] = CVoiceChooserGroup001::Instance();
    s_apVoiceChooserList[2] = CVoiceChooserGroup002::Instance();
    s_apVoiceChooserList[3] = CVoiceChooserGroup003::Instance();
    s_apVoiceChooserList[4] = CVoiceChooserGroup004::Instance();
    s_apVoiceChooserList[5] = CVoiceChooserGroup005::Instance();
    s_apVoiceChooserList[6] = CVoiceChooserGroup006::Instance();
    s_apVoiceChooserList[7] = CVoiceChooserGroup007::Instance();
    s_apVoiceChooserList[8] = CVoiceChooserGroup008::Instance();
    s_apVoiceChooserList[9] = CVoiceChooserGroup009::Instance();
    s_apVoiceChooserList[10] = CVoiceChooserGroup010::Instance();
    s_apVoiceChooserList[11] = CVoiceChooserGroup011::Instance();
    s_apVoiceChooserList[12] = CVoiceChooserGroup012::Instance();
    s_apVoiceChooserList[13] = CVoiceChooserGroup013::Instance();
    s_apVoiceChooserList[14] = CVoiceChooserGroup014::Instance();
    s_apVoiceChooserList[15] = CVoiceChooserGroup015::Instance();
    s_apVoiceChooserList[16] = CVoiceChooserGroup016::Instance();
    s_apVoiceChooserList[17] = CVoiceChooserGroup017::Instance();
    s_apVoiceChooserList[18] = CVoiceChooserGroup018::Instance();
    s_apVoiceChooserList[19] = CVoiceChooserGroup019::Instance();
    s_apVoiceChooserList[20] = CVoiceChooserGroup020::Instance();
    s_apVoiceChooserList[21] = CVoiceChooserGroup021::Instance();
    s_apVoiceChooserList[22] = CVoiceChooserGroup022::Instance();
    s_apVoiceChooserList[23] = CVoiceChooserGroup023::Instance();
    s_apVoiceChooserList[24] = CVoiceChooserGroup024::Instance();
    s_apVoiceChooserList[25] = CVoiceChooserGroup025::Instance();
    s_apVoiceChooserList[26] = CVoiceChooserGroup026::Instance();
    s_apVoiceChooserList[27] = CVoiceChooserGroup027::Instance();
    s_apVoiceChooserList[28] = CVoiceChooserGroup028::Instance();
    s_apVoiceChooserList[29] = CVoiceChooserGroup029::Instance();
    s_apVoiceChooserList[30] = CVoiceChooserGroup030::Instance();
    s_apVoiceChooserList[31] = CVoiceChooserGroup031::Instance();
    s_apVoiceChooserList[32] = CVoiceChooserGroup032::Instance();
    s_apVoiceChooserList[33] = CVoiceChooserGroup033::Instance();
    s_apVoiceChooserList[34] = CVoiceChooserGroup034::Instance();
    s_apVoiceChooserList[35] = CVoiceChooserGroup035::Instance();
    s_apVoiceChooserList[36] = CVoiceChooserGroup036::Instance();
    s_apVoiceChooserList[37] = CVoiceChooserGroup037::Instance();
    s_apVoiceChooserList[38] = CVoiceChooserGroup038::Instance();
    s_apVoiceChooserList[39] = CVoiceChooserGroup039::Instance();
    s_apVoiceChooserList[40] = CVoiceChooserGroup040::Instance();
    s_apVoiceChooserList[41] = CVoiceChooserGroup041::Instance();
    s_apVoiceChooserList[42] = CVoiceChooserGroup042::Instance();
    s_apVoiceChooserList[43] = CVoiceChooserGroup043::Instance();
    s_apVoiceChooserList[44] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[45] = CVoiceChooserGroup045::Instance();
    s_apVoiceChooserList[46] = CVoiceChooserGroup046::Instance();
    s_apVoiceChooserList[47] = CVoiceChooserGroup047::Instance();
    s_apVoiceChooserList[48] = CVoiceChooserGroup048::Instance();
    s_apVoiceChooserList[49] = CVoiceChooserGroup049::Instance();
    s_apVoiceChooserList[50] = CVoiceChooserGroup050::Instance();
    s_apVoiceChooserList[51] = CVoiceChooserGroup051::Instance();
    s_apVoiceChooserList[52] = CVoiceChooserGroup052::Instance();
    s_apVoiceChooserList[53] = CVoiceChooserGroup053::Instance();
    s_apVoiceChooserList[54] = CVoiceChooserGroup054::Instance();
    s_apVoiceChooserList[55] = CVoiceChooserGroup055::Instance();
    s_apVoiceChooserList[56] = CVoiceChooserGroup056::Instance();
    s_apVoiceChooserList[57] = CVoiceChooserGroup057::Instance();
    s_apVoiceChooserList[58] = CVoiceChooserGroup058::Instance();
    s_apVoiceChooserList[59] = CVoiceChooserGroup059::Instance();
    s_apVoiceChooserList[60] = CVoiceChooserGroup060::Instance();
    s_apVoiceChooserList[61] = CVoiceChooserGroup061::Instance();
    s_apVoiceChooserList[62] = CVoiceChooserGroup062::Instance();
    s_apVoiceChooserList[63] = CVoiceChooserGroup063::Instance();
    s_apVoiceChooserList[64] = CVoiceChooserGroup064::Instance();
    s_apVoiceChooserList[65] = CVoiceChooserGroup065::Instance();
    s_apVoiceChooserList[66] = CVoiceChooserGroup066::Instance();
    s_apVoiceChooserList[67] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[68] = CVoiceChooserGroup068::Instance();
    s_apVoiceChooserList[69] = CVoiceChooserGroup069::Instance();
    s_apVoiceChooserList[70] = CVoiceChooserGroup070::Instance();
    s_apVoiceChooserList[71] = CVoiceChooserGroup071::Instance();
    s_apVoiceChooserList[72] = CVoiceChooserGroup072::Instance();
    s_apVoiceChooserList[73] = CVoiceChooserGroup073::Instance();
    s_apVoiceChooserList[74] = CVoiceChooserGroup074::Instance();
    s_apVoiceChooserList[75] = CVoiceChooserGroup075::Instance();
    s_apVoiceChooserList[76] = CVoiceChooserGroup076::Instance();
    s_apVoiceChooserList[77] = CVoiceChooserGroup077::Instance();
    s_apVoiceChooserList[78] = CVoiceChooserGroup078::Instance();
    s_apVoiceChooserList[79] = CVoiceChooserGroup079::Instance();
    s_apVoiceChooserList[80] = CVoiceChooserGroup080::Instance();
    s_apVoiceChooserList[81] = CVoiceChooserGroup081::Instance();
    s_apVoiceChooserList[82] = CVoiceChooserGroup082::Instance();
    s_apVoiceChooserList[83] = CVoiceChooserGroup083::Instance();
    s_apVoiceChooserList[84] = CVoiceChooserGroup084::Instance();
    s_apVoiceChooserList[85] = CVoiceChooserGroup085::Instance();
    s_apVoiceChooserList[86] = CVoiceChooserGroup086::Instance();
    s_apVoiceChooserList[87] = CVoiceChooserGroup087::Instance();
    s_apVoiceChooserList[88] = CVoiceChooserGroup088::Instance();
    s_apVoiceChooserList[89] = CVoiceChooserGroup089::Instance();
    s_apVoiceChooserList[90] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[91] = CVoiceChooserGroup091::Instance();
    s_apVoiceChooserList[92] = CVoiceChooserGroup092::Instance();
    s_apVoiceChooserList[93] = CVoiceChooserGroup093::Instance();
    s_apVoiceChooserList[94] = CVoiceChooserGroup094::Instance();
    s_apVoiceChooserList[95] = CVoiceChooserGroup095::Instance();
    s_apVoiceChooserList[96] = CVoiceChooserGroup096::Instance();
    s_apVoiceChooserList[97] = CVoiceChooserGroup097::Instance();
    s_apVoiceChooserList[98] = CVoiceChooserGroup098::Instance();
    s_apVoiceChooserList[99] = CVoiceChooserGroup099::Instance();
    s_apVoiceChooserList[100] = CVoiceChooserGroup100::Instance();
    s_apVoiceChooserList[101] = CVoiceChooserGroup101::Instance();
    s_apVoiceChooserList[102] = CVoiceChooserGroup102::Instance();
    s_apVoiceChooserList[103] = CVoiceChooserGroup103::Instance();
    s_apVoiceChooserList[104] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[105] = CVoiceChooserGroup105::Instance();
    s_apVoiceChooserList[106] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[107] = CVoiceChooserGroup107::Instance();
    s_apVoiceChooserList[108] = CVoiceChooserGroup108::Instance();
    s_apVoiceChooserList[109] = CVoiceChooserGroup109::Instance();
    s_apVoiceChooserList[110] = CVoiceChooserGroup110::Instance();
    s_apVoiceChooserList[111] = CVoiceChooserGroup111::Instance();
    s_apVoiceChooserList[112] = CVoiceChooserGroup112::Instance();
    s_apVoiceChooserList[113] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[114] = CVoiceChooserGroup114::Instance();
    s_apVoiceChooserList[115] = CVoiceChooserGroup115::Instance();
    s_apVoiceChooserList[116] = CVoiceChooserGroup116::Instance();
    s_apVoiceChooserList[117] = CVoiceChooserGroup117::Instance();
    s_apVoiceChooserList[118] = CVoiceChooserGroup118::Instance();
    s_apVoiceChooserList[119] = CVoiceChooserGroup119::Instance();
    s_apVoiceChooserList[120] = CVoiceChooserGroup120::Instance();
    s_apVoiceChooserList[121] = CVoiceChooserGroup121::Instance();
    s_apVoiceChooserList[122] = CVoiceChooserGroup122::Instance();
    s_apVoiceChooserList[123] = CVoiceChooserGroup123::Instance();
    s_apVoiceChooserList[124] = CVoiceChooserGroup124::Instance();
    s_apVoiceChooserList[125] = CVoiceChooserGroup125::Instance();
    s_apVoiceChooserList[126] = CVoiceChooserGroup126::Instance();
    s_apVoiceChooserList[127] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[128] = CVoiceChooserGroup128::Instance();
    s_apVoiceChooserList[129] = CVoiceChooserGroup129::Instance();
    s_apVoiceChooserList[130] = CVoiceChooserGroup130::Instance();
    s_apVoiceChooserList[131] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[132] = CVoiceChooserGroup132::Instance();
    s_apVoiceChooserList[133] = CVoiceChooserGroup133::Instance();
    s_apVoiceChooserList[134] = CVoiceChooserGroup134::Instance();
    s_apVoiceChooserList[135] = CVoiceChooserGroup135::Instance();
    s_apVoiceChooserList[136] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[137] = CVoiceChooserGroup137::Instance();
    s_apVoiceChooserList[138] = CVoiceChooserGroup138::Instance();
    s_apVoiceChooserList[139] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[140] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[141] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[142] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[143] = CVoiceChooserGroup143::Instance();
    s_apVoiceChooserList[144] = CVoiceChooserGroup144::Instance();
    s_apVoiceChooserList[145] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[146] = CVoiceChooserGroup146::Instance();
    s_apVoiceChooserList[147] = CVoiceChooserGroup147::Instance();
    s_apVoiceChooserList[148] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[149] = CVoiceChooserGroup149::Instance();
    s_apVoiceChooserList[150] = CVoiceChooserGroup150::Instance();
    s_apVoiceChooserList[151] = CVoiceChooserGroup151::Instance();
    s_apVoiceChooserList[152] = CVoiceChooserGroup152::Instance();
    s_apVoiceChooserList[153] = CVoiceChooserGroup153::Instance();
    s_apVoiceChooserList[154] = CVoiceChooserGroup154::Instance();
    s_apVoiceChooserList[155] = CVoiceChooserGroup155::Instance();
    s_apVoiceChooserList[156] = CVoiceChooserGroup156::Instance();
    s_apVoiceChooserList[157] = CVoiceChooserGroup157::Instance();
    s_apVoiceChooserList[158] = CVoiceChooserGroup158::Instance();
    s_apVoiceChooserList[159] = CVoiceChooserGroup159::Instance();
    s_apVoiceChooserList[160] = CVoiceChooserGroup160::Instance();
    s_apVoiceChooserList[161] = CVoiceChooserGroup161::Instance();
    s_apVoiceChooserList[162] = CVoiceChooserGroup162::Instance();
    s_apVoiceChooserList[163] = CVoiceChooserGroup163::Instance();
    s_apVoiceChooserList[164] = CVoiceChooserGroup164::Instance();
    s_apVoiceChooserList[165] = CVoiceChooserGroup165::Instance();
    s_apVoiceChooserList[166] = CVoiceChooserGroup166::Instance();
    s_apVoiceChooserList[167] = CVoiceChooserGroupEx167::Instance();
    s_apVoiceChooserList[168] = CVoiceChooserGroup168::Instance();
    s_apVoiceChooserList[169] = CVoiceChooserGroup169::Instance();
    s_apVoiceChooserList[170] = CVoiceChooserGroupEx170::Instance();
    s_apVoiceChooserList[171] = CVoiceChooserGroup171::Instance();
    s_apVoiceChooserList[172] = CVoiceChooserGroup172::Instance();
    s_apVoiceChooserList[173] = CVoiceChooserGroupEx173::Instance();
    s_apVoiceChooserList[174] = CVoiceChooserGroup174::Instance();
    s_apVoiceChooserList[175] = CVoiceChooserGroup175::Instance();
    s_apVoiceChooserList[176] = CVoiceChooserGroup176::Instance();
    s_apVoiceChooserList[177] = CVoiceChooserGroup177::Instance();
    s_apVoiceChooserList[178] = CVoiceChooserGroup178::Instance();
    s_apVoiceChooserList[179] = CVoiceChooserGroup179::Instance();
    s_apVoiceChooserList[180] = CVoiceChooserGroup180::Instance();
    s_apVoiceChooserList[181] = CVoiceChooserGroup181::Instance();
    s_apVoiceChooserList[182] = CVoiceChooserGroup182::Instance();
    s_apVoiceChooserList[183] = CVoiceChooserGroup183::Instance();
    s_apVoiceChooserList[184] = CVoiceChooserGroup184::Instance();
    s_apVoiceChooserList[185] = CVoiceChooserGroup185::Instance();
    s_apVoiceChooserList[186] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[187] = CVoiceChooserGroup187::Instance();
    s_apVoiceChooserList[188] = CVoiceChooserGroup188::Instance();
    s_apVoiceChooserList[189] = CVoiceChooserGroup189::Instance();
    s_apVoiceChooserList[190] = CVoiceChooserGroup190::Instance();
    s_apVoiceChooserList[191] = CVoiceChooserGroup191::Instance();
    s_apVoiceChooserList[192] = CVoiceChooserGroup192::Instance();
    s_apVoiceChooserList[193] = CVoiceChooserGroup193::Instance();
    s_apVoiceChooserList[194] = CVoiceChooserGroup194::Instance();
    s_apVoiceChooserList[195] = CVoiceChooserGroup195::Instance();
    s_apVoiceChooserList[196] = CVoiceChooserGroup196::Instance();
    s_apVoiceChooserList[197] = CVoiceChooserGroup197::Instance();
    s_apVoiceChooserList[198] = CVoiceChooserGroup198::Instance();
    s_apVoiceChooserList[199] = CVoiceChooserGroup199::Instance();
    s_apVoiceChooserList[200] = CVoiceChooserGroup200::Instance();
    s_apVoiceChooserList[201] = CVoiceChooserGroup201::Instance();
    s_apVoiceChooserList[202] = CVoiceChooserGroup202::Instance();
    s_apVoiceChooserList[203] = CVoiceChooserGroup203::Instance();
    s_apVoiceChooserList[204] = CVoiceChooserGroup204::Instance();
    s_apVoiceChooserList[205] = CVoiceChooserGroup205::Instance();
    s_apVoiceChooserList[206] = CVoiceChooserGroup206::Instance();
    s_apVoiceChooserList[207] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[208] = CVoiceChooserGroup208::Instance();
    s_apVoiceChooserList[209] = CVoiceChooserGroup209::Instance();
    s_apVoiceChooserList[210] = CVoiceChooserGroup210::Instance();
    s_apVoiceChooserList[211] = CVoiceChooserGroup211::Instance();
    s_apVoiceChooserList[212] = CVoiceChooserGroup212::Instance();
    s_apVoiceChooserList[213] = CVoiceChooserGroup213::Instance();
    s_apVoiceChooserList[214] = CVoiceChooserGroup214::Instance();
    s_apVoiceChooserList[215] = CVoiceChooserGroup215::Instance();
    s_apVoiceChooserList[216] = CVoiceChooserGroup216::Instance();
    s_apVoiceChooserList[217] = CVoiceChooser::Dummy();
    s_apVoiceChooserList[218] = CVoiceChooserGroup218::Instance();
    s_apVoiceChooserList[219] = CVoiceChooserGroup219::Instance();
    s_apVoiceChooserList[220] = CVoiceChooserGroup220::Instance();
    s_apVoiceChooserList[221] = CVoiceChooserGroup221::Instance();
    s_apVoiceChooserList[222] = CVoiceChooserGroup222::Instance();
    s_apVoiceChooserList[223] = CVoiceChooserGroup223::Instance();
    s_apVoiceChooserList[224] = CVoiceChooserGroup224::Instance();
    s_apVoiceChooserList[225] = CVoiceChooserGroup225::Instance();
    s_apVoiceChooserList[226] = CVoiceChooserGroup226::Instance();
    s_apVoiceChooserList[227] = nullptr;
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
        CVoiceChooseTool::m_VoicePlayerID = PLAYERID::ID_INVALID;
        return false;
    }
    else
    {
        CGameSound::PlayVoice(nVoiceCode, CVoiceChooseTool::m_VoicePlayerID);
        CVoiceChooseTool::m_VoicePlayerID = PLAYERID::ID_INVALID;
        return true;
    };
};