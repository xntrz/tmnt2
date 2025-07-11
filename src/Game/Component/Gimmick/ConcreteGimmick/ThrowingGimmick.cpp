#include "ThrowingGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/ItemInfo.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/ThrowingGimmickMove.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


static const char* ATARI_NAME_EMPTY = "";


const CThrowingGimmick::KINDINFO CThrowingGimmick::m_aThrowingGimmickKindInfoList[] =
{
    { "drum",			ATARI_NAME_EMPTY,	0.8f, 0.8f, 1.5f, 50, 1, false, SDCODE_SE(0x102B) },
    { "drum",			ATARI_NAME_EMPTY,	0.8f, 0.8f, 1.5f, 50, 1, false, SDCODE_SE(0x102B) },
    { "drum_ekitai",	ATARI_NAME_EMPTY,	0.8f, 0.8f, 1.5f, 50, 1, false, SDCODE_SE(0x1077) },
    { "drum_sleep",		ATARI_NAME_EMPTY,	0.8f, 0.8f, 1.5f, 50, 1, false, SDCODE_SE(0x1076) },
    { "box",			"box",				0.75f,0.8f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "vase",			ATARI_NAME_EMPTY,	0.6f, 1.0f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "trashbox_l",		"trashbox_l_a",		1.5f, 1.0f, 2.5f, 50, 1, true,	SDCODE_SE(0x1013) },
    { "rock_s",			ATARI_NAME_EMPTY,	0.5f, 0.5f, 1.5f, 40, 15,false, SDCODE_SE(0x1013) },
    { "rock_l",			ATARI_NAME_EMPTY,	1.0f, 1.0f, 2.5f, 60, 25,true,	SDCODE_SE(0x1013) },
    { "desk",			"desk_a",			1.5f, 1.0f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "chair",			"chair",			1.0f, 0.5f, 1.5f, 30, 1, false,	SDCODE_SE(0x1013) },
    { "candlestick",	ATARI_NAME_EMPTY,	0.5f, 0.5f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "trashbox_s",		ATARI_NAME_EMPTY,	0.5f, 0.5f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
};


const CThrowingGimmick::KINDINFO CThrowingGimmick::m_aItemBoxGimmickKindInfoList[] =
{
    { "box",			"box",				0.75f,0.5f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "vase",			ATARI_NAME_EMPTY,	1.0f, 1.0f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
    { "vase",			ATARI_NAME_EMPTY,	1.0f, 1.0f, 1.5f, 30, 1, false, SDCODE_SE(0x1013) },
};


CThrowingGimmick::CThrowingGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pThrowingMove(nullptr)
, m_state(STATE_NONE)
, m_type(TYPE_NORMAL)
, m_subid(0)
, m_nItem(0)
, m_fTimer(0.0f)
, m_fTimerPlayerRideOnJudge(0.0f)
, m_pBodyHitData(nullptr)
, m_pModuleManager(nullptr)
, m_hAtari(0)
, m_pKindinfo(nullptr)
, m_hKagaribiEffect(0)
{
    init(pParam);

    m_pModuleManager = new CModuleManager;
    ASSERT(m_pModuleManager);

    float fShadowRadius = m_pKindinfo->m_fShadowRadius;

    CCircleShadowModule* pShadowMod = CCircleShadowModule::New(this, fShadowRadius, fShadowRadius, true);
    ASSERT(pShadowMod != nullptr);

    m_pModuleManager->Include(pShadowMod);
};


CThrowingGimmick::~CThrowingGimmick(void)
{
    if (m_hKagaribiEffect)
    {
        CEffectManager::Finish(m_hKagaribiEffect);
        m_hKagaribiEffect = 0;
    };

    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = nullptr;
    };

    if (m_pThrowingMove)
    {
        delete m_pThrowingMove;
        m_pThrowingMove = nullptr;
    };

    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CThrowingGimmick::MessageProc(int32 nMessageID, void* pParam)
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


void CThrowingGimmick::Draw(void) const
{
    CGimmick::Draw();
};


void CThrowingGimmick::GetCenterPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    
    if (m_pThrowingMove)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pThrowingMove->GetPosition(&vPosition);

        vPosition.y += m_pKindinfo->m_fRadius;

        *pvPosition = vPosition;
    };
};


