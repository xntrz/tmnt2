#include "SystemGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/SeGroupID.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/DebugShape.hpp"


CAndSwitchGimmick::CAndSwitchGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_privatestate(PRIVATESTATE_CHECK)
, m_szEventGimmick()
, m_szTargetGimmick()
{
    m_szEventGimmick[0] = '\0';
    m_szTargetGimmick[0] = '\0';

    GIMMICKPARAM::GIMMICK_TERMS_AND* pAndSwitchParam = static_cast<GIMMICKPARAM::GIMMICK_TERMS_AND*>(pParam);
    ASSERT(pAndSwitchParam);
    
    std::strcpy(m_szTargetGimmick, pAndSwitchParam->m_szTargetGimmick);
    std::strcpy(m_szEventGimmick, pAndSwitchParam->m_szEventGimmickName);
};


CAndSwitchGimmick::~CAndSwitchGimmick(void)
{
	;
};


void CAndSwitchGimmick::PostMove(void)
{
    switch (m_privatestate)
    {
    case PRIVATESTATE_CHECK:
        {
            if (checkSwitch())
            {
                CGimmickManager::PostEvent(m_szTargetGimmick, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
                m_privatestate = PRIVATESTATE_END;
            };
        }
        break;

    default:
        break;
    };
};


bool CAndSwitchGimmick::checkSwitch(void)
{
	CGimmick* pEventGimmick = CGimmickManager::Find(m_szEventGimmick);
    if (pEventGimmick)
    {
        CStateGimmickQuery query(GIMMICKTYPES::QUERY_SYS_SWITCH);
        pEventGimmick->Query(&query);

        if ((query.m_nState == query.m_nTarget) &&
            (query.m_nTarget > 0))
            return true;
    };

    return false;
};


//
// *********************************************************************************
//


void CGroupGimmick::CHILDNODE::Init(const char* pszName)
{
    ASSERT(pszName);
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));

    std::strcpy(m_szName, pszName);
};


const char* CGroupGimmick::CHILDNODE::GetName(void) const
{
    return m_szName;
};


CGimmick* CGroupGimmick::CHILDNODE::GetGimmick(void) const
{
    CGameObject* pGameObject = CGameObjectManager::GetObject(m_szName);
    if (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK);

        CGimmick* pGimmick = static_cast<CGimmick*>(pGameObject);
        return pGimmick;
	};
	
    return nullptr;
};


CGroupGimmick::CGroupGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pChildArray(nullptr)
, m_nChildNum(0)
{
    GIMMICKPARAM::GIMMICK_GROUP* pGroupParam = static_cast<GIMMICKPARAM::GIMMICK_GROUP*>(pParam);
    ASSERT(pGroupParam);

    if (pGroupParam->m_nGimmickNum > 0)
    {
        m_nChildNum = pGroupParam->m_nGimmickNum;
        m_pChildArray = new CHILDNODE[m_nChildNum];
    };

    for (int32 i = 0; i < m_nChildNum; ++i)
        m_pChildArray[i].Init(pGroupParam->m_aszGimmickName[i]);
};


CGroupGimmick::~CGroupGimmick(void)
{
    if (m_pChildArray)
    {
        delete[] m_pChildArray;
        m_pChildArray = nullptr;
        
        m_nChildNum = 0;
    };
};


bool CGroupGimmick::Query(CGimmickQuery* pQuery) const
{
    bool bResult = false;

    for (int32 i = 0; i < m_nChildNum; ++i)
    {
        CGimmick* pGimmick = GetChild(i);

        if (pGimmick && pGimmick->Query(pQuery))
            bResult = true;
    };

    return bResult;
};


void CGroupGimmick::OnReceiveEvent(const char* pszSender,
                                               GIMMICKTYPES::EVENTTYPE eventtype)
{
    for (int32 i = 0; i < m_nChildNum; ++i)
        CGimmickManager::PostEvent(getChildNode(i)->GetName(), pszSender, eventtype);
};


CGimmick* CGroupGimmick::GetChild(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < m_nChildNum);

    return getChildNode(no)->GetGimmick();
};


CGroupGimmick::CHILDNODE* CGroupGimmick::getChildNode(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < m_nChildNum);

    return &m_pChildArray[no];
};


//
// *********************************************************************************
//


