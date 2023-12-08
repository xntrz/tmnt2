#include "TestSoundSequence.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Sound/Sound.hpp"
#include "Game/System/Sound/GameSound.hpp"


class CTestSound
{
public:
    enum MENUTYPE
    {
        MENUTYPE_SELECT = 0,
        MENUTYPE_BGM,
        MENUTYPE_VOICE,
        MENUTYPE_SE,

        MENUTYPENUM,
    };
    
public:
    static MENUTYPE m_eMenuType;
    static bool m_bExitRq;
    static bool m_bBankSwRq;
    static int32 m_nBankVOX;
    static int32 m_nBankSE;

public:    
    CTestSound(void);
    ~CTestSound(void);
    void Attach(void);
    void Detach(void);
    bool Move(void);
    void Draw(void);
    void MenuInitialize(void);
    void MenuTerminate(void);
    
private:
    CDebugMenuCtrl* m_apMenu[MENUTYPENUM];
    CDebugMenuCtrl* m_pMenuCurrent;
};


/*static*/ CTestSound::MENUTYPE CTestSound::m_eMenuType = CTestSound::MENUTYPE_SELECT;
/*static*/ bool CTestSound::m_bExitRq = false;
/*static*/ bool CTestSound::m_bBankSwRq = false;
/*static*/ int32 CTestSound::m_nBankVOX = 0;
/*static*/ int32 CTestSound::m_nBankSE = 0;


CTestSound::CTestSound(void)
: m_apMenu()
, m_pMenuCurrent(nullptr)
{
    ;
};


CTestSound::~CTestSound(void)
{
    ;
};


void CTestSound::Attach(void)
{
    m_eMenuType = MENUTYPE_SELECT;
    m_bExitRq   = false;
    m_bBankSwRq = false;
    m_nBankVOX	= 0;
    m_nBankSE   = 0;
    
    m_pMenuCurrent = nullptr;

    CGameData::PushOption();
    CGameData::Option().Sound().SetDefault();

    MenuInitialize();
};


void CTestSound::Detach(void)
{
    MenuTerminate();

    CGameData::PopOption();
};


bool CTestSound::Move(void)
{
    if (m_bExitRq)
        return true;

    m_pMenuCurrent = m_apMenu[m_eMenuType];
    ASSERT(m_pMenuCurrent);
    
    m_pMenuCurrent->Period();

    switch (m_pMenuCurrent->GetResult())
    {
    case CDebugMenuCtrl::RESULT_CANCEL:
        {
            if (m_eMenuType == MENUTYPE_SELECT)
                m_bExitRq = true;
            else
                m_eMenuType = MENUTYPE_SELECT;
        }
        break;
    };

    return false;
};


void CTestSound::Draw(void)
{
    ASSERT(m_pMenuCurrent);
    m_pMenuCurrent->Draw();
};


