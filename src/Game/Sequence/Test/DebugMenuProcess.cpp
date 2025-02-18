#include "DebugMenuProcess.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameStageDebug.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/ModuleDebug.hpp"
#include "Game/Sequence/Area/AreaSequence.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitDebug.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Screen.hpp"

#include <cctype> // toupper

#ifdef TARGET_PC
#include "System/PC/PCSpecific.hpp"
#include "System/PC/PCPhysicalControllerKey.hpp"
#endif /* TARGET_PC */


#ifdef TARGET_PC
#define MENU_ACTIVATE_KEY (DIK_F4)
#endif /* TARGET_PC */


static int32 s_aSequenceLabelListToAddMenu[] =
{
    PROCLABEL_SEQ_NORMALSTAGE,
    PROCLABEL_SEQ_RIDESTAGE,
    PROCLABEL_SEQ_NEXUSSTAGE,
    PROCLABEL_SEQ_PLAYDEMO,
    PROCLABEL_SEQ_ENEMYTESTSTAGE,
};


static inline bool IsMenuAvailableForSequence(int32 iSeqLabel)
{
    for (int32 i = 0; i < COUNT_OF(s_aSequenceLabelListToAddMenu); ++i)
    {
        if (s_aSequenceLabelListToAddMenu[i] == iSeqLabel)
            return true;
    };

    return false;
};


static uint32 FindFirstObjectHandleExcept(uint32 hObjExcept)
{
    CGameObject* pObj = CGameObjectManager::GetNext();
    while (pObj)
    {
        if (pObj->GetHandle() != hObjExcept)
            return pObj->GetHandle();

        pObj = CGameObjectManager::GetNext(pObj);
    };

    return 0;
};


static void MakeDummyHitAttack(int32 playerNo,
                               CHitAttackData::STATUS status = CHitAttackData::STATUS_KNOCK,
                               bool bConfusion = false)
{
    if (playerNo >= CGameProperty::GetPlayerNum())
        return;

    if (!CGameProperty::Player(playerNo)->IsAlive())
        return;

    //
    //  attacker cannot attack himself so find any object on map that is not same PLAYER
    //
    uint32 hAttacker = FindFirstObjectHandleExcept(CGameProperty::Player(playerNo)->GetCurrentCharacter()->GetHandle());
    if (!hAttacker)
        return;

    //
    //	move hit pos infront of player
    //
    RwV3d vPlayerPos = Math::VECTOR3_ZERO;
    CGameProperty::Player(playerNo)->GetPosition(&vPlayerPos);

    RwV3d vPlayerDir = Math::VECTOR3_AXIS_Z;
    CGameProperty::Player(playerNo)->GetCurrentCharacter()->RotateVectorByDirection(&vPlayerDir, &vPlayerDir);

    Math::Vec3_Scale(&vPlayerDir, &vPlayerDir, 1.0f);
    Math::Vec3_Add(&vPlayerPos, &vPlayerPos, &vPlayerDir);

    //
    //	regist dummy attack
    //
    RwSphere HitSphere = { vPlayerPos, 2.0f };

    CHitAttackData AttackData;
    AttackData.Cleanup();
    AttackData.SetObject(hAttacker);
    AttackData.SetObjectPos(&vPlayerPos);
    AttackData.SetShape(CHitAttackData::SHAPE_SPHERE);
    AttackData.SetSphere(&HitSphere);
    AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_BREAK);
    AttackData.SetAttackNo(0);
    AttackData.SetPower(30);
    AttackData.SetStatus(status);
    AttackData.SetTarget(CHitAttackData::TARGET_PLAYER);
    AttackData.SetMotion("DUMMY");

    if (status == CHitAttackData::STATUS_FLYAWAY)
        AttackData.SetFlyawayParameter(12.0f, 10.0f);
    else if ((status != CHitAttackData::STATUS_KNOCK) && (status != CHitAttackData::STATUS_THROW))
        AttackData.SetTroubleParameter(8.0f);

    if (bConfusion)
        AttackData.SetFlagConfusion(true);

    CHitAttackManager::RegistAttack(&AttackData);
};