CGameEndGimmick::CGameEndGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_mode(MODE_GAMECLEAR)
, m_clearsub(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    switch (pBasicParam->m_id)
    {
    case GIMMICKID::ID_S_GCLEAR:
        m_mode = MODE_GAMECLEAR;
        m_clearsub = pBasicParam->m_subid;
        break;

    case GIMMICKID::ID_S_GOVER:
        m_mode = MODE_GAMEOVER;
        m_clearsub = pBasicParam->m_subid;
        break;

    default:
        ASSERT(false);
        break;
    };
};


CGameEndGimmick::~CGameEndGimmick(void)
{
    ;
};


void CGameEndGimmick::OnReceiveEvent(const char* pszSender,
                                                 GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        switch (m_mode)
        {
        case MODE_GAMECLEAR:
            if (m_clearsub)
                CGameEvent::SetGameCleared(CGamePlayResult::CLEARSUB_B);
            else
                CGameEvent::SetGameCleared(CGamePlayResult::CLEARSUB_A);
            break;

        case MODE_GAMEOVER:
            CGameEvent::SetGameOvered();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


//
// *********************************************************************************
//


CCameraChangeGimmick::CCameraChangeGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_step(STEP_IDLE)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_fRotationY(0.0f)
, m_szSenderGimmick()
{
    m_szSenderGimmick[0] = '\0';

    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    m_vPosition = pBasicParam->m_vPosition;

    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pBasicParam->m_quat);
    
    m_vRotation = matrix.at;
    m_vRotation.y = 0.0f;
    Math::Vec3_Normalize(&m_vRotation, &m_vRotation);
    
    m_fRotationY = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);
};


CCameraChangeGimmick::~CCameraChangeGimmick(void)
{
    ;
};


void CCameraChangeGimmick::PostMove(void)
{
    switch (m_step)
    {
    case STEP_IDLE:
        break;

    case STEP_FADEOUT:
        {
            CScreenFade::BlackOut(1.0f);
            m_step = STEP_FADEOUT_CHECK;
            if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST43N)
                CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);
        }
        break;

    case STEP_FADEOUT_CHECK:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_REPLACE;
        }
        break;

    case STEP_REPLACE:
        {
            ReplacePlayers();
            
            CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
            ASSERT(pMapCamera);
            pMapCamera->UpdateMoment(&m_vPosition);
            
            if (std::strcmp(m_szSenderGimmick, ""))                
                CGimmickManager::PostEvent(m_szSenderGimmick, GetName(), GIMMICKTYPES::EVENTTYPE_INACTIVATE);

            if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST43N)
            {
                CGameSound::Stop();
                CGameSound::PlayBGM(SDCODE_BGM(12315));
            };

            m_step = STEP_FADEIN;
        }
        break;

    case STEP_FADEIN:
        {
            CScreenFade::BlackIn(1.0f);
            m_step = STEP_FADEIN_CHECK;
        }
        break;

    case STEP_FADEIN_CHECK:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_RELEASE;
        }
        break;
        
    case STEP_RELEASE:
        {
            Release();            
        }
        break;

    default:
        break;
    };
};


void CCameraChangeGimmick::OnReceiveEvent(const char* pszSender,
                                                      GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_step == STEP_IDLE)
        {
            ASSERT(pszSender);
            ASSERT(std::strcmp(pszSender, "") != 0);

            std::strcpy(m_szSenderGimmick, pszSender);
            m_step = STEP_FADEOUT;
        };
    };
};


void CCameraChangeGimmick::ReplacePlayers(void)
{
    const float afRotation[]
    {
        45.0f,
        315.0f,
        135.0f,
        225.0f,
    };

    static_assert(COUNT_OF(afRotation) == GAMETYPES::PLAYERS_MAX, "update me");

    RwV3d vDir = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vDir, &m_vRotation);
    Math::Vec3_Scale(&vDir, &vDir, 1.5f);

	int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
    {
        RwMatrix matrix;
        RwMatrixSetIdentity(&matrix);
        Math::Matrix_RotateY(&matrix, MATH_DEG2RAD(afRotation[i]));

        RwV3d vPosition = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vPosition, &vDir, &matrix);
        Math::Vec3_Add(&vPosition, &vPosition, &m_vPosition);

        float fHeight = CWorldMap::GetMapHeight(&vPosition);

        MAPTYPES::ATTRIBUTE attribute = CWorldMap::GetCollisionResultAttribute();
        if (attribute == MAPTYPES::ATTRIBUTE_DEATH)
            vPosition = m_vPosition;
        else
            vPosition.y = fHeight;

		IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(i);
		pGamePlayer->Relocation(&vPosition, m_fRotationY, true);
		pGamePlayer->Release();
    };
};


