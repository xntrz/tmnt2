#include "UtromGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/UtromGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


/*static*/ int32 CUtromGimmick::m_iUtromID = 0;


CUtromGimmick::CUtromGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_eState(STATE_WAIT)
, m_fCounter(0.0f)
, m_model()
, m_pBodyHitData(nullptr)
, m_pModuleMan(nullptr)
, m_pGimmickMotion(nullptr)
, m_pUtromMove(nullptr)
{
    struct INITPARAM
    {
        const char* pszModelName;
        const char* pszMotionSetName;
        const char* pszMotionName;
        float fHitRadius;
    };

    struct MOTIONPARAM
    {
        const char* pszMotionName;
        float fOffset;
    };

    static const INITPARAM s_aInitParam[] =
    {
        { "utrom0", "utrom", "Idle", 0.3f },
        { "utrom1", "utrom", "Idle", 0.3f },
        { "utrom2", "utrom", "Idle", 0.3f },
        { "utrom3", "utrom", "Idle", 0.3f },
        { "utrom4", "utrom", "Idle", 0.3f },
    };

    static const MOTIONPARAM s_aMotionParam[] =
    {
        { "Idle",   0.0f    },
        { "Idle",   0.5f    },
        { "Idle2",  0.0f    },
        { "Idle2",  0.25f   },
    };

    const int32 iUtromID = m_iUtromID++;
    const INITPARAM* pInitParam = &s_aInitParam[iUtromID % COUNT_OF(s_aInitParam)];
    const MOTIONPARAM* pMotionParam = &s_aMotionParam[iUtromID % COUNT_OF(s_aMotionParam)];

    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);
    
    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel(pInitParam->pszModelName);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.UpdateFrame();
    
    //
    //  init movement
    //
    m_pUtromMove = new CUtromGimmickMove;
    m_pUtromMove->SetRadius(pInitParam->fHitRadius);
    m_pUtromMove->SetRotation(&vRotation);
    m_pUtromMove->SetPosition(&pBasicParam->m_vPosition);
    m_pUtromMove->Start();

    //
    //  init module man
    //
    float fShadowWH = (pInitParam->fHitRadius * 3.0f);
    
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, fShadowWH, fShadowWH, true));

    //
    //  init motion
    //
    CMotionManager::SetCurrentMotionSet(pInitParam->pszMotionSetName);
    
    m_pGimmickMotion = new CGimmickMotion(pModel);
    m_pGimmickMotion->AddMotion(pMotionParam->pszMotionName);
    m_pGimmickMotion->SetMotion(pMotionParam->pszMotionName, 0.0f, 0.0f, 0.0f, false);
    m_pGimmickMotion->SetTime(m_pGimmickMotion->GetEndTime() * pMotionParam->fOffset);
    m_pGimmickMotion->StartRepeat(1.0f);

    //
    //  init body hit
    //
    m_pBodyHitData = CBodyHitManager::AllocData();
    ASSERT(m_pBodyHitData);
    
    m_pBodyHitData->InitData(&pBasicParam->m_vPosition, pInitParam->fHitRadius);
    m_pBodyHitData->SetState(CBodyHitData::STATE_TODO_0x2, true);

    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pUtromMove);

    //
    //  reset utrom generator
    //
    if (m_iUtromID == UTROM_MAX)
        m_iUtromID = 0;
};


CUtromGimmick::~CUtromGimmick(void)
{
    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };

    if (m_pGimmickMotion)
    {
        delete m_pGimmickMotion;
        m_pGimmickMotion = nullptr;
    };

    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };

    if (m_pUtromMove)
    {
        delete m_pUtromMove;
        m_pUtromMove = nullptr;
    };
};


void CUtromGimmick::MessageProc(int32 nMessageID, void* pParam)
{
    CGimmick::MessageProc(nMessageID, pParam);

    switch (nMessageID)
    {
    case CHARACTERTYPES::MESSAGEID_CATCH:
        OnCatch(static_cast<CHitCatchData::RESULT*>(pParam));
        break;

    case CHARACTERTYPES::MESSAGEID_LIFT:
        OnLift(static_cast<CCharacter::MSG_LIFT_INFO*>(pParam));
        break;

    case CHARACTERTYPES::MESSAGEID_THROW:
        OnThrow(static_cast<RwV3d*>(pParam));
        break;

    case CHARACTERTYPES::MESSAGEID_MISSTHROW:
        OnMissThrow(static_cast<RwV3d*>(pParam));
        break;

    default:
        break;
    };
};