GIMMICKTYPES::FEATURE CThrowingGimmick::GetFeatures(void) const
{
    return (GIMMICKTYPES::FEATURE_HOMING | GIMMICKTYPES::FEATURE_ABLE_SLEEP);
};


void CThrowingGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_NONE:
        m_state = STATE_WAIT;
        break;
        
    case STATE_WAIT:
        waiting();
        break;
        
    case STATE_INFORM:
        inform();
        break;
        
    case STATE_LIFTED:
        lifting();
        break;
        
    case STATE_THROWN:
        throwing();
        break;
        
    case STATE_BROKEN:
        broken();
        break;
        
    case STATE_END:
        Release();
        break;
        
    default:
        ASSERT(false);
        break;
    };

    updateModule();

    if (m_type == TYPE_NORMAL && m_subid == SUBID_THROWING::CANDLESTICK)
    {
        if (m_state >= STATE_WAIT && m_state < STATE_BROKEN)
            updateKagaribiFireEffect();
    };
};


void CThrowingGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        CGameObject* pObj = CGameObjectManager::GetObject(pszSender);
        if (pObj)
        {
            if (pObj->GetType() == GAMEOBJECTTYPE::CHARACTER)
            {
                m_fTimerPlayerRideOnJudge = 0.0f;
            }
            else if (pObj->GetType() == GAMEOBJECTTYPE::GIMMICK)
            {
                onBreak();
            };
        };
    };
};


void CThrowingGimmick::OnTouchedDown(void)
{
    if (m_state == STATE_THROWN)
        onBreak();
};


void CThrowingGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_state != STATE_WAIT)
        return;
    
    CGameObject* pAttacker = pAttack->GetObject();
    if (pAttack->GetStatus() == CHitAttackData::STATUS_THROW)
    {        
        if (isPlayerEnableLift(pAttacker))
        {
            onInform();
            pAttack->GetCatch()->SetResult(CHitCatchData::RESULT_THROWFRONT);
        };
    }
    else
    {
        CGimmickUtils::PlayHitEffect(pAttack);

        m_nLife -= pAttack->GetPower();
        if (m_nLife < 0)
        {
            onBreak();
            if (m_type != TYPE_ITEMBOX)
            {
                if (!(Math::Rand() % 3)
                    && (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER)
                    && (static_cast<CCharacter*>(pAttacker)->GetCharacterType() == CCharacter::TYPE_PLAYER))
                {
                    CMessageManager::Request(SEGROUPID::VALUE(9));
                };                
            };
        };
    };
};


void CThrowingGimmick::OnCatch(CHitCatchData::RESULT* pResult)
{
    if ((*pResult == CHitCatchData::RESULT_THROWFRONT) ||
        (*pResult == CHitCatchData::RESULT_THROWBACK))
    {
        m_pThrowingMove->SetRotation(&Math::VECTOR3_ZERO);

        if (m_pBodyHitData)
            m_pBodyHitData->SetState(CBodyHitData::STATE_ENABLE, false);

        if (m_hAtari)
        {
            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;
        };

        m_state = STATE_LIFTED;
    };
};