//
// *********************************************************************************
//


/*static*/ const CSEGimmick::SEINFO CSEGimmick::m_aSeInfoList[] =
{
    { SDCODE_SE(0x1000),	CSEGimmick::SEINFO::FLAG_NONE,  0.0f    },
    { SDCODE_SE(0x103A),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1095),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x0),		CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2007),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2012),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1090),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1098),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x108F),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x108E),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x108D),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1035),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2017),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2016),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2015),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2010),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2014),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x2013),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1064),	CSEGimmick::SEINFO::FLAG_LOOP,  2.0f    },
    { SDCODE_SE(0x1094),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
    { SDCODE_SE(0x1097),	CSEGimmick::SEINFO::FLAG_FADE,  0.0f    },
};


CSEGimmick::CSEGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_bPlaying(false)
, m_vPosition(Math::VECTOR3_ZERO)
, m_pNowSEInfo(nullptr)
, m_fTimer(0.0f)
{
    GIMMICKPARAM::GIMMICK_PLAYSE* pPlaySeParam = static_cast<GIMMICKPARAM::GIMMICK_PLAYSE*>(pParam);
    ASSERT(pPlaySeParam);
    
    ASSERT(pPlaySeParam->m_nSE >= 0);    
    ASSERT(pPlaySeParam->m_nSE < COUNT_OF(m_aSeInfoList));
    
	m_vPosition = pPlaySeParam->m_vPosition;
    m_pNowSEInfo = &m_aSeInfoList[pPlaySeParam->m_nSE];

    if (pPlaySeParam->m_subid == 1)
        startSE();
};


CSEGimmick::~CSEGimmick(void)
{
    if (m_bPlaying)
        CGameSound::StopSE(m_pNowSEInfo->m_nSE);
};


void CSEGimmick::GetPosition(RwV3d* pvPosition) const
{
	*pvPosition = m_vPosition;
};


void CSEGimmick::PostMove(void)
{
    if ((m_pNowSEInfo->m_flag & SEINFO::FLAG_LOOP) && m_bPlaying)
    {
        if (m_fTimer >= m_pNowSEInfo->m_fDuration)
        {
            CGameSound::PlayObjectSE(this, m_pNowSEInfo->m_nSE);
            m_fTimer -= m_pNowSEInfo->m_fDuration;
        };

        m_fTimer += CGameProperty::GetElapsedTime();
	};
};


void CSEGimmick::OnReceiveEvent(const char* pszSender,
                                            GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_bPlaying)
            stopSE();
        else
            startSE();
    };
};


void CSEGimmick::startSE(void)
{
    m_bPlaying = true;
	CGameSound::PlayObjectSE(this, m_pNowSEInfo->m_nSE);
};


void CSEGimmick::stopSE(void)
{
    m_bPlaying = false;
    if (m_pNowSEInfo->m_flag & SEINFO::FLAG_FADE)
        CGameSound::FadeOutSE(m_pNowSEInfo->m_nSE, CGameSound::FADESPEED_NORMAL);    
};


//
// *********************************************************************************
//


CInvisibleWallGimmick::CInvisibleWallGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_type(TYPE_5M_DISAPPEAR)
, m_model()
, m_hAtari(0)
, m_bEnable(false)
{
    GIMMICKPARAM::GIMMICK_BASIC* pWallParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pWallParam);

    const char* apszModel[] =
    {
        "wall5",
        "wall5",
        "wall20",
        "wall20",
    };

    static_assert(COUNT_OF(apszModel) == TYPENUM, "update me");

    CModel* pAtariModel = CModelManager::CreateModel(apszModel[pWallParam->m_subid]);
    ASSERT(pAtariModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);
    
#ifdef _DEBUG    
    CModel* pDispModel = CModelManager::CreateModel(apszModel[pWallParam->m_subid]);
    ASSERT(pDispModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pDispModel);
#endif /* _DEBUG */
        
    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pWallParam->m_quat);
    
    RwV3d vRotate = Math::VECTOR3_ZERO;
    CGimmickUtils::MatrixToRotation(&matrix, &vRotate);

    m_model.SetPosition(&pWallParam->m_vPosition);
    m_model.SetRotation(&vRotate);
    SetModelStrategy(&m_model);

    m_type = static_cast<TYPE>(pWallParam->m_subid);
    switch (m_type)
    {
    case TYPE_5M_DISAPPEAR:
    case TYPE_20M_DISAPPEAR:
        m_bEnable = true;
        break;
        
    case TYPE_5M_APPEAR:
    case TYPE_20M_APPEAR:
        m_bEnable = false;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_bEnable)
        initCollision();
};


