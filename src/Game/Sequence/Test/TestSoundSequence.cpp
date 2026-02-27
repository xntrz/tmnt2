#include "TestSoundSequence.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/Sound.hpp"
#include "Game/System/Utils/MathFunction.hpp"

#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"

#pragma push_macro("new")
#pragma push_macro("delete")
#undef new
#undef delete
#include <deque>
#pragma pop_macro("delete")
#pragma pop_macro("new")


#define REQ_POST(type, ...) \
    CTestSoundSequence::m_pQueue->push_back(CTestSoundSequence::REQUEST(type, __VA_ARGS__))

#define CB_REQ_TRIGGER(type) \
    [](void* param){ REQ_POST(type, reinterpret_cast<int32>(param)); }

#define CB_REQ_INT(type) \
    [](int32 value, bool trigger){ REQ_POST(type, value, trigger); }

#define REQ_SE_MAKECODE(code, id) \
    (((id & 0xFFFF) << 16) | (code & 0xFFFF))

#define REQ_SE_GETCODE(code) \
    (code & 0xFFFF)

#define REQ_SE_GETID(code) \
    ((code >> 16) & 0xFFFF)


/* funcs from SdDrv module for testing */
extern int32 GetSECount(void);
extern int32 GetSECountInBank(int32 _bank);
extern int32 GetSEBank(int32 _code);
extern bool LoadSEBank(int32 _bank);
extern int32 GetTRSECount(void);
extern int32 GetTRSEBank(int32 _code);
extern bool LoadTRSEBank(int32 _bank);
extern int32 GetTRSEWaveBank(int32 _code);
extern bool IsTRSEWaveBankLoaded(int32 _bank);


static int32 s_stereoTestSECodeList[] =
{
    SDCODE_SE(0x2008),
    SDCODE_SE(0x1035),
    SDCODE_SE(0x108D),
    SDCODE_SE(0x108E),
    SDCODE_SE(0x1097),
    SDCODE_SE(0x1098),
    SDCODE_SE(0x10ED),
    SDCODE_SE(0x110D),
};


class CTestSoundSequence final : public CSequence
{
public:
    struct REQUEST
    {
        int32 id;
        int32 value;
        bool trigger;

        /* int callback constructor */
        inline REQUEST(int32 _id, int32 _value, bool _trigger)
            : id(_id), value(_value), trigger(_trigger) {};

        /* trigger callback constructor with void* */
        inline REQUEST(int32 _id, void* param)
            : id(_id), value(reinterpret_cast<int32>(param)), trigger(false) {};

        /* trigger callback constructor with int32 */
        inline REQUEST(int32 _id, int32 param)
            : id(_id), value(param), trigger(false) {};
    };
    
    enum MENUTYPE
    {
        MENUTYPE_SELECT = 0,
        MENUTYPE_STEREO_TEST,
        MENUTYPE_BGM,
        MENUTYPE_VOICE,
        MENUTYPE_SE,
        MENUTYPE_TRSE,

        MENUTYPENUM,
    };

    enum REQTYPE
    {
        /* bgm */
        REQTYPE_BGM_PLAY = 0,
        REQTYPE_BGM_FADE,
        REQTYPE_BGM_VOL,

        /* voice */
        REQTYPE_VOICE_BANK,
        REQTYPE_VOICE_PLAY,
        REQTYPE_VOICE_VOL,

        /* se & trans se */
        REQTYPE_SE_PLAY, // hiword of request value contains SE unique id, loword contains SE id itself (see REQ_SE_x macro above)
        REQTYPE_SE_FADE,
        REQTYPE_SE_VOL,

        /* stereo test of se or trans se */
        REQTYPE_TEST_START,
        REQTYPE_TEST_RST_POS,
        REQTYPE_TEST_SD_MODE,

        /* common sound */
        REQTYPE_SOUND_PAUSE,
        REQTYPE_SOUND_RESUME,

        /* menu */
        REQTYPE_MENU,
        REQTYPE_EXIT,
    };

    enum SE_LOAD_STATE
    {
        SE_LOAD_STATE_NONE = 0,
        SE_LOAD_STATE_TRANS,
        SE_LOAD_STATE_WAVE,
    };

    struct SE_LOAD
    {
        int32 bank;
        int32 reqId;
        int32 reqCode;
    };