void CThrowingGimmick::OnLift(CCharacter::MSG_LIFT_INFO* pMsgLiftInfo)
{
    ASSERT(pMsgLiftInfo);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;    
    setLiftGimmickPosition(&vPosition, pMsgLiftInfo);

    m_pThrowingMove->SetPosition(&vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetRotation(&vRotation);
    vRotation.y = pMsgLiftInfo->m_fDirection;
    vRotation.z = -MATH_PI05;
    m_pThrowingMove->SetRotation(&vRotation);
    m_model.SetRotation(&vRotation);
};


void CThrowingGimmick::OnMissThrow(RwV3d* pvVelocity)
{
	setThrowingParam(pvVelocity);
    m_state = STATE_THROWN;
};


void CThrowingGimmick::OnThrow(RwV3d* pvVelocity)
{
    setThrowingParam(pvVelocity);
    m_state = STATE_THROWN;
    
    if (!(Math::Rand() % 3))
    {
        switch (m_subid)
        {
        case SUBID_THROWING::DRUM:
        case SUBID_THROWING::DRUM_1:
            CMessageManager::Request(SEGROUPID::VALUE(14));
            break;

        case SUBID_THROWING::DRUM_EKITAI:
            CMessageManager::Request(SEGROUPID::VALUE(16));
            break;

        case SUBID_THROWING::DRUM_SLEEP:
            CMessageManager::Request(SEGROUPID::VALUE(18));
            break;

        default:
            CMessageManager::Request(SEGROUPID::VALUE(19));
            break;
        };
    };
};


void CThrowingGimmick::init(void* pParam)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);    
    m_subid = pInitParam->m_subid;

    bool bIsBoxNotEmpty = false;
    
    if (pInitParam->m_id == GIMMICKID::ID_N_ITEMBX)
    {
        ASSERT(m_subid >= 0);
        ASSERT(m_subid < COUNT_OF(m_aItemBoxGimmickKindInfoList));

        m_pKindinfo = &m_aItemBoxGimmickKindInfoList[m_subid];
        m_type = TYPE_ITEMBOX;

        GIMMICKPARAM::GIMMICK_ITEMBOX* pItemInitParam = static_cast<GIMMICKPARAM::GIMMICK_ITEMBOX*>(pParam);
        m_nItem = pItemInitParam->m_nItem;
        
        changeItemBySituation();
        ASSERT(CItemInfo::IsValidID(ITEMID::VALUE(m_nItem)));

        if (m_nItem)
            bIsBoxNotEmpty = true;
    }
    else
    {
        ASSERT(m_subid >= 0);
        ASSERT(m_subid < COUNT_OF(m_aThrowingGimmickKindInfoList));

        m_pKindinfo = &m_aThrowingGimmickKindInfoList[m_subid];
    };

    m_nLife = m_pKindinfo->m_nLife;

    //
    //  Init model & atari
    //
    CModel* pDispModel = CModelManager::CreateModel(m_pKindinfo->m_pszModelName);
    ASSERT(pDispModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pDispModel);

    if (!std::strcmp(m_pKindinfo->m_pszAtariModelName, ATARI_NAME_EMPTY))
    {
        m_pBodyHitData = CBodyHitManager::AllocData();
        ASSERT(m_pBodyHitData);

        RwV3d vPosition = pInitParam->m_vPosition;
        vPosition.y += m_pKindinfo->m_fRadius;

        m_pBodyHitData->InitData(&vPosition, m_pKindinfo->m_fRadius);
        m_pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);
    }
    else
    {
        CModel* pAtariModel = CModelManager::CreateModel(m_pKindinfo->m_pszAtariModelName);
        ASSERT(pAtariModel);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);

        if (m_model.GetCollisionModelClump())
        {
            RpClump* pClump = m_model.GetCollisionModelClump();

            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_THROWOBJ);
            if (m_hAtari)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, m_pKindinfo->m_fRadius * 1.5f);
        };
    };

    m_model.SetPosition(&pInitParam->m_vPosition);
    
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    m_pThrowingMove = new CThrowingGimmickMove;
    m_pThrowingMove->SetPosition(&pInitParam->m_vPosition);
    m_pThrowingMove->SetRadius(m_pKindinfo->m_fRadius);
    m_pThrowingMove->Start();
    SetMoveStrategy(m_pThrowingMove);

    //
    //  Init effect
    //
    if ((m_type == TYPE_NORMAL) && (m_subid == SUBID_THROWING::CANDLESTICK))
    {
        RwV3d vPosition = pInitParam->m_vPosition;
        vPosition.y += 2.0f;
        
        m_hKagaribiEffect = CEffectManager::Play(EFFECTID::ID_FIRE_TORCH, &vPosition);
        if (m_hKagaribiEffect)
            CEffectManager::SetScale(m_hKagaribiEffect, 1.5f);
    };

    //
    //  Post init
    //
    if ((m_type == TYPE_ITEMBOX) && (m_nItem == ITEMID::ID_DON_LASER))
    {
        if (m_hAtari)
        {
            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;
        };

        if (!CGameData::Record().Secret().IsDonLazerEnabled())
            createItem();
        
        m_state = STATE_END;
    }
    else
    {
        if (bIsBoxNotEmpty)
            CGameEvent::SetItemBoxPut();
    };
};