CInvisibleWallGimmick::~CInvisibleWallGimmick(void)
{
    termCollision();
};


void CInvisibleWallGimmick::Draw(void) const
{
#ifdef _DEBUG
    if (CGimmickDebug::SHOW_MODEL)
        CGimmick::Draw();
#endif /* _DEBUG */
};


void CInvisibleWallGimmick::OnReceiveEvent(const char* pszSender,
                                                       GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        switch (m_type)
        {
        case TYPE_5M_DISAPPEAR:
        case TYPE_20M_DISAPPEAR:
            m_bEnable = false;
            break;

        case TYPE_5M_APPEAR:
        case TYPE_20M_APPEAR:
            m_bEnable = true;
            break;

        default:
            ASSERT(false);
            break;
        };

        if (m_bEnable)
            initCollision();
        else
            termCollision();
    };
};


void CInvisibleWallGimmick::initCollision(void)
{
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        ASSERT(pClump);

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);

        if (m_hAtari)
        {
            switch (m_type)
            {
            case TYPE_5M_APPEAR:
            case TYPE_5M_DISAPPEAR:
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
                break;

            case TYPE_20M_APPEAR:
            case TYPE_20M_DISAPPEAR:
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 20.0f);
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CInvisibleWallGimmick::termCollision(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


//
// *********************************************************************************
//


/*static*/ void CReplaceGimmick::ReplacePlayer(int32 nPlayerNo, const RwV3d* pvPosition, bool bBlink)
{
    char szSearchGimmickName[GAMEOBJECTTYPES::NAME_MAX];
    szSearchGimmickName[0] = '\0';

    const char* pszBaseName = CGimmickInfo::GetBaseName(GIMMICKID::ID_S_RESET);
    ASSERT(pszBaseName);

    std::sprintf(szSearchGimmickName, "%s_A", pszBaseName);
    ReplacePlayer(szSearchGimmickName, nPlayerNo, pvPosition, bBlink);
};


/*static*/ void CReplaceGimmick::SetPlayerStartPosition(int32 nPlayerNo, const RwV3d* pvPosition, bool bBlink)
{
    char szSearchGimmickName[GAMEOBJECTTYPES::NAME_MAX];
    szSearchGimmickName[0] = '\0';

    const char* pszBaseName = CGimmickInfo::GetBaseName(GIMMICKID::ID_S_RESET);
    ASSERT(pszBaseName);

    std::sprintf(szSearchGimmickName, "%s_B", pszBaseName);
    ReplacePlayer(szSearchGimmickName, nPlayerNo, pvPosition, bBlink);
};


/*static*/ void CReplaceGimmick::ReplacePlayer(const char* pszSearchGimmickName,
                                               int32 nPlayerNo,
                                               const RwV3d* pvPosition,
                                               bool bBlink)
{
    float fMinDistance = TYPEDEF::FLOAT_MAX;
    CReplaceGimmick* pOptimalReplaceGimmick = nullptr;
    
    CGameObject* pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
    while (pGameObj)
    {
        ASSERT(pGameObj->GetType() == GAMEOBJECTTYPE::GIMMICK);
        CGimmick* pGimmick = static_cast<CGimmick*>(pGameObj);

        if (!std::strncmp(pGimmick->GetName(), pszSearchGimmickName, std::strlen(pszSearchGimmickName)))
        {
            CReplaceGimmick* pReplaceGimmick = static_cast<CReplaceGimmick*>(pGimmick);

            if (!pReplaceGimmick->m_bStartPosition)
            {
                if (pReplaceGimmick->IsEnableReplaceGimmick())
                {
                    RwV3d vDistance = Math::VECTOR3_ZERO;
                    Math::Vec3_Sub(&vDistance, pvPosition, &pReplaceGimmick->m_vReplacePosition);

                    float fDistance = Math::Vec3_Length(&vDistance);
                    fDistance = std::fabs(fDistance);

                    if (fDistance < fMinDistance)
                    {
                        fMinDistance = fDistance;
                        pOptimalReplaceGimmick = pReplaceGimmick;
                    };
                };
            }
            else
            {
                pOptimalReplaceGimmick = pReplaceGimmick;
                break;
            };
        };

        pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pGameObj);
    };

    if (pOptimalReplaceGimmick)
    {
        pOptimalReplaceGimmick->OnReplacePlayer(nPlayerNo, bBlink);
    }
    else
    {
        IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);
        pGamePlayer->Relocation(pvPosition, pGamePlayer->GetRotY(), bBlink);
		pGamePlayer->Release();
    };
};