void CUtromGimmick::Draw(void) const
{
    CGimmick::Draw();

    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


void CUtromGimmick::PreMove(void)
{
    ;
};


void CUtromGimmick::PostMove(void)
{
    switch (m_eState)
    {
    case STATE_WAIT:
        {
            //
            //  update catch hit
            //
            RwSphere HitSphere = { 0 };
            m_pUtromMove->GetPosition(&HitSphere.center);
            HitSphere.radius = m_pUtromMove->GetRadius();
        
            CHitCatchData CatchData;
            CatchData.Cleanup();
            CatchData.SetObject(GetHandle());
            CatchData.SetObjectType(GetType());
            CatchData.SetShape(CHitCatchData::SHAPE_SPHERE);
            CatchData.SetSphere(&HitSphere);

            CHitAttackManager::RegistCatch(&CatchData);

            //
            //  update body hit
            //
            RwV3d vMovVel = Math::VECTOR3_ZERO;
            m_pUtromMove->GetVelocity(&vMovVel);
            
            RwV3d vMovPos = Math::VECTOR3_ZERO;
            m_pUtromMove->GetPosition(&vMovPos);

            m_pBodyHitData->SetCurrentPos(&vMovPos);

            RwV3d vResultVelocity = Math::VECTOR3_ZERO;
            if (CBodyHitManager::CheckHit(m_pBodyHitData, &vMovVel, &vResultVelocity))
            {
                Math::Vec3_Scale(&vResultVelocity, &vResultVelocity, 0.01f);

                RwV3d vNewPos = Math::VECTOR3_ZERO;
                Math::Vec3_Add(&vNewPos, &vMovPos, &vResultVelocity);
                
                m_pUtromMove->SetPosition(&vNewPos);
                m_pBodyHitData->SetCurrentPos(&vNewPos);
            };
        }
        break;

    case STATE_LIFTED:
        {
            ;
        }
        break;

    case STATE_WARP:
        {
            if (m_fCounter >= 0.0f)
            {
                CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

                RwRGBA MatColor = pMdl->GetColor();
                MatColor.alpha = uint8(255.0f * (m_fCounter / 0.5f));
                pMdl->SetColor(MatColor);

                m_fCounter -= CGameProperty::GetElapsedTime();
            }
            else
            {
                m_eState = STATE_WARP_OUT;
            };
        }
        break;

    case STATE_WARP_OUT:
        {
            m_eState = STATE_RELEASE;
        }
        break;

    case STATE_RELEASE:
        {
            Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };

#ifdef _DEBUG    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pUtromMove->GetPosition(&vPosition);
    ASSERT(vPosition.y > -10.0f);
#endif    

    m_pGimmickMotion->Update();

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


void CUtromGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    if (m_eState != STATE_WAIT)
        return;

    m_pUtromMove->Stop();
    m_eState = STATE_WARP;
    m_fCounter = 0.5f;
    
    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };

    bool bSoundCall = false;
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pUtromMove->GetPosition(&vPosition);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_START, &vPosition, bSoundCall);
    ASSERT(hEffect);

    CGameSound::PlayObjectSE(this, SDCODE_SE(4205));
};


void CUtromGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_eState != STATE_WAIT)
        return;

    if (pAttack->GetStatus() != CHitAttackData::STATUS_THROW)
        return;

    pAttack->GetCatch()->SetResult(CHitCatchData::RESULT_THROWFRONT);
};


void CUtromGimmick::OnCatch(CHitCatchData::RESULT* pResult)
{
    if ((*pResult == CHitCatchData::RESULT_THROWFRONT) ||
        (*pResult == CHitCatchData::RESULT_THROWBACK))
    {
        m_pUtromMove->SetRotation(&Math::VECTOR3_ZERO);

        if (m_pBodyHitData)
            m_pBodyHitData->SetState(CBodyHitData::STATE_TODO_0x2, false);

        m_eState = STATE_LIFTED;
    };
};


void CUtromGimmick::OnLift(CCharacter::MSG_LIFT_INFO* pMsgLiftInfo)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;

    if ((pMsgLiftInfo->m_iStatus == PLAYERTYPES::STATUS_LIFT) ||
        (pMsgLiftInfo->m_iStatus == PLAYERTYPES::STATUS_LIFT_WALK))
    {
        vPosition = pMsgLiftInfo->m_vPosition;
    }
    else
    {
        m_pUtromMove->GetPosition(&vPosition);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vVelocity, &pMsgLiftInfo->m_vPosition, &vPosition);

        if (CWorldMap::CheckCollisionSphereMove(&pMsgLiftInfo->m_vPosition, m_pUtromMove->GetRadius(), &vVelocity, CWorldMap::CHECKFLAG_NONE))
        {
            m_pUtromMove->GetPosition(&vPosition);
            vPosition.y = pMsgLiftInfo->m_vPosition.y;
        }
        else
        {
            vPosition = pMsgLiftInfo->m_vPosition;
        };
    };

    vPosition.y += 0.4f;

    m_pUtromMove->SetPosition(&vPosition);
    m_pUtromMove->Stop();

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pUtromMove->GetRotation(&vRotation);    
    vRotation.y = pMsgLiftInfo->m_fDirection;
    m_pUtromMove->SetRotation(&vRotation);
    
    m_model.SetRotation(&vRotation);
};