class CDebugMenu
{
private:
    struct SEQMENUINFO
    {
        int32 iSeqLbl;
        CDebugMenuCtrl* pMenuCtrl;
    };

public:
    CDebugMenu(CProcessMail& mail);
    ~CDebugMenu(void);
    void PeriodSeq(void); // updates every frame regardless whether menu enabled or not
    bool PeriodMenu(void); // updates only when menu is enabled, returns true if OK pressed or not exist for current sequence
    void DrawSeq(void); // sames rules as for period
    void DrawMenu(void); // sames rules as for period
    void AddMenuForSequence(int32 iSeqLbl, CDebugMenuCtrl* pMenuCtrl);
    void InitMenu(void);
    CDebugMenuCtrl& Menu(void);

private:
    static CDebugMenu*  m_pInstance;
    CProcessMail&       m_mail;
    int32               m_menuDispMax;
    int32               m_iCurrentSeqLbl;
    SEQMENUINFO         m_aSeqMenuInfo[32];
    int32               m_nSeqMenuInfoCnt;
    CDebugMenuCtrl*     m_pCurrentMenu;
    CDebugMenuCtrl      m_menu;
    int32               m_targetPlayerNo;
    int32               m_targetStatusHit;
    bool                m_bInSeqStep;
    int32               m_numFramesInStep;
    int32               m_inStepMakeScrShot;
};


/*static*/ CDebugMenu* CDebugMenu::m_pInstance = nullptr;


CDebugMenu::CDebugMenu(CProcessMail& mail)
: m_mail(mail)
, m_menuDispMax(20)
, m_iCurrentSeqLbl(PROCESSTYPES::LABEL_EOL)
, m_aSeqMenuInfo()
, m_nSeqMenuInfoCnt(0)
, m_pCurrentMenu(nullptr)
, m_menu()
, m_targetPlayerNo(-1)
, m_targetStatusHit(-1)
, m_bInSeqStep(false)
, m_numFramesInStep(0)
, m_inStepMakeScrShot(0)
{
    m_pInstance = this;
};


CDebugMenu::~CDebugMenu(void)
{
    m_pInstance = nullptr;
};


void CDebugMenu::PeriodSeq(void)
{
    int32 iCurrentSeqLbl = CSequence::GetCurrently();
    if (iCurrentSeqLbl != m_iCurrentSeqLbl)
    {
        InitMenu();

        m_iCurrentSeqLbl = iCurrentSeqLbl;
        m_pCurrentMenu = nullptr;

        for (int32 i = 0; i < m_nSeqMenuInfoCnt; ++i)
        {
            if (m_aSeqMenuInfo[i].iSeqLbl == m_iCurrentSeqLbl)
                m_pCurrentMenu = m_aSeqMenuInfo[i].pMenuCtrl;
        };
    };

    if (m_bInSeqStep)
    {
        if (--m_numFramesInStep <= 0)
        {
            m_mail.Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE);
            m_bInSeqStep = false;
        };
    };
};


bool CDebugMenu::PeriodMenu(void)
{
    if (m_inStepMakeScrShot)
    {
        --m_inStepMakeScrShot;
        if (m_inStepMakeScrShot == 1)
        {
#ifdef TARGET_PC
            CPCSpecific::MakeWindowScreenshotToClipboard();
#endif /* TARGET_PC */
        };
    };

    if (m_pCurrentMenu)
    {
        m_pCurrentMenu->SetColorBackground({ 0x00, 0x00, 0x00, 0x7F });
        m_pCurrentMenu->SetPos(5, 5);
        m_pCurrentMenu->Period();

        if (m_bInSeqStep || m_inStepMakeScrShot)
            return false;
        
        return (m_pCurrentMenu->GetResult() == CDebugMenuCtrl::RESULT_OK);
    };

    return true;
};


void CDebugMenu::DrawSeq(void)
{
	;
};


void CDebugMenu::DrawMenu(void)
{
    if (m_inStepMakeScrShot)
        return;

    if (m_pCurrentMenu)
        m_pCurrentMenu->Draw();
};


void CDebugMenu::AddMenuForSequence(int32 iSeqLbl, CDebugMenuCtrl* pMenuCtrl)
{
    ASSERT(pMenuCtrl);
    ASSERT(m_nSeqMenuInfoCnt < COUNT_OF(m_aSeqMenuInfo));

    m_aSeqMenuInfo[m_nSeqMenuInfoCnt].pMenuCtrl = pMenuCtrl;
    m_aSeqMenuInfo[m_nSeqMenuInfoCnt].iSeqLbl = iSeqLbl;

    ++m_nSeqMenuInfoCnt;
};