CReplaceGimmick::CReplaceGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vReplacePosition(Math::VECTOR3_ZERO)
, m_vReplaceRotation(Math::VECTOR3_ZERO)
, m_aAreaRectVertex()
, m_fRotationY(0.0f)
, m_bEnable(false)
, m_bStartPosition(false)
{
    std::memset(m_aAreaRectVertex, 0x00, sizeof(m_aAreaRectVertex));

    GIMMICKPARAM::GIMMICK_BASIC* pReplaceParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pReplaceParam);

    m_vReplacePosition = pReplaceParam->m_vPosition;
    
    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pReplaceParam->m_quat);
    
    m_vReplaceRotation = matrix.at;
    m_vReplaceRotation.y = 0.0f;    
    Math::Vec3_Normalize(&m_vReplaceRotation, &m_vReplaceRotation);

    m_fRotationY = CGimmickUtils::QuaternionToRotationY(&pReplaceParam->m_quat);

    setAreaVertex();

    m_bStartPosition = (pReplaceParam->m_subid == 1) ? true : false;
};


CReplaceGimmick::~CReplaceGimmick(void)
{
    ;
};


void CReplaceGimmick::PostMove(void)
{
	;
};


void CReplaceGimmick::OnReceiveEvent(const char* pszSender,
                                                 GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        m_bEnable = !m_bEnable;
};


void CReplaceGimmick::OnReplacePlayer(int32 nPlayerNo, bool bBlink)
{
    const float afRotation[]
    {
        45.0f,
        315.0f,
        135.0f,
        225.0f,
    };

    static_assert(COUNT_OF(afRotation) == GAMETYPES::PLAYERS_MAX, "update me");

    RwV3d vDir = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vDir, &m_vReplaceRotation);
    Math::Vec3_Scale(&vDir, &vDir, 1.5f);

    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    Math::Matrix_RotateY(&matrix, MATH_DEG2RAD(afRotation[nPlayerNo]));

    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &vDir, &matrix);
    Math::Vec3_Add(&vPosition, &vPosition, &m_vReplacePosition);

    float fHeight = CWorldMap::GetMapHeight(&vPosition);

    MAPTYPES::ATTRIBUTE attribute = CWorldMap::GetCollisionResultAttribute();
    if (attribute == MAPTYPES::ATTRIBUTE_DEATH)
        vPosition = m_vReplacePosition;
    else
        vPosition.y = fHeight;

	IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);
	pGamePlayer->Relocation(&vPosition, m_fRotationY, bBlink);
	pGamePlayer->Release();
};


bool CReplaceGimmick::IsEnableReplaceGimmick(void) const
{
    return (m_bEnable && isAreaRectVertexInScreen());
};


void CReplaceGimmick::setAreaVertex(void)
{
    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    Math::Matrix_RotateY(&matrix, m_fRotationY);
    
    matrix.pos = m_vReplacePosition;
    
    for (int32 i = 0; i < COUNT_OF(m_aAreaRectVertex); ++i)
    {
        RwV3d point = Math::VECTOR3_ZERO;
        
        float x = ((i & ~1) != 0) ? -1.0f : 1.0f;
        float z = ((i & 1) != 0) ? -1.0f : 1.0f;
        
        point.x = x + x;
        point.y = 0.0f;
        point.z = z + z;
        
        RwV3dTransformPoint(&m_aAreaRectVertex[i], &point, &matrix);
    };
};


bool CReplaceGimmick::isAreaRectVertexInScreen(void) const
{
    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);    
    CGameProperty::GetCameraViewMatrix(&matrix);

    for (int32 i = 0; i < COUNT_OF(m_aAreaRectVertex); ++i)
    {
        RwV3d vScPos = Math::VECTOR3_ZERO;

        RwV3dTransformPoint(&vScPos, &m_aAreaRectVertex[i], &matrix);

        if (vScPos.z > 0.0f)
        {
            vScPos.x *= (1.0f / vScPos.z);
            vScPos.y *= (1.0f / vScPos.z);

            if (vScPos.z >= 1.0f    &&
                vScPos.z <= 200.0f  &&
                vScPos.x >= 0.0f    &&
                vScPos.x <= 1.0f    &&
                vScPos.y >= 0.0f    &&
                vScPos.y <= 1.0f)
            {
                return true;
            };
        };
    };

    return false;
};