void CThrowingGimmick::waiting(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetPosition(&vPosition);

    vPosition.y += m_pKindinfo->m_fRadius;

    RwSphere hitSphere;
    hitSphere.center = vPosition;
    hitSphere.radius = m_pKindinfo->m_fRadius;

    {
        CHitCatchData hitCatch;
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);

        CHitAttackManager::RegistCatch(&hitCatch);
    }

    if (m_fTimerPlayerRideOnJudge > 10000.0f)
        m_fTimerPlayerRideOnJudge -= 10000.0f;

    m_fTimerPlayerRideOnJudge += CGameProperty::GetElapsedTime();
};


void CThrowingGimmick::inform(void)
{
    m_state = STATE_WAIT;
};


void CThrowingGimmick::lifting(void)
{
    ;
};


void CThrowingGimmick::throwing(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetRotation(&vRotation);
    m_model.SetRotation(&vRotation);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetPosition(&vPosition);

    RwSphere hitSphere;
    hitSphere.center = vPosition;
    hitSphere.radius = m_pKindinfo->m_fHitAttackRadius;

    CHitAttackData hitAttack;
    hitAttack.SetObject(GetHandle());
    hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
    hitAttack.SetSphere(&hitSphere);
    hitAttack.SetPower(m_pKindinfo->m_nAttackPower);
    hitAttack.SetTarget(CHitAttackData::TARGET_ENEMY_GIMMICK);
    
    CHitAttackManager::RegistAttack(&hitAttack);
};