void CDebugMenu::InitMenu(void)
{    
    m_nSeqMenuInfoCnt = 0;
    m_menu.Reset();
    m_menu.SetDispMax(m_menuDispMax);

    /**
     *  Init STAGE opts
     */
    m_menu.AddText("------- STAGE -------");
    m_menu.AddTrigger("Set Game clear A", [](void*) {
        CGameStage::GetCurrent()->NotifyGameClear(CGamePlayResult::CLEARSUB_A);
    });
    
    m_menu.AddTrigger("Set Game clear B", [](void*) {
        CGameStage::GetCurrent()->NotifyGameClear(CGamePlayResult::CLEARSUB_B);
    });
    
    m_menu.AddTrigger("Set Game over", [](void*) {
        CGameStage::GetCurrent()->NotifyGameOver();
    });

    m_menu.AddFloat("Stage camera zoom scale", 1.0f, 20.0f, 0.1f, 1.0f, [](float value, bool trigger) {
        CGameStageDebug::CAMERA_ZOOM_SCALE = value;
    });

    m_menu.AddFloat("Manual camera move speed", 1.0f, 200.0f, 1.0f, CGameStageDebug::CAMERA_MANUAL_SPEED, [](float value, bool trigger) {
        CGameStageDebug::CAMERA_MANUAL_SPEED = value;
    });

    m_menu.AddIntDisp("Stage tick", &CGameStage::Tick);
    m_menu.SetLastItemEnable(false);
    m_menu.AddInt("Step stage for", 1, 61, 1, 1, [](int32 value, bool trigger) {
        if (trigger)
        {
            if (!m_pInstance->m_bInSeqStep)
            {
                m_pInstance->m_mail.Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);
                m_pInstance->m_bInSeqStep = true;
                m_pInstance->m_numFramesInStep = Clamp(value, 1, 60);
            };
        };        
    });

    m_menu.AddBool("Godmode", [](bool bValue, bool bTrigger) {

        if (bValue != CGameStageDebug::GODMODE)
        {
            CGameStageDebug::GODMODE = bValue;
            if (CGameStageDebug::GODMODE)
                CGameStage::GetCurrent()->BeginPlayerNegateDamage();
            else
                CGameStage::GetCurrent()->EndPlayerNegateDamager();
        };
    });
    
    m_menu.AddInt("Add HP", -300, 300, 1, 0, [](int32 nVal, bool bTrigger) {
        if(bTrigger)
            CGameProperty::Player(0)->AddHP(nVal);
    });
    
    m_menu.AddInt("Set camera mode", 0, CMapCamera::MODEMAX, 1, 0, { "manual", "auto", "intro" }, [](int32 nVal, bool bTrigger) {
        CGameStage* pGameStage = CGameStage::GetCurrent();
        CMapCamera* pMapCamera = pGameStage->GetMapCamera();
        if (bTrigger && pMapCamera)
            pMapCamera->SetCameraMode(CMapCamera::MODE(nVal));
    });
    
    m_menu.AddInt("Replace PLAYERS to", 0, GAMETYPES::PLAYERS_MAX, 1, 0, { "player1", "player2", "player3", "player4" }, [](int32 nVal, bool bTrigger) {
        if (bTrigger)
        {
            IGamePlayer* pGameplayer = CGameProperty::Player(nVal);

            RwV3d vecReplacePos = Math::VECTOR3_ZERO;
            pGameplayer->GetPosition(&vecReplacePos);

            int32 numPlayers = CGameProperty::GetPlayerNum();
            for (int32 i = 0; i < numPlayers; ++i)
            {
                pGameplayer = CGameProperty::Player(i);
                pGameplayer->Relocation(&vecReplacePos, 0.0f, false);
            };
        };
    });
    
    /**
     *  Init GIMMICK debug opts
     */
    m_menu.AddText("");
    m_menu.AddText("------- GIMMICK -------");

    m_menu.AddBool("Gimmick show model", CGimmickDebug::SHOW_MODEL, [](bool bValue, bool bTrigger) {
        CGimmickDebug::SHOW_MODEL = bValue;
    });
    
    m_menu.AddBool("Gimmick show area", CGimmickDebug::SHOW_AREA, [](bool bValue, bool bTrigger) {
        CGimmickDebug::SHOW_AREA = bValue;
    });
    
    m_menu.AddBool("Gimmick show me", CGimmickDebug::SHOW_ME, [](bool bValue, bool bTrigger) {
        CGimmickDebug::SHOW_ME = bValue;
    });

    /**
     *  Init HIT debug opts
     */
    m_menu.AddText("");
    m_menu.AddText("------- HIT -------");

    static int32 s_targetPlayerNo = 0;

    m_menu.AddInt("Hit target", 0, GAMETYPES::PLAYERS_MAX, 1, s_targetPlayerNo, { "player1", "player2", "player3", "player4" }, [](int32 nVal, bool bTrigger) {
        s_targetPlayerNo = nVal;
    });

    static bool s_applyHitConfusion = false;

    m_menu.AddBool("Apply hit confusion", s_applyHitConfusion, [](bool value, bool bTrigger) {
        s_applyHitConfusion = value;
    });

    static const char* s_apszHitStatusName[] =
    {
        "KNOCK",
        "FLYAWAY",
        "THROW",
        "STUN",
        "DINDLE",
        "SLEEP",
        "FREEZE",
        "BIND",
    };

    static CHitAttackData::STATUS s_aHitStatus[] =
    {
        CHitAttackData::STATUS_KNOCK,
        CHitAttackData::STATUS_FLYAWAY,
        CHitAttackData::STATUS_THROW,
        CHitAttackData::STATUS_STUN,
        CHitAttackData::STATUS_DINDLE,
        CHitAttackData::STATUS_SLEEP,
        CHitAttackData::STATUS_FREEZE,
        CHitAttackData::STATUS_BIND,
    };

    m_menu.AddInt("Make hit ", 0, COUNT_OF(s_aHitStatus), 1, 0, s_apszHitStatusName, [](int32 nVal, bool bTrigger) {
        if ((nVal == 2) && s_applyHitConfusion)
            return;
        
        if (bTrigger)
            MakeDummyHitAttack(s_targetPlayerNo, s_aHitStatus[nVal], s_applyHitConfusion);
    });

    m_menu.AddBool("Show hit attack", CHitDebug::SHOW_HIT_ATTACK, [](bool bValue, bool bTrigger) {
        CHitDebug::SHOW_HIT_ATTACK = bValue;
    });
    
    m_menu.AddBool("Show hit catch", CHitDebug::SHOW_HIT_CATCH, [](bool bValue, bool bTrigger) {
        CHitDebug::SHOW_HIT_CATCH = bValue;
    });
    
    m_menu.AddBool("Show body hit", CHitDebug::SHOW_HIT_BODY, [](bool bValue, bool bTrigger) {
        CHitDebug::SHOW_HIT_BODY = bValue;
    });

    /**
     *  Init MODULE debug opts
     */
    m_menu.AddText("");
    m_menu.AddText("------- MODULE -------");

    m_menu.AddBool("Show EFFECT time", CModuleDebug::SHOW_CHR_STATUS_DURATION, [](bool bValue, bool bTrigger) {
        CModuleDebug::SHOW_CHR_STATUS_DURATION = bValue;
    });

    /**
     *  Init ENEMY & AI debug opts
     */
    m_menu.AddText("");
    m_menu.AddText("------- ENEMY & AI -------");

    m_menu.AddBool("Show AI info", CEnemyDebug::SHOW_AI_INFO, [](bool bValue, bool bTrigger) {
        CEnemyDebug::SHOW_AI_INFO = bValue;
    });

    m_menu.AddBool("Show AI move point", CEnemyDebug::SHOW_AI_MOVE_POINT, [](bool bValue, bool bTrigger) {
        CEnemyDebug::SHOW_AI_MOVE_POINT = bValue;
    });

    m_menu.AddBool("Show AI patrol area", CEnemyDebug::SHOW_AI_PATROL_AREA, [](bool bValue, bool bTrigger) {
        CEnemyDebug::SHOW_AI_PATROL_AREA = bValue;
    });
    
    m_menu.AddBool("Show AI suitable area", CEnemyDebug::SHOW_AI_SUITABLE_AREA, [](bool bValue, bool bTrigger) {
        CEnemyDebug::SHOW_AI_SUITABLE_AREA = bValue;
    });
    
    m_menu.AddBool("Show ENEMY SPAWN info", CEnemyDebug::SHOW_ENEMY_SPAWN_INFO, [](bool bValue, bool bTrigger) {
        CEnemyDebug::SHOW_ENEMY_SPAWN_INFO = bValue;
    });

    /**
     *  Init COMMON opts
     */
    m_menu.AddText("");
    m_menu.AddText("------- OTHER -------");

    m_menu.AddTrigger("Make WND screenshot to clipboard", [](void*) {
        m_pInstance->m_inStepMakeScrShot = 3;
    });

    m_menu.AddText("");

    /**
     *  Regist menu for sequence label list
     */
    for (int32 i = 0; i < COUNT_OF(s_aSequenceLabelListToAddMenu); ++i)
        AddMenuForSequence(s_aSequenceLabelListToAddMenu[i], &m_menu);
};