//
// *********************************************************************************
//


/*static*/ const CHelpGimmick::INITPARAM CHelpGimmick::m_aInitParam[] =
{
    { SEGROUPID::VALUE(0x2),    false },
    { SEGROUPID::VALUE(0x3),    false },
    { SEGROUPID::VALUE(0x4),    false },
    { SEGROUPID::VALUE(0x5),    false },
    { SEGROUPID::VALUE(0x6),    false },
    { SEGROUPID::VALUE(0x7),    false },
    { SEGROUPID::VALUE(0x8),    false },
    { SEGROUPID::VALUE(0xA),    false },
    { SEGROUPID::VALUE(0xC),    false },
    { SEGROUPID::VALUE(0x14),   false },
    { SEGROUPID::VALUE(0x15),   false },
    { SEGROUPID::VALUE(0x16),   false },
    { SEGROUPID::VALUE(0x17),   false },
    { SEGROUPID::VALUE(0x18),   false },
    { SEGROUPID::VALUE(0x2E),   false },
    { SEGROUPID::VALUE(0x37),   false },
    { SEGROUPID::VALUE(0x38),   false },
    { SEGROUPID::VALUE(0x3A),   false },
    { SEGROUPID::VALUE(0x3B),   false },
    { SEGROUPID::VALUE(0x3C),   false },
    { SEGROUPID::VALUE(0x3D),   false },
    { SEGROUPID::VALUE(0x3E),   false },
    { SEGROUPID::VALUE(0x41),   false },
    { SEGROUPID::VALUE(0x46),   false },
    { SEGROUPID::VALUE(0x47),   false },
    { SEGROUPID::VALUE(0x48),   false },
    { SEGROUPID::VALUE(0x49),   false },
    { SEGROUPID::VALUE(0x4B),   false },
    { SEGROUPID::VALUE(0x4C),   false },
    { SEGROUPID::VALUE(0x4E),   false },
    { SEGROUPID::VALUE(0x4F),   false },
    { SEGROUPID::VALUE(0x50),   false },
    { SEGROUPID::VALUE(0x52),   false },
    { SEGROUPID::VALUE(0x53),   false },
    { SEGROUPID::VALUE(0x55),   false },
    { SEGROUPID::VALUE(0x56),   false },
    { SEGROUPID::VALUE(0x57),   false },
    { SEGROUPID::VALUE(0x58),   false },
    { SEGROUPID::VALUE(0x5B),   false },
    { SEGROUPID::VALUE(0x5C),   false },
    { SEGROUPID::VALUE(0x5D),   false },
    { SEGROUPID::VALUE(0x5E),   false },
    { SEGROUPID::VALUE(0x61),   false },
    { SEGROUPID::VALUE(0x64),   false },
    { SEGROUPID::VALUE(0x66),   false },
    { SEGROUPID::VALUE(0x67),   false },
    { SEGROUPID::VALUE(0x6C),   false },
    { SEGROUPID::VALUE(0x72),   false },
    { SEGROUPID::VALUE(0x73),   false },
    { SEGROUPID::VALUE(0x75),   false },
    { SEGROUPID::VALUE(0x76),   false },
    { SEGROUPID::VALUE(0x79),   false },
    { SEGROUPID::VALUE(0x7A),   false },
    { SEGROUPID::VALUE(0x7B),   false },
    { SEGROUPID::VALUE(0x7C),   false },
    { SEGROUPID::VALUE(0x7D),   false },
    { SEGROUPID::VALUE(0x7E),   false },
    { SEGROUPID::VALUE(0x80),   false },
    { SEGROUPID::VALUE(0x81),   false },
    { SEGROUPID::VALUE(0x82),   false },
    { SEGROUPID::VALUE(0x84),   false },
    { SEGROUPID::VALUE(0x85),   false },
    { SEGROUPID::VALUE(0x87),   false },
    { SEGROUPID::VALUE(0x89),   false },
    { SEGROUPID::VALUE(0x8F),   false },
    { SEGROUPID::VALUE(0x90),   false },
    { SEGROUPID::VALUE(0x93),   false },
    { SEGROUPID::VALUE(0x95),   false },
    { SEGROUPID::VALUE(0x96),   false },
    { SEGROUPID::VALUE(0x99),   false },
    { SEGROUPID::VALUE(0xA6),   false },
    { SEGROUPID::VALUE(0xA7),   false },
    { SEGROUPID::VALUE(0xAB),   false },
    { SEGROUPID::VALUE(0xAF),   false },
    { SEGROUPID::VALUE(0xB1),   false },
    { SEGROUPID::VALUE(0xB9),   false },
    { SEGROUPID::VALUE(0xBB),   false },
    { SEGROUPID::VALUE(0xBC),   false },
    { SEGROUPID::VALUE(0xBD),   false },
    { SEGROUPID::VALUE(0xBE),   false },
    { SEGROUPID::VALUE(0xBF),   false },
    { SEGROUPID::VALUE(0xC0),   false },
    { SEGROUPID::VALUE(0xC1),   false },
    { SEGROUPID::VALUE(0xC2),   false },
    { SEGROUPID::VALUE(0xC3),   false },
    { SEGROUPID::VALUE(0xC5),   false },
    { SEGROUPID::VALUE(0xC6),   false },
    { SEGROUPID::VALUE(0xC7),   false },
    { SEGROUPID::VALUE(0xC8),   false },
    { SEGROUPID::VALUE(0xC9),   false },
    { SEGROUPID::VALUE(0xCA),   false },
    { SEGROUPID::VALUE(0xCB),   false },
    { SEGROUPID::VALUE(0xCC),   false },
    { SEGROUPID::VALUE(0xCD),   false },
    { SEGROUPID::VALUE(0xCE),   false },
    { SEGROUPID::VALUE(0xD0),   false },
    { SEGROUPID::VALUE(0xD1),   false },
    { SEGROUPID::VALUE(0xD2),   false },
    { SEGROUPID::VALUE(0xD3),   false },
    { SEGROUPID::VALUE(0xD4),   false },
    { SEGROUPID::VALUE(0xD5),   false },
    { SEGROUPID::VALUE(0xD6),   false },
    { SEGROUPID::VALUE(0xD7),   false },
    { SEGROUPID::VALUE(0xD8),   false },
    { SEGROUPID::VALUE(0xDA),   false },
    { SEGROUPID::VALUE(0xDB),   false },
    { SEGROUPID::VALUE(0xDC),   false },
    { SEGROUPID::VALUE(0xDD),   false },
    { SEGROUPID::VALUE(0xDE),   false },
    { SEGROUPID::VALUE(0xDF),   false },
    { SEGROUPID::VALUE(0xE0),   false },
    { SEGROUPID::VALUE(0xE1),   false },
    { SEGROUPID::VALUE(0xE2),   false },
    { SEGROUPID::VALUE(0xE3),   false },
    { SEGROUPID::VALUE(0xE4),   false },
    { SEGROUPID::VALUE(0xE5),   false },
    { SEGROUPID::VALUE(0xE6),   false },
    { SEGROUPID::VALUE(0xE7),   false },
};


