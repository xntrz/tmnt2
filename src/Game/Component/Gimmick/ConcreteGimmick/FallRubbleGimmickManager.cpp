#include "FallRubbleGimmickManager.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FallGimmick.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"


static const CFallRubbleGimmickManager::INITPARAM s_aInitParam[] =
{
    { 1.0f, 5, 0.0f,  6.0f, 1.0f, CFallRubbleGimmickManager::TYPE_M58OB },
    { 0.0f, 5, 10.0f, 6.0f, 0.4f, CFallRubbleGimmickManager::TYPE_HUN   },
    { 0.0f, 7, 10.0f, 6.0f, 0.4f, CFallRubbleGimmickManager::TYPE_HUN2  },
};


static_assert(COUNT_OF(s_aInitParam) == CFallRubbleGimmickManager::TYPENUM, "should equal");


CFallRubbleGimmickManager::CFallRubbleGimmickManager(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_aRubbleInfo()
, m_nTotalCreatedNum(0)
, m_fTimerUntilFall(0.0f)
, m_nCreateSameTime(0)
, m_type(TYPENUM)
, m_nRubbleDamage(0)
, m_fRubbleBaseHeight(0.0f)
, m_fRubbleRandHeight(0.0f)
, m_fRubbleBodySphereRadius(0.0f)
{
    std::memset(m_aRubbleInfo, 0, sizeof(m_aRubbleInfo));

    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);
    
    ASSERT(pBasicParam->m_subid >= 0);
    ASSERT(pBasicParam->m_subid < TYPENUM);

    const INITPARAM* pInitParam = &s_aInitParam[pBasicParam->m_subid];

    m_nTotalCreatedNum          = 0;
    m_nCreateSameTime           = pInitParam->m_nCreate;
    m_fTimerUntilFall           = pInitParam->m_fTime;
    m_type                      = pInitParam->m_type;
    m_fRubbleBaseHeight         = pInitParam->m_fBaseHeight;
    m_nRubbleDamage             = 5;
    m_fRubbleRandHeight         = pInitParam->m_fRandom;
    m_fRubbleBodySphereRadius   = pInitParam->m_fBodyRadius;
};


CFallRubbleGimmickManager::~CFallRubbleGimmickManager(void)
{
    ;
};


void CFallRubbleGimmickManager::Run(void)
{
    rubbleGrandControl();
};


void CFallRubbleGimmickManager::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        createRubble();
};


void CFallRubbleGimmickManager::rubbleGrandControl(void)
{
    if (m_type == TYPE_M58OB)
        createRubble();

    for (int32 i = 0; i < COUNT_OF(m_aRubbleInfo); ++i)
        rubbleControl(&m_aRubbleInfo[i]);
};


void CFallRubbleGimmickManager::createRubble(void)
{
    int32 nPlayerCnt = CGameProperty::GetPlayerNum();
    
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChara = CGameProperty::Player(i)->GetCurrentCharacter();
        if (canRubbleCreate(pPlayerChara))
            initRubbleInfo(pPlayerChara, &m_aRubbleInfo[i]);
    };
};