void CUtromGimmick::OnMissThrow(RwV3d* pvVelocity)
{
    onThrow(pvVelocity, true);
};


void CUtromGimmick::OnThrow(RwV3d* pvVelocity)
{
    onThrow(pvVelocity, false);
};


void CUtromGimmick::onThrow(RwV3d* pvVelocity, bool bMiss)
{
    RwV3d vVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vVelocity, pvVelocity, (bMiss ? 5.0f : 10.0f));

    m_pUtromMove->SetVelocity(&vVelocity);
    m_pUtromMove->Start();

    if (m_pBodyHitData)
        m_pBodyHitData->SetState(CBodyHitData::STATE_TODO_0x2, true);

    m_eState = STATE_WAIT;
};


//
// *********************************************************************************
//


CUtromAreaCheckGimmick::CUtromAreaCheckGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_state(STATE_WARNING)
, m_fRadius(0.0f)
, m_fHeight(0.0f)
, m_iUtromCount(0)
, m_szClearTargetName()
, m_listUtrom()
{
    GIMMICKPARAM::GIMMICK_TERMS_AREA_BOX* pTermsParam = static_cast<GIMMICKPARAM::GIMMICK_TERMS_AREA_BOX*>(pParam);

    ASSERT(pTermsParam);

    m_listUtrom.clear();

    m_vPosition = pTermsParam->m_vPosition;
    std::strcpy(m_szClearTargetName, pTermsParam->m_szTargetGimmick);
    m_iUtromCount = 0;
    m_fRadius = pTermsParam->m_box.x * 0.5f;
    m_fHeight = pTermsParam->m_box.y;
    m_state = STATE_INIT;
};


CUtromAreaCheckGimmick::~CUtromAreaCheckGimmick(void)
{
    auto it = m_listUtrom.begin();
    while (!m_listUtrom.empty())
    {
        CListNode<CUtromGimmick>* pNode = it.node();
        it = m_listUtrom.erase(it);

        pNode->data = nullptr;
        delete pNode;
    };
};


void CUtromAreaCheckGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_INIT:
        AreaCheckInit();
        break;

    case STATE_ON:
        AreaCheckOn();
        break;

    default:
        break;
    };
};


void CUtromAreaCheckGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    ;
};


void CUtromAreaCheckGimmick::AreaCheckInit(void)
{
    char szObjName[GAMEOBJECTTYPES::NAME_MAX];
    szObjName[0] = '\0';

    std::sprintf(szObjName, "%s_A", CGimmickInfo::GetBaseName(GIMMICKID::ID_N_UTROM));

    CGameObject* pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
    while (pGameObj)
    {
        ASSERT(pGameObj->GetType() == GAMEOBJECTTYPE::GIMMICK);
        CGimmick* pGimmick = static_cast<CGimmick*>(pGameObj);

        if (!std::strncmp(pGimmick->GetName(), szObjName, std::strlen(szObjName)))
        {
            CListNode<CUtromGimmick>* pNode = new CListNode<CUtromGimmick>;
            pNode->data = static_cast<CUtromGimmick*>(pGimmick);

            m_listUtrom.push_back(pNode);
            ++m_iUtromCount;
        };

        pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pGameObj);
    };

    m_state = STATE_ON;
};


void CUtromAreaCheckGimmick::AreaCheckOn(void)
{
    auto it = m_listUtrom.begin();
    while (it != m_listUtrom.end())
    {
        CListNode<CUtromGimmick>* pNode = it.node();
        CUtromGimmick* pUtromGimmick = pNode->data;

        if (!pUtromGimmick)
        {
            ++it;
            continue;
        };

        RwV3d vPosition = Math::VECTOR3_ZERO;
        pUtromGimmick->m_pUtromMove->GetPosition(&vPosition);

        if ((vPosition.y >= m_vPosition.y) && ((m_vPosition.y + m_fHeight) >= vPosition.y))
        {
            RwV3d vPos = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vPos, &vPosition, &m_vPosition);
            vPos.y = 0.0f;
            
            float fDist = Math::Vec3_Length(&vPos);
            
            if ((fDist <= m_fRadius) && (pUtromGimmick->m_eState == CUtromGimmick::STATE_WAIT))
                CGimmickManager::PostEvent( pUtromGimmick->GetName(), GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
        };

        if (pUtromGimmick->m_eState == CUtromGimmick::STATE_WARP_OUT)
        {
            pNode->data = nullptr;
            --m_iUtromCount;

            if (m_iUtromCount == 15)
                CMessageManager::Request(SEGROUPID::VALUE(98));
            else if (m_iUtromCount == 5)
                CMessageManager::Request(SEGROUPID::VALUE(99));
        };

        ++it;
    };

    if (m_iUtromCount <= 0)
        CGimmickManager::PostEvent(m_szClearTargetName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
};