void CThrowingGimmick::broken(void)
{
    switch (m_type)
    {
    case TYPE_NORMAL:
        {
            if (m_subid == SUBID_THROWING::CANDLESTICK)
            {
                if (m_hKagaribiEffect)
                {
                    float fScale = 1.0f - (m_fTimer * 0.25f);
                    CEffectManager::SetScale(m_hKagaribiEffect, fScale);
                };

                m_fTimer += CGameProperty::GetElapsedTime();
                if (m_fTimer > 4.0f)
                    m_state = STATE_END;
            }
            else
            {
                m_state = STATE_END;
            };
        }
        break;
        
    case TYPE_ITEMBOX:
        {
            m_state = STATE_END;
        }        
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CThrowingGimmick::onInform(void)
{
    m_state = STATE_INFORM;
};


void CThrowingGimmick::onBreak(void)
{
    setBreakEffect();
    m_model.SetDrawEnable(false);

    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_type == TYPE_ITEMBOX)
        createItem();

    m_state = STATE_BROKEN;
};


void CThrowingGimmick::setBreakEffect(void)
{
    switch (m_type)
    {
    case TYPE_NORMAL:
        {
            if (m_subid == SUBID_THROWING::CANDLESTICK)
                setKagaribiBreakEffect();
            else
                setDefaultBreakEffect();
        }
        break;

    case TYPE_ITEMBOX:
        {
            setDefaultBreakEffect();
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_pKindinfo->m_nBreakSE)
        CGameSound::PlayObjectSE(this, m_pKindinfo->m_nBreakSE);

    if (!(Math::Rand() % 3))
    {
        switch (m_subid)
        {
        case SUBID_THROWING::DRUM:
        case SUBID_THROWING::DRUM_1:
            CMessageManager::Request(SEGROUPID::VALUE(13));
            break;
            
        case SUBID_THROWING::DRUM_EKITAI:
            CMessageManager::Request(SEGROUPID::VALUE(15));
            break;
            
        case SUBID_THROWING::DRUM_SLEEP:
            CMessageManager::Request(SEGROUPID::VALUE(17));
            break;

        default:
            break;
        };
    };
};


void CThrowingGimmick::setKagaribiBreakEffect(void)
{
    if (m_hKagaribiEffect)
    {
        CEffectManager::Finish(m_hKagaribiEffect);
        m_hKagaribiEffect = 0;
    };

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetPosition(&vPosition);

    m_hKagaribiEffect = CEffectManager::Play(EFFECTID::ID_FLAME_MAP, &vPosition);
    if (m_hKagaribiEffect)
        CEffectManager::SetScale(m_hKagaribiEffect, 2.0f);
};


void CThrowingGimmick::setDefaultBreakEffect(void)
{
	STAGEID::VALUE      idStage = CGameData::PlayParam().GetStage();
	MAPID::VALUE        idMap = CStageInfo::GetMapID(idStage);
	CMapInfo::CATEGORY  catMap = CMapInfo::GetCategory(idMap);
    
	EFFECTID::VALUE idEffect = EFFECTID::ID_UNKNOWN;    
	MAGICID::VALUE	idMagic = MAGICID::ID_UNKNOWN;   

    float fScale = 1.0f;
    int32 nNumEffect = 1;

    if (m_type == TYPE_ITEMBOX)
    {
        if (m_subid != SUBID_ITEMBOX::VASE)
        {
            if ((catMap == CMapInfo::CATEGORY_SPACE) || (catMap == CMapInfo::CATEGORY_ZERO))
                idEffect = EFFECTID::ID_METAL;
            else
                idEffect = EFFECTID::ID_WOOD;
        }
        else
        {
            idEffect = EFFECTID::ID_CERAMICS;
        };
    }
    else
    {
        switch (m_subid)
        {
        case SUBID_THROWING::DRUM:
        case SUBID_THROWING::DRUM_1:
            idMagic = MAGICID::ID_EXPL_B1;
            fScale = 1.5f;
            break;

        case SUBID_THROWING::DRUM_EKITAI:
            idMagic = MAGICID::ID_FREEZEGAS;
            fScale = 1.5f;
            break;

        case SUBID_THROWING::DRUM_SLEEP:
            idMagic = MAGICID::ID_SLEEPGAS;
            fScale = 1.5f;
            break;

        case SUBID_THROWING::BOX:
            {
                if ((catMap == CMapInfo::CATEGORY_SPACE) || (catMap == CMapInfo::CATEGORY_ZERO))
                    idEffect = EFFECTID::ID_METAL;
                else
                    idEffect = EFFECTID::ID_WOOD;
            }
            break;

        case SUBID_THROWING::VASE:
            idEffect = EFFECTID::ID_CERAMICS;
            break;

        case SUBID_THROWING::TRASHBOX_L:
            nNumEffect = 2;
            idEffect = EFFECTID::ID_DUSTBOX;
            break;

        case SUBID_THROWING::ROCK_S:
            idEffect = EFFECTID::ID_ROCK;
            break;

        case SUBID_THROWING::ROCK_L:
            idEffect = EFFECTID::ID_ROCK;
            nNumEffect = 2;
            break;

        case SUBID_THROWING::DESK:
            idEffect = (catMap == CMapInfo::CATEGORY_JAPAN ? EFFECTID::ID_WOOD : EFFECTID::ID_METAL);
            nNumEffect = 3;
            break;

        case SUBID_THROWING::CHAIR:
            idEffect = (catMap == CMapInfo::CATEGORY_JAPAN ? EFFECTID::ID_WOOD : EFFECTID::ID_METAL);
            break;

        case SUBID_THROWING::CANDLESTICK:
            break;

        case SUBID_THROWING::TRASHBOX_S:
            idEffect = EFFECTID::ID_DUSTBOX;
            break;

        default:
            break;
        };
    };

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetPosition(&vPosition);

    if (idMagic != MAGICID::ID_UNKNOWN)
    {
        vPosition.y += m_pKindinfo->m_fRadius;        

        CMagicManager::CParameter param;
        param.SetPositon(&vPosition);
        param.SetObject(this);

        uint32 hMagic = CMagicManager::Play(idMagic, &param);
        if (hMagic)
            CMagicManager::SetScale(hMagic, fScale);
    }
    else if (idEffect != EFFECTID::ID_UNKNOWN)
    {
        for (int32 i = 0; i < nNumEffect; ++i)
        {
            uint32 hEffect = CEffectManager::Play(idEffect, &vPosition);
            if (hEffect)
                CEffectManager::SetScale(hEffect, fScale);
            
            vPosition.x += Math::RandFloat() * 0.1f;
            vPosition.z += Math::RandFloat() * 0.1f;
        };
    };
};


void CThrowingGimmick::setThrowingParam(RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vVelocity, pvVelocity, 12.0f);
    m_pThrowingMove->SetVelocity(&vVelocity);

    uint32 rand = ((Math::Rand() % TYPEDEF::UINT32_MAX) / TYPEDEF::UINT32_MAX);

    RwV3d vRotVelocity = Math::VECTOR3_ZERO;
    vRotVelocity.x = MATH_DEG2RAD(vVelocity.z * 10.0f);
    vRotVelocity.y = MATH_DEG2RAD((200.0f * static_cast<float>(rand)) - 100.0f);
    vRotVelocity.z = MATH_DEG2RAD(vVelocity.x * 10.0f);

	m_pThrowingMove->SetRotVelocity(&vRotVelocity);
};


void CThrowingGimmick::updateModule(void)
{
    if (m_pModuleManager)
    {
        m_pModuleManager->Run();
        
        CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(m_pModuleManager->GetModule(MODULETYPE::CIRCLE_SHADOW));
        if (pCircleShadowMod)
        {
            bool bIsWaiting = (m_state == STATE_WAIT);
            bool bIsLifted = (m_state == STATE_LIFTED);
            bool bIsThrown = (m_state == STATE_THROWN);
            bool bIsAtariModelEmpty = (std::strcmp(m_pKindinfo->m_pszAtariModelName, ATARI_NAME_EMPTY) == 0);

            pCircleShadowMod->SetEnable(bIsWaiting && bIsAtariModelEmpty || bIsLifted || bIsThrown);
        };
    };
};


void CThrowingGimmick::updateKagaribiFireEffect(void)
{
    if (m_hKagaribiEffect)
    {
        RwV3d vRotation = Math::VECTOR3_ZERO;
        m_pThrowingMove->GetRotation(&vRotation);
        
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pThrowingMove->GetPosition(&vPosition);
        
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        Math::Matrix_Rotate(&matrix, &vRotation);
        
        RwV3d vEffectOfs = { 0.0f, 2.0f, 0.0f };
        RwV3d vEffectPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vEffectPos, &vEffectOfs, &matrix);

        Math::Vec3_Add(&vEffectPos, &vEffectPos, &vPosition);
        
        CEffectManager::SetPosition(m_hKagaribiEffect, &vEffectPos);
    };
};