void CFallRubbleGimmickManager::rubbleControl(RUBBLEINFO* pInfo)
{
    if (!pInfo->m_bUse)
        return;

    switch (pInfo->m_state)
    {
    case STATE_EXEC:
        {
            createRubbleGimmick(pInfo, m_nCreateSameTime);
            pInfo->m_state = (m_type == TYPE_M58OB ? STATE_REST : STATE_END);
        }
        break;

    case STATE_REST:
        {
            CGameObject* pObj = CGameObjectManager::GetObject(pInfo->m_hObj);
            ASSERT(pObj->GetType() == GAMEOBJECTTYPE::CHARACTER);

            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(pObj);
            
            if ((pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DOWN_FRONT) ||
                (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DOWN_BACK))
            {
                pInfo->m_state = STATE_END;
            };
        }
        break;

    case STATE_END:
        {
            pInfo->m_bUse = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


bool CFallRubbleGimmickManager::canRubbleCreate(CPlayerCharacter* pPlayerChara)
{
    if ((m_type != TYPE_M58OB)
        || (pPlayerChara->GetStatus() == PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_FRONT)
        || (pPlayerChara->GetStatus() == PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK))
    {
        return (!isExistSameHandleInWork(pPlayerChara->GetHandle()));
    };
    
    return false;
};


void CFallRubbleGimmickManager::initRubbleInfo(CGameObject* pObj, RUBBLEINFO* pInfo)
{
    ASSERT(pObj->GetType() == GAMEOBJECTTYPE::CHARACTER);

    std::memset(pInfo, 0, sizeof(*pInfo));

    pInfo->m_fTimer = 0.0f;
    pInfo->m_bUse   = true;
    pInfo->m_hObj   = pObj->GetHandle();
    pInfo->m_state  = STATE_EXEC;
    RwMatrixSetIdentity(&pInfo->m_matrix);

    CPlayerCharacter* pPlayerChara = static_cast<CPlayerCharacter*>(pObj);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    pPlayerChara->GetPosition(&vPosition);
    vPosition.y = m_fRubbleBaseHeight;

	RwV3d vRotation = Math::VECTOR3_ZERO;
	vRotation.y = pPlayerChara->GetDirection();

    if (pPlayerChara->GetStatus() == PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK)
    {
		vRotation.y += Math::PI;
		vRotation.y = Math::RadianInvClamp(vRotation.y);
    };

    Math::Matrix_AffineTransformation(&pInfo->m_matrix, nullptr, &vRotation, &vPosition);
};


bool CFallRubbleGimmickManager::isExistSameHandleInWork(uint32 hObj)
{
    for (int32 i = 0; i < COUNT_OF(m_aRubbleInfo); ++i)
    {
        if (m_aRubbleInfo[i].m_bUse && (m_aRubbleInfo[i].m_hObj == hObj))
            return true;
    };

    return false;
};


void CFallRubbleGimmickManager::createRubbleGimmick(RUBBLEINFO* pInfo, int32 nCount)
{
    for (int32 i = 0; i < nCount; ++i)
    {
        CGameObject* pObj = CGameObjectManager::GetObject(pInfo->m_hObj);        
        ASSERT(pObj);

        //
        //  prepare create param
        //
        GIMMICKPARAM::GIMMICK_BASIC param;
        std::memset(&param, 0, sizeof(param));

        //
        //  init position
        //
        if (i || (pObj->GetType() != GAMEOBJECTTYPE::CHARACTER))
        {
            getRandomRubblePos(&param.m_vPosition, pInfo);
        }
        else
        {
            static_cast<CCharacter*>(pObj)->GetPosition(&param.m_vPosition);
            param.m_vPosition.y = (m_fRubbleBaseHeight + (Math::RandFloat() * 2.0f));
        };

        //
        //  init rotation
        //
        RwV3d vAxis = { 1.0f, 1.0f, 1.0f };
        float fAngle = MATH_DEG2RAD(Math::RandFloat() * 180.0f);
        CGimmickUtils::QuaternionFromAxisAngle(&param.m_quat, &vAxis, fAngle);

        //
        //  init subid & name
        //
        param.m_subid = CFallGimmick::SUBID_SCRAP;
        CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_N_FALOBJ, m_type, m_nTotalCreatedNum % 1000);

        //
        //  make new fall gimmick
        //
        CFallGimmick* pFallGimmick = static_cast<CFallGimmick*>(CGimmickManager::Create(GIMMICKID::ID_N_FALOBJ, m_type, &param));
        ASSERT(pFallGimmick);

        ++m_nTotalCreatedNum;

        //
        //  init & activate fall gimmick
        //
        pFallGimmick->SetPower(m_nRubbleDamage);
        pFallGimmick->SetInterval(m_fTimerUntilFall);
        pFallGimmick->SetTarget(CHitAttackData::TARGET_PLAYER);
        pFallGimmick->SetBodyHitRadiusAutoChangeEnable(false);

        CGameSound::PlayObjectSE(this, SDCODE_SE(4405));

        CGimmickManager::PostEvent(pFallGimmick->GetName(), GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
    };
};


void CFallRubbleGimmickManager::getRandomRubblePos(RwV3d* pvRet, RUBBLEINFO* pInfo)
{
    CGameObject* pObj = CGameObjectManager::GetObject(pInfo->m_hObj);
    ASSERT(pObj);
    ASSERT(pObj->GetType() == GAMEOBJECTTYPE::CHARACTER);

    if (pObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    RwV3d vTempPos = Math::VECTOR3_ZERO;

    switch (m_type)
    {
    case TYPE_M58OB:
        {
            vTempPos.x = Math::RandFloatRange(-1.0f, 1.0f) * 2.5f;
            vTempPos.y = Math::RandFloat() * m_fRubbleRandHeight;
            vTempPos.z = Math::RandFloat() + 1.0f;

            RwV3dTransformPoint(pvRet, &vTempPos, &pInfo->m_matrix);
        }
        break;

    case TYPE_HUN:
        {
            vTempPos.x = Math::RandFloatRange(-1.0f, 1.0f) * 1.5f;
            vTempPos.y = Math::RandFloat() * m_fRubbleRandHeight;
            vTempPos.z = Math::RandFloatRange(-1.0f, 1.0f) * 1.5f;

            RwV3dTransformPoint(pvRet, &vTempPos, &pInfo->m_matrix);
        }
        break;

    case TYPE_HUN2:
        {
            vTempPos.x = Math::RandFloatRange(-1.0f, 1.0f) * 2.0f;
            vTempPos.y = Math::RandFloat() * m_fRubbleRandHeight;
            vTempPos.z = Math::RandFloatRange(-1.0f, 1.0f) * 2.0f;

            RwV3dTransformPoint(pvRet, &vTempPos, &pInfo->m_matrix);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};