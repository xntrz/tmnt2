#include "DbgParamCtrlProcess.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Sequence/Area/AreaSequence.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/Sequence.hpp"


static uint32 FindFirstObjectHandleExcept(uint32 hObjExcept)
{
    for (CGameObject* pGameObj = CGameObjectManager::GetNext();
        pGameObj != nullptr;
        pGameObj = CGameObjectManager::GetNext(pGameObj))
    {
		if (pGameObj->GetHandle() == hObjExcept)
			continue;        

        return pGameObj->GetHandle();
    };

    return 0;
};


class CDbgParamCtrl
{
private:
    struct SEQMENUINFO
    {
        int32 iSeqLbl;
        CDebugMenuCtrl* pMenuCtrl;
    };

public:
    CDbgParamCtrl(void);
    ~CDbgParamCtrl(void);
    bool Period(void); // returns true if OK pressed or not exist for current sequence
    void Draw(void);
    void AddMenuForSequence(int32 iSeqLbl, CDebugMenuCtrl* pMenuCtrl);

private:
    int32 m_iCurrentSeqLbl;
    SEQMENUINFO m_aSeqMenuInfo[32];
    int32 m_nSeqMenuInfoCnt;
    CDebugMenuCtrl* m_pCurrentMenu;
    CDebugMenuCtrl m_menuAreaSel;
    CDebugMenuCtrl m_menuStage;
};


CDbgParamCtrl::CDbgParamCtrl(void)
: m_iCurrentSeqLbl(PROCESSTYPES::LABEL_EOL)
, m_aSeqMenuInfo()
, m_nSeqMenuInfoCnt(0)
, m_pCurrentMenu(nullptr)
, m_menuAreaSel()
, m_menuStage()
{
    //
    //  Area menu
    //
    m_menuAreaSel.AddTrigger("Test cursor anim", [](void*) { CAreaSequence::m_bDebugClearAnimRequest = true; });
    m_menuAreaSel.AddInt(
        "Set all area clear",
        GAMETYPES::CLEARRANK_NONE,
        GAMETYPES::CLEARRANK_NUM,
        1,
		0,
        { "NONE", "E", "D", "C", "B", "A", "S", "SS" },
        [](int32 nValue, bool bTrigger) {
            if (!bTrigger)
                return;
            
            for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; ++i)
            {
                CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_A);
                CGameData::Record().Area().SetAreaRank(AREAID::VALUE(i), GAMETYPES::CLEARRANK(nValue));
            };                
    });

    AddMenuForSequence(PROCLABEL_SEQ_AREA, &m_menuAreaSel);

    //
    //  Stage menu
    //
    m_menuStage.SetDispMax(30);
    m_menuStage.AddTrigger("Set Game clear A",  [](void*) { CGameStage::GetCurrent()->NotifyGameClear(CGamePlayResult::CLEARSUB_A); });
    m_menuStage.AddTrigger("Set Game clear B",  [](void*) { CGameStage::GetCurrent()->NotifyGameClear(CGamePlayResult::CLEARSUB_B); });
    m_menuStage.AddTrigger("Set Game over", [](void*) { CGameStage::GetCurrent()->NotifyGameOver(); });
    m_menuStage.AddBool("Godmode", [](bool bValue, bool bTrigger) {
		if (!bTrigger)
			return;
        if (bValue)
            CGameStage::GetCurrent()->BeginPlayerNegateDamage();
        else
            CGameStage::GetCurrent()->EndPlayerNegateDamager();
    });
    m_menuStage.AddInt("Add HP", -300, 300, 1, 0, [](int32 nVal, bool bTrigger) {
        if(bTrigger)
            CGameProperty::Player(0)->AddHP(nVal);
    });
    m_menuStage.AddInt("Set camera mode", 0, CMapCamera::MODEMAX, 1, 0, { "manual", "auto", "intro" }, [](int32 nVal, bool bTrigger) {
        CGameStage* pGameStage = CGameStage::GetCurrent();
        CMapCamera* pMapCamera = pGameStage->GetMapCamera();
        if (bTrigger && pMapCamera)
            pMapCamera->SetCameraMode(CMapCamera::MODE(nVal));
    });
    m_menuStage.AddInt("Hit FRONT flyaway at", 0, 4, 1, 0, { "player1", "player2", "player3", "player4" }, [](int32 nVal, bool bTrigger) {
        // for test fall rubble at area id 58 that triggered only from flyaway wall crash
        if (!bTrigger)
            return;
        
        if (nVal >= CGameProperty::GetPlayerNum())
            return;
        
        if (!CGameProperty::Player(nVal)->IsAlive())
            return;

		//
        //  attacker cannot attack himself so find any object on map that is not same PLAYER
		//
        uint32 hAttacker = FindFirstObjectHandleExcept(CGameProperty::Player(nVal)->GetCurrentCharacter()->GetHandle());
        if (!hAttacker)
            return;

		//
		//	move hit pos infront of player
		//
        RwV3d vPlayerPos = Math::VECTOR3_ZERO;
        CGameProperty::Player(nVal)->GetPosition(&vPlayerPos);

        RwV3d vPlayerDir = Math::VECTOR3_AXIS_Z;
        CGameProperty::Player(nVal)->GetCurrentCharacter()->RotateVectorByDirection(&vPlayerDir, &vPlayerDir);
        
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
        AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        AttackData.SetAttackNo(0);
        AttackData.SetPower(0);
        AttackData.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        AttackData.SetTarget(CHitAttackData::TARGET_PLAYER);
        AttackData.SetFlyawayParameter(12.0f, 10.0f);
        AttackData.SetMotion("Flyaway");
        
        CHitAttackManager::RegistAttack(&AttackData);
    });
    m_menuStage.AddSeparator();
    m_menuStage.AddBool("Gimmick show model", [](bool bValue, bool bTrigger){ CGimmickDebug::SHOW_MODEL = bValue;   });
    m_menuStage.AddBool("Gimmick show area", [](bool bValue, bool bTrigger) { CGimmickDebug::SHOW_AREA = bValue;    });
    m_menuStage.AddBool("Gimmick show me", [](bool bValue, bool bTrigger)   { CGimmickDebug::SHOW_ME = bValue;      });
    m_menuStage.AddSeparator();
    m_menuStage.AddBool("Show hit attack", [](bool bValue, bool bTrigger)   { CHitAttackManager::m_bDebugDrawSphereAttack = bValue; });
    m_menuStage.AddBool("Show hit catch", [](bool bValue, bool bTrigger)    { CHitAttackManager::m_bDebugDrawSphereCatch = bValue;  });
    m_menuStage.AddBool("Show body hit", [](bool bValue, bool bTrigger)     { CBodyHitManager::m_bDebugDrawSphere = bValue;         });

    AddMenuForSequence(PROCLABEL_SEQ_NORMALSTAGE, &m_menuStage);
    AddMenuForSequence(PROCLABEL_SEQ_RIDESTAGE, &m_menuStage);
    AddMenuForSequence(PROCLABEL_SEQ_NEXUSSTAGE, &m_menuStage);
};