void CTestSound::MenuInitialize(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apMenu); ++i)
    {
        ASSERT(m_apMenu[i] == nullptr);
        m_apMenu[i] = new CDebugMenuCtrl;
    };

    //
    //  Init main menu
    //
    CDebugMenuCtrl& MenuSel = *m_apMenu[MENUTYPE_SELECT];
    MenuSel.AddTrigger("BGM",   [](void* Param) { m_eMenuType = MENUTYPE_BGM;   });
    MenuSel.AddTrigger("VOICE", [](void* Param) { m_eMenuType = MENUTYPE_VOICE; });
    MenuSel.AddTrigger("SE",    [](void* Param) { m_eMenuType = MENUTYPE_SE;    });

    //
    //  Init BGM menu
    //
    CDebugMenuCtrl& MenuBGM = *m_apMenu[MENUTYPE_BGM];
	int32 BGMVolInit = CGameData::Option().Sound().GetVolume(OPTIONTYPES::VOLUMETYPE_BGM);

    MenuBGM.HexDispBegin();
    MenuBGM.AddInt("Play code", 0x3000, 0x3FFF, 0x1, 0x3000, [](int32 Value, bool Trigger) {
        if (Trigger)
            CGameSound::PlayBGM(Value);
    });
    MenuBGM.HexDispEnd();    
    MenuBGM.AddInt("Fade out", 0, 2 + 1, 1, 0, { "FAST", "NORMAL", "SLOW" }, [](int32 Value, bool Trigger) {
        if (Trigger)
            CGameSound::FadeOut(CGameSound::FADESPEED(Value));
    });    
    MenuBGM.AddInt("Volume", 0, 10 + 1, 1, BGMVolInit, [](int32 Value, bool Trigger) {
        if (Trigger)
        {
            CGameData::Option().Sound().SetVolume(OPTIONTYPES::VOLUMETYPE_BGM, Value);
            CGameData::Option().Sound().Apply();
        };
    });

    //
    //  Init VOICE menu
    //
    CDebugMenuCtrl& MenuVOX = *m_apMenu[MENUTYPE_VOICE];
	int32 VOXVolInit = CGameData::Option().Sound().GetVolume(OPTIONTYPES::VOLUMETYPE_VOICE);

    MenuVOX.AddInt("Bank", 0, 3, 1, 0, [](int32 Value, bool Trigger) {
        m_nBankVOX = Value;
    });
    MenuVOX.HexDispBegin();
    MenuVOX.AddInt("Play code", 0x000, 0xFFF, 0x1, 0x000, [](int32 Code, bool Trigger) {
        if (Trigger)
            CGameSound::PlayVoice((0x4000 + (m_nBankVOX * 0x1000)) + Code);
    });
    MenuVOX.HexDispEnd();
    MenuVOX.AddInt("Volume", 0, 10 + 1, 1, VOXVolInit, [](int32 Value, bool Trigger) {
		if (Trigger)
		{
			CGameData::Option().Sound().SetVolume(OPTIONTYPES::VOLUMETYPE_VOICE, Value);
			CGameData::Option().Sound().Apply();
		};
	});

    //
    //  Init SE menu
    //

    //
    //  Init menu common
    //
    for (int32 i = 0; i < COUNT_OF(m_apMenu); ++i)
    {
        CDebugMenuCtrl& Menu = *m_apMenu[i];

        if (i != MENUTYPE_SELECT)
        {
            Menu.AddTrigger("Pause",    [](void* Param) { CGameSound::Pause();  });
            Menu.AddTrigger("Resume",   [](void* Param) { CGameSound::Resume(); });
        };

        Menu.AddSeparator(true);
        
        if (i == MENUTYPE_SELECT)
            Menu.AddTrigger("EXIT", [](void* Param) { m_bExitRq = true; });
        else
            Menu.AddTrigger("Back", [](void* Param) { m_eMenuType = MENUTYPE_SELECT; CGameSound::Stop(); });
    };
};


void CTestSound::MenuTerminate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apMenu); ++i)
    {
        if (m_apMenu[i])
        {
            delete m_apMenu[i];
            m_apMenu[i] = nullptr;
        };
    };
};


static CTestSound* s_pTestSoundImpl = nullptr;


/*static*/ CProcess* CTestSoundSequence::Instance(void)
{
    return new CTestSoundSequence;
};


CTestSoundSequence::CTestSoundSequence(void)
{
    ;
};


CTestSoundSequence::~CTestSoundSequence(void)
{
    ;
};


bool CTestSoundSequence::OnAttach(const void* pParam)
{
    ASSERT(!s_pTestSoundImpl);
    
    s_pTestSoundImpl = new CTestSound;
    s_pTestSoundImpl->Attach();
    
    return true;
};


void CTestSoundSequence::OnDetach(void)
{
    ASSERT(s_pTestSoundImpl);

    s_pTestSoundImpl->Detach();
    delete s_pTestSoundImpl;
    s_pTestSoundImpl = nullptr;
};


void CTestSoundSequence::OnMove(bool bRet, const void* pReturnValue)
{
    ASSERT(s_pTestSoundImpl);
    
    (s_pTestSoundImpl->Move() ? Ret() : (void)0);
};


void CTestSoundSequence::OnDraw(void) const
{
    ASSERT(s_pTestSoundImpl);

    s_pTestSoundImpl->Draw();
};

#endif /* _DEBUG */