void CThrowingGimmick::createItem(void)
{
    ASSERT(m_type == TYPE_ITEMBOX);

    if (m_nItem == ITEMID::ID_NONE)
        return;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pThrowingMove->GetPosition(&vPosition);

    GIMMICKPARAM::GIMMICK_ITEMBOX param;
    param.m_id = GIMMICKID::ID_N_ITEM;
    param.m_subid = 0;
    param.m_size = sizeof(param);
    param.m_quat = { 0 };
    param.m_vPosition = vPosition;
	param.m_nItem = m_nItem;

    const char* pszName = GetName();
    int32 no = std::atol(&pszName[11]);
    CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_N_ITEM, 0, no);
    
    CGimmick* pGimmick = CGimmickManager::Create(GIMMICKID::ID_N_ITEM, 0, &param);
    ASSERT(pGimmick);
};


void CThrowingGimmick::changeItemBySituation(void)
{
    ASSERT(m_type == TYPE_ITEMBOX);

    AREAID::VALUE idAreaNow = CGameData::PlayParam().GetArea();

    switch (m_nItem)
    {
    case ITEMID::ID_ANTIQUE:
        {
            if (CGameData::Attribute().IsPlayDemoMode())
            {
                m_nItem = ITEMID::ID_NONE;
            }
            else if (CGameData::Record().Antique().IsAreaAntiqueTaken(idAreaNow))
            {
                m_nItem = ITEMID::ID_NONE;
            };
        }
        break;

    case ITEMID::ID_CRY_RED:
    case ITEMID::ID_CRY_GREEN:
    case ITEMID::ID_CRY_ORANGE:
    case ITEMID::ID_CRY_WHITE:
        {
            GAMETYPES::CRYSTALTYPE crytype = GAMETYPES::CRYSTALTYPE_NONE;

            switch (m_nItem)
            {
            case ITEMID::ID_CRY_RED:
                crytype = GAMETYPES::CRYSTALTYPE_RED;
                break;
                
            case ITEMID::ID_CRY_GREEN:
                crytype = GAMETYPES::CRYSTALTYPE_GREEN;
                break;
                
            case ITEMID::ID_CRY_ORANGE:
                crytype = GAMETYPES::CRYSTALTYPE_ORANGE;
                break;
                
            case ITEMID::ID_CRY_WHITE:
                crytype = GAMETYPES::CRYSTALTYPE_WHITE;
                break;

            default:
                ASSERT(false);
                break;
            };

            if (CGameData::Record().Item().IsAreaCrystalTaken(crytype, idAreaNow))
                m_nItem = ITEMID::ID_NONE;
        }
        break;

    case ITEMID::ID_HEAL_SMALL:
    case ITEMID::ID_HEAL_FULL:
        {
            if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_PIZZAPARADISE))
                m_nItem = ITEMID::ID_HEAL_FULL;

            if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_ENDURANCE))
                m_nItem = ITEMID::ID_NONE;
        }
        break;

    case ITEMID::ID_MISSILE:
        {
            if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_SHURIKEN))
                m_nItem = ITEMID::ID_NONE;            
        }
        break;

    default:
        break;
    };
};