CHelpGimmick::CHelpGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_state(STATE_IDLE)
, m_vPosition(Math::VECTOR3_ZERO)
, m_nSeGroupID(0)
, m_fDelayTime(0.0f)
, m_fTime(0.0f)
, m_bExclusive(false)
{
    GIMMICKPARAM::GIMMICK_HELP* pHelpParam = static_cast<GIMMICKPARAM::GIMMICK_HELP*>(pParam);
    ASSERT(pHelpParam);

    ASSERT(pHelpParam->m_nHelpNo >= 0);
    ASSERT(pHelpParam->m_nHelpNo < COUNT_OF(m_aInitParam));

    m_nSeGroupID = m_aInitParam[pHelpParam->m_nHelpNo].m_nSeGroupID;
    m_bExclusive = m_aInitParam[pHelpParam->m_nHelpNo].m_bExclusive;

    if (SEGROUPID::IsHelpID(SEGROUPID::VALUE(m_nSeGroupID)))
        m_fDelayTime = 30.0f;
    else
        m_fDelayTime = 0.0f;

    m_vPosition = pHelpParam->m_vPosition;
    
    if (!isSatisfyCrystalMessageCondition())
        forceEnd();
};


CHelpGimmick::~CHelpGimmick(void)
{
    ;
};


void CHelpGimmick::Run(void)
{
    switch (m_state)
    {
    case STATE_RUN:
        if (m_fTime < m_fDelayTime)
        {
            m_fTime += CGameProperty::GetElapsedTime();
        }
        else
        {
            showMessage();

            m_fTime = 0.0f;
            if (!m_bExclusive)
                forceEnd();
        };
        break;

    case STATE_STOP:
        Release();
        break;

    default:
        break;
    };
};


