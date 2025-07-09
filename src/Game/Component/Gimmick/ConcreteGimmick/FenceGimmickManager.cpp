#include "FenceGimmickManager.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "System/Common/Screen.hpp"


/*static*/ CFenceGimmickManager* CFenceGimmickManager::m_pInstance = nullptr;

/*static*/ CFenceGimmickManager::ATTACKERINFO*
CFenceGimmickManager::m_paPlayerInfo[GAMETYPES::PLAYERS_MAX] = { 0 };


/*static*/ void CFenceGimmickManager::Initialize(void)
{
    ASSERT(m_pInstance);
    
    for (int32 i = 0; i < COUNT_OF(m_paPlayerInfo); ++i)
        m_paPlayerInfo[i] = nullptr;

    IntrudeObservation();
};


/*static*/ void CFenceGimmickManager::Terminate(void)
{
    ASSERT(m_pInstance);
    
    for (int32 i = 0; i < COUNT_OF(m_paPlayerInfo); ++i)
    {
        if (m_paPlayerInfo[i])
        {
            delete m_paPlayerInfo[i];
            m_paPlayerInfo[i] = nullptr;
        };
    };
};


/*static*/ void CFenceGimmickManager::IntrudeObservation(void)
{
    ASSERT(m_pInstance);

    int32 nPlayerCnt = CGameProperty::GetPlayerNum();

    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (!CGameProperty::Player(i)->IsAlive())
            continue;

        if (m_paPlayerInfo[i])
            continue;

        m_paPlayerInfo[i] = AllocNewFenceInfo();
        ASSERT(m_paPlayerInfo[i]);

        m_paPlayerInfo[i]->m_nPlayerNo = i;
        m_paPlayerInfo[i]->m_fAttackTime = -1.0f;
    };
};


/*static*/ bool CFenceGimmickManager::DispatchInformation(CHitAttackData* pAttack)
{
    ASSERT(m_pInstance);
    ASSERT(pAttack);

    CGameObject* pAttacker = pAttack->GetObject();
    if (pAttacker->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return false;

    if (static_cast<CCharacter*>(pAttacker)->GetCharacterType() != CCharacter::TYPE_PLAYER)
        return false;

    int32 nPlayerNo = static_cast<CPlayerCharacter*>(pAttacker)->GetPlayerNo();

    ASSERT(nPlayerNo >= 0);
    ASSERT(nPlayerNo < COUNT_OF(m_paPlayerInfo));
    ASSERT(m_paPlayerInfo[nPlayerNo] != nullptr);

    ATTACKERINFO* pInfo = m_paPlayerInfo[nPlayerNo];
    pInfo->m_fAttackTime = CScreen::TimeElapsed();
    pInfo->m_nPlayerNo = nPlayerNo;

    return true;
};


/*static*/ int32 CFenceGimmickManager::GetLastAttacker(void)
{
    ASSERT(m_pInstance);

    float fRecentTime = 0.0f;
    int32 nLastAttacker = -1;
    int32 nPlayerCnt = CGameProperty::GetPlayerNum();
    
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        ATTACKERINFO* pInfo = m_paPlayerInfo[i];
        
        if (!pInfo)
            continue;

        if (fRecentTime < pInfo->m_fAttackTime)
        {
            nLastAttacker = i;
            fRecentTime = pInfo->m_fAttackTime;
        };
    };

    return nLastAttacker;
};


/*static*/ const CFenceGimmickManager::ATTACKERINFO*
CFenceGimmickManager::GetInfo(int32 nPlayerNo)
{
    ASSERT(m_pInstance);
    ASSERT(nPlayerNo >= 0);
    ASSERT(nPlayerNo < COUNT_OF(m_paPlayerInfo));

    return m_paPlayerInfo[nPlayerNo];
};


/*static*/ CFenceGimmickManager::ATTACKERINFO*
CFenceGimmickManager::AllocNewFenceInfo(void)
{
    ASSERT(m_pInstance);

    ATTACKERINFO* pInfo = new ATTACKERINFO;
    pInfo->m_nPlayerNo = 0;
    pInfo->m_fAttackTime = 0.0f;

    return pInfo;
};


CFenceGimmickManager::CFenceGimmickManager(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
{
    ASSERT(!m_pInstance);
    
    m_pInstance = this;
    
    Initialize();
};


CFenceGimmickManager::~CFenceGimmickManager(void)
{
    ASSERT(m_pInstance);
    ASSERT(m_pInstance == this);

    Terminate();

    m_pInstance = nullptr;
};


void CFenceGimmickManager::Run(void)
{
    CGimmick::Run();
    IntrudeObservation();
};


void CFenceGimmickManager::Draw(void) const
{
    CGimmick::Draw();
};