void CThrowingGimmick::setLiftGimmickPosition(RwV3d* pvPosition, CCharacter::MSG_LIFT_INFO* pInfo)
{
    ASSERT(pvPosition);
    ASSERT(pInfo);

    STAGEID::VALUE      idStageNow  = CGameData::PlayParam().GetStage();
    MAPID::VALUE        idMapNow    = CStageInfo::GetMapID(idStageNow);
    CMapInfo::CATEGORY  catMap      = CMapInfo::GetCategory(idMapNow);

    float fOffsetH = 0.0f;

    switch (m_type)
    {
    case TYPE_NORMAL:
        {
            switch (m_subid)
            {
            case SUBID_THROWING::DRUM:
            case SUBID_THROWING::DRUM_1:
            case SUBID_THROWING::DRUM_EKITAI:
            case SUBID_THROWING::DRUM_SLEEP:
                fOffsetH = ((catMap == CMapInfo::CATEGORY_SPACE) || (catMap == CMapInfo::CATEGORY_ZERO)) ? 0.3f : 0.5f;
                break;

            case SUBID_THROWING::TRASHBOX_L:
                fOffsetH = 1.2f;
                break;
                
            case SUBID_THROWING::ROCK_L:
                fOffsetH = 0.8f;
                break;
                
            case SUBID_THROWING::DESK:
                fOffsetH = 1.0f;
                break;
                
            case SUBID_THROWING::CANDLESTICK:
                fOffsetH = 0.3f;
                break;
                
            default:
                fOffsetH = 0.5f;
                break;
            };
        }
        break;
        
    case TYPE_ITEMBOX:
        {
            fOffsetH = 0.5f;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    pvPosition->x = pInfo->m_vPosition.x + (Math::Sin(pInfo->m_fDirection) * 0.3f);
    pvPosition->y = pInfo->m_vPosition.y + fOffsetH;
    pvPosition->z = pInfo->m_vPosition.z + (Math::Cos(pInfo->m_fDirection) * 0.3f);
};


bool CThrowingGimmick::isPlayerEnableLift(const CGameObject* pGameObject)
{
    if (pGameObject->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return false;

    if (static_cast<const CCharacter*>(pGameObject)->GetCharacterType() != CCharacter::TYPE_PLAYER)
        return false;

    if (m_pKindinfo->m_bHeavy)
        return PlayerUtil::IsPowerfullCharacter(static_cast<const CPlayerCharacter*>(pGameObject));

    return (m_fTimerPlayerRideOnJudge > CGameProperty::GetElapsedTime());
};