CDbgParamCtrl::~CDbgParamCtrl(void)
{
    ;
};


bool CDbgParamCtrl::Period(void)
{
    int32 iCurrentSeqLbl = CSequence::GetCurrently();
    if (iCurrentSeqLbl != m_iCurrentSeqLbl)
    {
        m_iCurrentSeqLbl = iCurrentSeqLbl;
        m_pCurrentMenu = nullptr;

        for (int32 i = 0; i < m_nSeqMenuInfoCnt; ++i)
        {
            if (m_aSeqMenuInfo[i].iSeqLbl == m_iCurrentSeqLbl)
                m_pCurrentMenu = m_aSeqMenuInfo[i].pMenuCtrl;
        };
    };

    if (m_pCurrentMenu)
    {
        m_pCurrentMenu->SetColorBackground({ 0x00, 0x00, 0x00, 0x7F });
        m_pCurrentMenu->SetPos(5, 5);
        m_pCurrentMenu->Period();
        return (m_pCurrentMenu->GetResult() == CDebugMenuCtrl::RESULT_OK);
    };

    return true;
};


void CDbgParamCtrl::Draw(void)
{
    if (m_pCurrentMenu)
        m_pCurrentMenu->Draw();
};


void CDbgParamCtrl::AddMenuForSequence(int32 iSeqLbl, CDebugMenuCtrl* pMenuCtrl)
{
    ASSERT(pMenuCtrl);
    ASSERT(m_nSeqMenuInfoCnt < COUNT_OF(m_aSeqMenuInfo));

    m_aSeqMenuInfo[m_nSeqMenuInfoCnt].pMenuCtrl = pMenuCtrl;
    m_aSeqMenuInfo[m_nSeqMenuInfoCnt].iSeqLbl = iSeqLbl;

    ++m_nSeqMenuInfoCnt;
};


static bool s_bDebugMenuEnable = false;
static CDbgParamCtrl* s_pDebugMenu = nullptr;


/*static*/ CProcess* CDbgParamCtrlProcess::Instance(void)
{
    return new CDbgParamCtrlProcess;
};


/*static*/ void CDbgParamCtrlProcess::Initialize(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGPARAMCTRL, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CDbgParamCtrlProcess::Terminate(CProcess* pSender)
{
    pSender->Mail().Send(PROCLABEL_DBGPARAMCTRL, PROCESSTYPES::MAIL::TYPE_DETACH);
};


bool CDbgParamCtrlProcess::Attach(void)
{
    s_pDebugMenu = new CDbgParamCtrl;
    return true;
};


void CDbgParamCtrlProcess::Detach(void)
{
    if (s_pDebugMenu)
    {
        delete s_pDebugMenu;
        s_pDebugMenu = nullptr;
    };
};


void CDbgParamCtrlProcess::Move(void)
{
    if (s_bDebugMenuEnable)
    {
        if (s_pDebugMenu->Period())
        {
            s_bDebugMenuEnable = false;
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);
        };
    };
    
    uint32 Digital = 0;
    Digital |= IPad::GetDigital(IPad::CONTROLLER_LOCKED_ON_VIRTUAL);
    Digital |= IPad::GetDigital(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL);

    uint32 DigitalT = 0;
    DigitalT |= IPad::GetDigitalTrigger(IPad::CONTROLLER_LOCKED_ON_VIRTUAL);
    DigitalT |= IPad::GetDigitalTrigger(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL);

    if ((Digital & IPad::DIGITAL_START) && (DigitalT & IPad::DIGITAL_R3))
    {        
        s_bDebugMenuEnable = !s_bDebugMenuEnable;

        if (s_bDebugMenuEnable && !s_pDebugMenu->Period())
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE);
        else
            Mail().Send(CSequence::GetCurrently(), PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);
    };
};


void CDbgParamCtrlProcess::Draw(void) const
{
    if (s_bDebugMenuEnable)
        s_pDebugMenu->Draw();
};

#endif /* _DEBUG */