CDebugMenuCtrl& CDebugMenu::Menu(void)
{
    return m_menu;
};


static bool s_bDebugMenuEnable = false;
static CDebugMenu* s_pDebugMenu = nullptr;
static CDebugFontCtrl* s_pDebugMenuHelpFont = nullptr;


/*static*/ CProcess* CDebugMenuProcess::Instance(void)
{
    return new CDebugMenuProcess;
};


/*static*/ void CDebugMenuProcess::Initialize(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGMENU, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CDebugMenuProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGMENU, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ CDebugMenuCtrl& CDebugMenuProcess::Menu(void)
{
    ASSERT(s_pDebugMenu != nullptr);
    return s_pDebugMenu->Menu();
};


bool CDebugMenuProcess::Attach(void)
{
    s_pDebugMenu = new CDebugMenu(Mail());
    s_pDebugMenuHelpFont = new CDebugFontCtrl;

    return true;
};


void CDebugMenuProcess::Detach(void)
{
    if (s_pDebugMenuHelpFont)
    {
        delete s_pDebugMenuHelpFont;
        s_pDebugMenuHelpFont = nullptr;
    };

    if (s_pDebugMenu)
    {
        delete s_pDebugMenu;
        s_pDebugMenu = nullptr;
    };
};


void CDebugMenuProcess::Move(void)
{
    s_pDebugMenu->PeriodSeq();

    if (s_bDebugMenuEnable)
    {
        if (s_pDebugMenu->PeriodMenu())
        {
            s_bDebugMenuEnable = false;
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);
        };
    };

#ifdef TARGET_PC
    if (CPCSpecific::IsKeyTrigger(MENU_ACTIVATE_KEY))
    {
        s_bDebugMenuEnable = !s_bDebugMenuEnable;

        if (s_bDebugMenuEnable && !s_pDebugMenu->PeriodMenu())
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE);
        else
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);
    };