void CHelpGimmick::OnReceiveEvent(const char* pszSender,
                                              GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        switch (m_state)
        {
        case STATE_IDLE:
            start();
            break;

        case STATE_RUN:
            forceEnd();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CHelpGimmick::start(void)
{
    m_state = STATE_RUN;
    m_fTime = 0.0f;
};


void CHelpGimmick::forceEnd(void)
{
    m_state = STATE_STOP;
};


bool CHelpGimmick::isSatisfyCrystalMessageCondition(void)
{
    bool bResult = true;
    
    switch (m_nSeGroupID)
    {
    case SEGROUPID::VALUE(227):
        if (CGameData::Record().Item().IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_RED, AREAID::ID_AREA01))
            bResult = false;
        break;

    case SEGROUPID::VALUE(228):
        if (CGameData::Record().Item().IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_GREEN, AREAID::ID_AREA01))
            bResult = false;
        break;
        
    case SEGROUPID::VALUE(229):
        if (CGameData::Record().Item().IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_WHITE, AREAID::ID_AREA01))
            bResult = false;
        break;

    case SEGROUPID::VALUE(230):
        if (CGameData::Record().Item().IsAreaCrystalTaken(GAMETYPES::CRYSTALTYPE_ORANGE, AREAID::ID_AREA01))
            bResult = false;
        break;

    case SEGROUPID::VALUE(231):
        if (CGameData::Record().Item().IsItemTakenYet(ITEMID::ID_INVINCIBLE))
            bResult = false;
        break;

    default:
        break;
    };

    return bResult;
};


void CHelpGimmick::showMessage(void)
{
    if ((m_nSeGroupID < SEGROUPID::VALUE(227)) ||
        (m_nSeGroupID > SEGROUPID::VALUE(231)))
        CMessageManager::Request(SEGROUPID::VALUE(m_nSeGroupID));
    else
        CMessageManager::OnlyTextRequest(SEGROUPID::VALUE(m_nSeGroupID));
};


//
// *********************************************************************************
//


CTutorialGimmick::CTutorialGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_state(STATE_IDLE)
, m_nTutoNo(0)
, m_bDonLaserTutoEnable(false)
{
    GIMMICKPARAM::GIMMICK_TUTORIAL* pTutorialParam = static_cast<GIMMICKPARAM::GIMMICK_TUTORIAL*>(pParam);
    ASSERT(pTutorialParam);

    m_nTutoNo = pTutorialParam->m_nTutoNo;
    if (m_nTutoNo == 3)
    {
        if (CGameData::Record().Secret().IsDonLazerEnabled())
            m_bDonLaserTutoEnable = true;
    };
};


CTutorialGimmick::~CTutorialGimmick(void)
{
    ;
};


void CTutorialGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_IDLE:
        break;

    case STATE_OPENED:
        m_state = STATE_CLOSED;
        break;

    case STATE_CLOSED:
        m_state = STATE_EOL;
        break;

    case STATE_EOL:        
        break;

    default:
        break;
    };
};


void CTutorialGimmick::OnReceiveEvent(const char* pszSender,
                                                  GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        switch (m_state)
        {
        case STATE_IDLE:
            if ((m_nTutoNo != 3) || !m_bDonLaserTutoEnable)
            {
                CGameEvent::SetTutorialOpened(m_nTutoNo);
                m_state = STATE_OPENED;
            };
            break;

        default:
            break;
        };
    };
};


//
// *********************************************************************************
//


CEnemyKillGimmick::CEnemyKillGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_szTargetGimmick()
{
    m_szTargetGimmick[0] = '\0';

    GIMMICKPARAM::GIMMICK_TERMS* pTermsParam = static_cast<GIMMICKPARAM::GIMMICK_TERMS*>(pParam);
    ASSERT(pTermsParam);

    std::strcpy(m_szTargetGimmick, pTermsParam->m_szTargetGimmick);
};


CEnemyKillGimmick::~CEnemyKillGimmick(void)
{
    ;
};


void CEnemyKillGimmick::OnReceiveEvent(const char* pszSender,
                                                   GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        CGimmickManager::PostEvent(m_szTargetGimmick, GetName(), GIMMICKTYPES::EVENTTYPE_ENEMYKILL);
};