    struct STEREO_TEST
    {
        RwCamera* pCamera;
        RwV3d vSoundSrcPos;
        int32 seCode;
        int32 seCodeId;
        float distance;
        bool enabled;
    };

public:
    static std::deque<REQUEST>* m_pQueue;

public:
    CTestSoundSequence(void);
    virtual ~CTestSoundSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void SetCurrentMenu(int32 type);
    void MenuInitialize(void);
    void MenuOnChange(int32 from, int32 to);
    bool SeLoadingProc(void);
    bool SurCtrlTestProc(void);
    void SurCtrlTestStart(int32 seCode);
    void SurCtrlTestStop(void);

private:
    int32           m_currentSE;
    int32           m_currentSEid;
    CDebugMenuCtrl  m_aMenu[MENUTYPENUM];
    CDebugMenuCtrl* m_pMenuCurrent;
    int32           m_currentMenuType;
    SE_LOAD_STATE   m_seLoadState;
    int32           m_currentTRANSBank;
    SE_LOAD         m_seLoadTRANS;
    int32           m_currentWAVEBank;
    SE_LOAD         m_seLoadWAVE;	
    int32           m_currentVoiceBank;
    STEREO_TEST     m_stereoTest;
};


/*static*/ std::deque<CTestSoundSequence::REQUEST>* CTestSoundSequence::m_pQueue = nullptr;


CTestSoundSequence::CTestSoundSequence(void)
: m_currentSE(0)
, m_currentSEid(0)
, m_aMenu()
, m_pMenuCurrent(nullptr)
, m_currentMenuType(-1)
, m_seLoadState(SE_LOAD_STATE_NONE)
, m_currentTRANSBank(-1)
, m_seLoadTRANS()
, m_currentWAVEBank(-1)
, m_seLoadWAVE()
, m_currentVoiceBank(-1)
, m_stereoTest()
{
    m_pQueue = new std::deque<REQUEST>;
    ASSERT(m_pQueue);

    for (int32 i = 0; i < COUNT_OF(m_aMenu); ++i)
        m_aMenu[i].Reset();
};


CTestSoundSequence::~CTestSoundSequence(void)
{
    SurCtrlTestStop();

    if (m_pQueue)
    {
        delete m_pQueue;
        m_pQueue = nullptr;
    };
};


bool CTestSoundSequence::OnAttach(const void* pParam)
{
    /* init se state */
    m_currentSE = 0;
    m_currentSEid = 0;

    /* init menu */
    for (int32 i = 0; i < COUNT_OF(m_aMenu); ++i)
        m_aMenu[i].Reset();
    m_pMenuCurrent = nullptr;
    m_currentMenuType = -1;

    MenuInitialize();
    SetCurrentMenu(MENUTYPE_SELECT);

    /* init load state */
    m_seLoadState = SE_LOAD_STATE_NONE;

    /* init TRANS WAVE load */
    m_currentTRANSBank = -1;
    m_seLoadTRANS.bank = -1;
    m_seLoadTRANS.reqCode = 0;
    m_seLoadTRANS.reqId = -1;

    /* init WAVE load */
    m_currentWAVEBank = -1;
    m_seLoadWAVE.bank = -1;
    m_seLoadWAVE.reqCode = 0;
    m_seLoadWAVE.reqId = -1;

    /* init voice load */
    m_currentVoiceBank = 0;

    /* init stereo test */
    m_stereoTest.pCamera = nullptr;
    m_stereoTest.vSoundSrcPos = Math::VECTOR3_ZERO;
    m_stereoTest.seCode = -1;
    m_stereoTest.seCodeId = -1;
    m_stereoTest.distance = 0.0f;
    m_stereoTest.enabled = false;

    return true;
};


void CTestSoundSequence::OnDetach(void)
{
    ;
};


void CTestSoundSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (SeLoadingProc())
        return;

    SurCtrlTestProc();

    if (m_pMenuCurrent)
        m_pMenuCurrent->Period();

    while (!m_pQueue->empty())
    {
        REQUEST request = m_pQueue->front();
        m_pQueue->pop_front();

        switch (request.id)
        {
        /*
        *	BGM request handling
        */
        case REQTYPE_BGM_PLAY:
            if (request.trigger)
                CGameSound::PlayBGM(request.value);
            break;

        case REQTYPE_BGM_FADE:
            if (request.trigger)
                CGameSound::FadeOut(CGameSound::FADESPEED(request.value));
            break;

        case REQTYPE_BGM_VOL:
            CGameData::Option().Sound().SetVolume(OPTIONTYPES::VOLUMETYPE_BGM, request.value);
            CGameData::Option().Sound().Apply();
            break;

        /*
         *	VOICE request handling
         */
        case REQTYPE_VOICE_BANK:
            m_currentVoiceBank = request.value;
            break;

        case REQTYPE_VOICE_PLAY:
            if (request.trigger)
                CGameSound::PlayVoice((SD_CODE_VOICE_BEGIN + (m_currentVoiceBank * SD_VOICE_PER_BANK)) + request.value);
            break;

        case REQTYPE_VOICE_VOL:
            CGameData::Option().Sound().SetVolume(OPTIONTYPES::VOLUMETYPE_VOICE, request.value);
            CGameData::Option().Sound().Apply();
            break;
            
        /*
         *	SE & TRANS SE request handling
         */
        case REQTYPE_SE_PLAY:
            if (request.trigger)
            {
                if (m_currentSE)
                {
                    CGameSound::FadeOutSE(m_currentSE, CGameSound::FADESPEED_FAST);
                    m_currentSE = 0;
                };

                int32 code = REQ_SE_GETCODE(request.value);
                int32 id = REQ_SE_GETID(request.value);

                bool bIsSE = ((code >= SD_CODE_SE_BEGIN) && (code <= SD_CODE_SE_END));
                if (bIsSE)
                {
                    /* se case */
                    int32 seWaveBank = GetSEBank(code);
                    if (m_currentWAVEBank != seWaveBank)
                    {
                        m_seLoadState = SE_LOAD_STATE_WAVE;
                        m_seLoadWAVE = { seWaveBank, request.id, request.value };
                        break;
                    };
                }
                else
                {
                    /* trans se case */
                    int32 seTransBank = GetTRSEBank(code);
                    if (m_currentTRANSBank != seTransBank)
                    {
                        m_seLoadState = SE_LOAD_STATE_TRANS;
                        m_seLoadTRANS = { seTransBank, request.id, request.value };
                        break;
                    };

                    int32 seWaveBank = GetTRSEWaveBank(code);
                    if (!IsTRSEWaveBankLoaded(seWaveBank))
                    {
                        m_seLoadState = SE_LOAD_STATE_WAVE;
                        m_seLoadWAVE = { seWaveBank, request.id, request.value };
                        break;
                    };
                };

                m_currentSE = code;
                m_currentSEid = id;

                if (m_currentSEid)
                    CGameSound::PlayPositionSE(&Math::VECTOR3_ZERO, m_currentSE, m_currentSEid);
                else
                    CGameSound::PlaySE(m_currentSE);
            };
            break;

        case REQTYPE_SE_FADE:
            if (request.trigger)
                CGameSound::FadeOutSE(m_currentSE, CGameSound::FADESPEED(request.value));
            break;

        case REQTYPE_SE_VOL:
            CGameData::Option().Sound().SetVolume(OPTIONTYPES::VOLUMETYPE_SE, request.value);
            CGameData::Option().Sound().Apply();
            break;

        /*
         *	Stereo test req handling
         */
        case REQTYPE_TEST_START:
            if (request.trigger)
            {
                if (m_stereoTest.enabled)
                    SurCtrlTestStop();
                
                SurCtrlTestStart(s_stereoTestSECodeList[request.value]);
            };
            break;

        case REQTYPE_TEST_RST_POS:
            m_stereoTest.vSoundSrcPos = Math::VECTOR3_ZERO;
            break;

        case REQTYPE_TEST_SD_MODE:
            CGameSound::SetMode(CGameSound::MODE(request.value));
            break;

        /*
         *	Common SOUND request handling
         */
        case REQTYPE_SOUND_PAUSE:
            CGameSound::Pause();
            break;

        case REQTYPE_SOUND_RESUME:
            CGameSound::Resume();
            break;

        /*
         *	Menu request handling
         */
        case REQTYPE_MENU:
            MenuOnChange(m_currentMenuType, request.value);
            SetCurrentMenu(request.value);
            break;

        case REQTYPE_EXIT:
            Ret();
            return;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CTestSoundSequence::OnDraw(void) const
{
    if (m_pMenuCurrent)
        m_pMenuCurrent->Draw();
};


void CTestSoundSequence::SetCurrentMenu(int32 type)
{
    ASSERT(type >= 0);
    ASSERT(type < COUNT_OF(m_aMenu));

    m_currentMenuType = type;
    m_pMenuCurrent = &m_aMenu[m_currentMenuType];
};


void CTestSoundSequence::MenuInitialize(void)
{
    //
    //  Init main menu
    //
    CDebugMenuCtrl& select = m_aMenu[MENUTYPE_SELECT];
    
    select.AddTrigger("BGM", 		CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_BGM);
    select.AddTrigger("VOICE", 		CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_VOICE);
    select.AddTrigger("SE", 		CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_SE);
    select.AddTrigger("TRANS SE", 	CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_TRSE);
    select.AddTrigger("STEREO TEST",CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_STEREO_TEST);

    //
    //	Init stereo test menu
    //
    CDebugMenuCtrl& stereoTest = m_aMenu[MENUTYPE_STEREO_TEST];

    stereoTest.AddText("CONTROL: Use <LSTICK> to move sound source position"); // NUMPAD for keyboard
    stereoTest.AddSeparator(true);
    stereoTest.HexDispBegin();
    stereoTest.AddInt("Start", 0, COUNT_OF(s_stereoTestSECodeList), 0x1, 0, CB_REQ_INT(REQTYPE_TEST_START));
    stereoTest.HexDispEnd();
    stereoTest.AddFloatDisp("Distance", &m_stereoTest.distance);
    stereoTest.AddTrigger("Reset position", CB_REQ_TRIGGER(REQTYPE_TEST_RST_POS));
    stereoTest.AddInt("Mode", 0, 2, 1, CGameSound::GetMode(), { "MONO", "STEREO" }, CB_REQ_INT(REQTYPE_TEST_SD_MODE));

    //
    //  Init BGM menu
    //
    CDebugMenuCtrl& bgm = m_aMenu[MENUTYPE_BGM];
    int32 bgmVolInit = CGameData::Option().Sound().GetVolume(OPTIONTYPES::VOLUMETYPE_BGM);
    int32 bgmCodeStart  = SD_CODE_BGM_BEGIN;
    int32 bgmCodeEnd    = SD_CODE_BGM_END;

    bgm.HexDispBegin();
    bgm.AddInt("Play code", bgmCodeStart, bgmCodeEnd, 0x1, bgmCodeStart, CB_REQ_INT(REQTYPE_BGM_PLAY));
    bgm.HexDispEnd();
    bgm.AddInt("Fade out", 0, 2 + 1, 1, 0, { "FAST", "NORMAL", "SLOW" }, CB_REQ_INT(REQTYPE_BGM_FADE));
    bgm.AddInt("Volume", 0, 10 + 1, 1, bgmVolInit, CB_REQ_INT(REQTYPE_BGM_VOL));

    //
    //  Init VOICE menu
    //
    CDebugMenuCtrl& voice = m_aMenu[MENUTYPE_VOICE];
    int32 voiceVolInit = CGameData::Option().Sound().GetVolume(OPTIONTYPES::VOLUMETYPE_VOICE);
    int32 voiceCodeStart = 0;
    int32 voiceCodeEnd = SD_VOICE_PER_BANK - 1;

    voice.AddInt("Bank", 0, 3, 1, 0, CB_REQ_INT(REQTYPE_VOICE_BANK));
    voice.HexDispBegin();
    voice.AddInt("Play code", voiceCodeStart, voiceCodeEnd, 0x1, voiceCodeStart, CB_REQ_INT(REQTYPE_VOICE_PLAY));
    voice.HexDispEnd();
    voice.AddInt("Volume", 0, 10 + 1, 1, voiceVolInit, CB_REQ_INT(REQTYPE_VOICE_VOL));

    //
    //  Init SE & TRANS SE menu
    //
    CDebugMenuCtrl& se = m_aMenu[MENUTYPE_SE];
    CDebugMenuCtrl& trse = m_aMenu[MENUTYPE_TRSE];
    int32 seVolInit = CGameData::Option().Sound().GetVolume(OPTIONTYPES::VOLUMETYPE_SE);
    int32 seCount = GetSECount();
    int32 trseCount = GetTRSECount();
    int32 seCodeStart = SD_CODE_SE_BEGIN;
    int32 seCodeEnd = seCodeStart + seCount;
    int32 trseCodeStart = SD_CODE_TRSE_BEGIN;
    int32 trseCodeEnd = trseCodeStart + trseCount;

    se.HexDispBegin();
    se.AddInt("Play code", seCodeStart, seCodeEnd, 0x1, seCodeStart, CB_REQ_INT(REQTYPE_SE_PLAY));
    se.HexDispEnd();

    trse.HexDispBegin();
    trse.AddInt("Play code", trseCodeStart, trseCodeEnd, 0x1, trseCodeStart, CB_REQ_INT(REQTYPE_SE_PLAY));
    trse.HexDispEnd();

    CDebugMenuCtrl* SeMenuCtrlTbl[] = {
        &m_aMenu[MENUTYPE_SE],
        &m_aMenu[MENUTYPE_TRSE],
        &m_aMenu[MENUTYPE_STEREO_TEST],
    };

    for (int32 i = 0; i < COUNT_OF(SeMenuCtrlTbl); ++i)
    {
        SeMenuCtrlTbl[i]->AddInt("Fade out", 0, 2 + 1, 1, 0, { "FAST", "NORMAL", "SLOW" }, CB_REQ_INT(REQTYPE_SE_FADE));
        SeMenuCtrlTbl[i]->AddInt("Volume", 0, 10 + 1, 1, seVolInit, CB_REQ_INT(REQTYPE_SE_VOL));
    };

    //
    //  Init SOUND common & menu common
    //
    for (int32 i = 0; i < COUNT_OF(m_aMenu); ++i)
    {
        CDebugMenuCtrl& menu = m_aMenu[i];
    
        if ((i != MENUTYPE_SELECT) &&
            (i != MENUTYPE_STEREO_TEST))
        {
            menu.AddTrigger("Pause", CB_REQ_TRIGGER(REQTYPE_SOUND_PAUSE));
            menu.AddTrigger("Resume", CB_REQ_TRIGGER(REQTYPE_SOUND_RESUME));
        };

        bool bWithoutText = true;
        menu.AddSeparator(bWithoutText);

        if (i == MENUTYPE_SELECT)
            menu.AddTrigger("EXIT", CB_REQ_TRIGGER(REQTYPE_EXIT));
        else
            menu.AddTrigger("Back", CB_REQ_TRIGGER(REQTYPE_MENU), MENUTYPE_SELECT);
    };
};


void CTestSoundSequence::MenuOnChange(int32 from, int32 to)
{
    ASSERT(from >= 0);
    ASSERT(from < MENUTYPENUM);
    ASSERT(to >= 0);
    ASSERT(to < MENUTYPENUM);
    ASSERT(from != to);

    switch (from)
    {
    case MENUTYPE_SELECT:
        CGameData::PushOption();
        CGameData::Option().Sound().SetDefault();
        break;

    case MENUTYPE_STEREO_TEST:
        SurCtrlTestStop();
        break;

    default:
        break;
    };

    switch (to)
    {
    case MENUTYPE_SELECT:
        CGameSound::FadeOut(CGameSound::FADESPEED_FAST);
        CGameSound::FadeOutSE(m_currentSE, CGameSound::FADESPEED_FAST);
        CGameData::PopOption();
        break;

    default:
        break;
    };
};


bool CTestSoundSequence::SeLoadingProc(void)
{
    switch (m_seLoadState)
    {
    case SE_LOAD_STATE_NONE:
        break;

    case SE_LOAD_STATE_TRANS:
        if (CGameSound::IsLoadEnd())
        {
            SE_LOAD& load = m_seLoadTRANS;
            if (load.bank != -1)
            {
                CLoadingDisplay::Start(this);
                LoadTRSEBank(load.bank);
                m_currentTRANSBank = load.bank;
                load.bank = -1;				
            }
            else
            {
                CLoadingDisplay::Stop(this);
                m_seLoadState = SE_LOAD_STATE_NONE;
                REQ_POST(load.reqId, load.reqCode, true);
                load.reqCode = 0;
                load.reqId = -1;
            };
        };
        break;

    case SE_LOAD_STATE_WAVE:
        if (CGameSound::IsLoadEnd())
        {
            SE_LOAD& load = m_seLoadWAVE;
            if (load.bank != -1)
            {
                CLoadingDisplay::Start(this);
                LoadSEBank(load.bank);
                m_currentWAVEBank = load.bank;
                load.bank = -1;
            }
            else
            {
                CLoadingDisplay::Stop(this);
                m_seLoadState = SE_LOAD_STATE_NONE;
                REQ_POST(load.reqId, load.reqCode, true);
                load.reqCode = 0;
                load.reqId = -1;
            };
        };
        break;

    default:
        break;
    };

    /* se load proc is busy whenever its not in none state */
    return (m_seLoadState != SE_LOAD_STATE_NONE);
};


bool CTestSoundSequence::SurCtrlTestProc(void)
{
    if (!m_stereoTest.enabled)
        return false;
    
    int16 lx = 0;
    lx |= CController::GetAnalog(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::ANALOG_LSTICK_X);
    lx |= CController::GetAnalog(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::ANALOG_LSTICK_X);

    int16 ly = 0;
    ly |= CController::GetAnalog(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::ANALOG_LSTICK_Y);
    ly |= CController::GetAnalog(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::ANALOG_LSTICK_Y);

    const float dt = CScreen::TimerStride();
    const float move = 30.0f;
    const float dtMove = dt * move;

    if (lx < 0)	// left
        m_stereoTest.vSoundSrcPos.x += dtMove;

    if (lx > 0) // right
        m_stereoTest.vSoundSrcPos.x -= dtMove;
    
    if (ly < 0) // back
        m_stereoTest.vSoundSrcPos.z -= dtMove;

    if (ly > 0) // forward
        m_stereoTest.vSoundSrcPos.z += dtMove;

    /* se position ctrl */
    CGameSound::PlayPositionSE(&m_stereoTest.vSoundSrcPos, m_stereoTest.seCode, m_stereoTest.seCodeId);

    /* update distance */
    RwFrame* pFrame = RwCameraGetFrame(m_stereoTest.pCamera);
    if (pFrame)
    {
        RwMatrix* pMat = RwFrameGetMatrix(pFrame);
        
        RwV3d vCameraPos = pMat->pos;
        RwV3d vSoundPos = m_stereoTest.vSoundSrcPos;

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, &vSoundPos, &vCameraPos);

        m_stereoTest.distance = Math::Vec3_Length(&vDist);
    };

    /* sur ctrl always not busy for allow menu input */
    return false;
};


void CTestSoundSequence::SurCtrlTestStart(int32 seCode)
{
    if (m_stereoTest.enabled)
        return;

    /* default pos for listener and sound source */
    RwV3d vCenterPos = { 0.0f, 0.0f, 0.0f };

    /* create camera */
    RwCamera* pCamera = RwCameraCreate();
    if (!pCamera)
        return;
    
    RwFrame* pFrame = RwFrameCreate();
    if (pFrame)
    {
        RwFrameSetIdentity(pFrame);

        RwMatrix* pMat = RwFrameGetMatrix(pFrame);
        pMat->pos = vCenterPos;

        RwCameraSetFrame(pCamera, pFrame);
    };

    /* init test */
    //vCenterPos = { -15.0f, 0.0f, 15.0f };
    
    m_stereoTest.enabled = true;
    m_stereoTest.pCamera = pCamera;
    m_stereoTest.vSoundSrcPos = vCenterPos;
    m_stereoTest.seCode = seCode;
    m_stereoTest.seCodeId = 7;	/* for sur ctrl unique id required for SE */

    CGameSound::AttachCamera(pCamera);

    /* manually post req for load & play se */
    REQ_POST(REQTYPE_SE_PLAY, REQ_SE_MAKECODE(m_stereoTest.seCode, m_stereoTest.seCodeId), true);
};


void CTestSoundSequence::SurCtrlTestStop(void)
{
    if (!m_stereoTest.enabled)
        return;

    m_currentSE = 0;

    CGameSound::DetachCamera();

    m_stereoTest.enabled = false;
    m_stereoTest.seCodeId = -1;

    if (m_stereoTest.seCode)
    {
        CGameSound::FadeOutSE(m_stereoTest.seCode, CGameSound::FADESPEED_FAST);
        m_stereoTest.seCode = -1;
    };

    if (m_stereoTest.pCamera)
    {
        RwFrame* pFrame = RwCameraGetFrame(m_stereoTest.pCamera);
        if (pFrame)
        {
            RwCameraSetFrame(m_stereoTest.pCamera, nullptr);
            RwFrameDestroy(pFrame);
        };

        RwCameraDestroy(m_stereoTest.pCamera);
        m_stereoTest.pCamera = nullptr;
    };
};


CProcess* TestSoundSequenceInstance()
{
    return new CTestSoundSequence;
};


#endif /* _DEBUG */