#endif /* TARGET_PC */

    /* terminate self when game exit */
    if (!Info().IsProcessExist(PROCLABEL_SEQ_GAMEMAIN))
        Mail().Send(Info().Label(), PROCESSTYPES::MAIL::TYPE_DETACH);
};


void CDebugMenuProcess::Draw(void) const
{
    s_pDebugMenu->DrawSeq();

    if (s_bDebugMenuEnable)
    {
        s_pDebugMenu->DrawMenu();
    }
    else
    {
        if (!IsMenuAvailableForSequence(CSequence::GetCurrently()))
            return;

        static float timer      = 0.0f;
        static float timerHold  = 0.0f;
		static float nextTime   = 1.0f;
		static bool  holdOn     = true;
		static uint8 alphaBasis = 0;

        if (timer <= nextTime)
        {
			timer += CScreen::TimerStride();
            alphaBasis = static_cast<uint8>(127.0f - Math::Cos(timer * MATH_PI) * 127.0f);
        }
        else
        {
            timerHold += CScreen::TimerStride();
            if (timerHold >= (holdOn ? 3.0f : 0.5f))
            {
				nextTime += 1.0f;
                timerHold = 0.0f;
				holdOn = !holdOn;
            };
        };

        s_pDebugMenuHelpFont->Background({ 0x00,0x00,0x00, alphaBasis }); // black
        s_pDebugMenuHelpFont->Color({ 0xFF, 0xFF, 0xFF, alphaBasis }); // white
        s_pDebugMenuHelpFont->Position(0, 0);

        /* get key name and transform it to upper case format */
        const char* pszKeyName = CPCSpecific::GetKeyName(MENU_ACTIVATE_KEY);
        ASSERT(pszKeyName, "key is invalid or not allowed");
        
        char szKeyBuffer[32];
        szKeyBuffer[0] = '\0';

        std::strcpy(szKeyBuffer, pszKeyName);

        size_t len = std::strlen(szKeyBuffer);
        for (size_t i = 0; i < len; ++i)
            szKeyBuffer[i] = static_cast<char>(std::toupper(static_cast<int>(szKeyBuffer[i])));

        /* format final string */        
        char szBuffer[128];
        szBuffer[0] = '\0';

        std::sprintf(szBuffer, "Press <%s> to open debug menu", szKeyBuffer);

        s_pDebugMenuHelpFont->Print(szBuffer);
    };
};

#endif /* _DEBUG */