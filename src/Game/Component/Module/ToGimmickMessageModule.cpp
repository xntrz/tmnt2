#include "ToGimmickMessageModule.hpp"

#include "Game/Component/Gimmick/ConcreteGimmick/ConsoleGimmick.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CToGimmickMessageModule* CToGimmickMessageModule::New(CPlayerCharacter* pPlayerCharacter)
{
    ASSERT(pPlayerCharacter);

    CToGimmickMessageModule* pRet = nullptr;

    switch (pPlayerCharacter->GetID())
    {
    case PLAYERID::ID_RAP:
    case PLAYERID::ID_CAS:
        pRet = new CPowerfullCharacterToGimmickMessageModule(pPlayerCharacter);
        break;

    case PLAYERID::ID_DON:
        pRet = new CConsoleCharacterToGimmickMessageModule(pPlayerCharacter);
        break;

    default:
        pRet = new CDefaultCharacterToGimmickMessageModule(pPlayerCharacter);
        break;
    };

    ASSERT(pRet);

    return pRet;
};


CToGimmickMessageModule::CToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter)
: IModule(MODULETYPE::TO_GIMMICK_MSG)
, m_pPlayerCharacter(pPlayerCharacter)
{
    m_szPrevGimmickObjName[0] ='\0';

    ASSERT(m_pPlayerCharacter);
};


CToGimmickMessageModule::~CToGimmickMessageModule(void)
{
    ;
};


void CToGimmickMessageModule::Run(void)
{
    const CPlayerCharacter::COLLISIONGROUNDINFO* pGroundInfo = m_pPlayerCharacter->GetCollisionGround();
    ASSERT(pGroundInfo);

    if (pGroundInfo->m_bHit && pGroundInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK)
    {
        switch (pGroundInfo->m_gimmickinfo.m_type)
        {
        case MAPTYPES::GIMMICKTYPE_SEESAW:
        case MAPTYPES::GIMMICKTYPE_PILLAR:
        case MAPTYPES::GIMMICKTYPE_THROWOBJ:
            SendMessageCommon(pGroundInfo->m_gimmickinfo.m_szGimmickObjName);
            break;

        case MAPTYPES::GIMMICKTYPE_SWITCH_PILLAR:
            if (std::strcmp(m_szPrevGimmickObjName, pGroundInfo->m_gimmickinfo.m_szGimmickObjName))
                SendMessageCommon(pGroundInfo->m_gimmickinfo.m_szGimmickObjName);
            break;
        };
    };

    if (pGroundInfo->m_bHit)
    {
        if (!FLAG_TEST(pGroundInfo->m_attribute, MAPTYPES::ATTRIBUTE_DEATH))
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            
            m_pPlayerCharacter->GetPosition(&vPosition);
            m_pPlayerCharacter->SetReplacePosition(&vPosition);
        };

        switch (pGroundInfo->m_hittype)
        {
        case MAPTYPES::HITTYPE_GIMMICK:
            SetPrevGimmickObjName(pGroundInfo->m_gimmickinfo.m_szGimmickObjName);
            break;

        case MAPTYPES::HITTYPE_MAP:
            SetPrevGimmickObjName();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CToGimmickMessageModule::Draw(void)
{
    ;
};


bool CToGimmickMessageModule::DirectionCheck(void)
{
    const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerCharacter->GetCollisionWall();
    ASSERT(pWallInfo);
    
    CGameObject* pGameObject = CGameObjectManager::GetObject(pWallInfo->m_gimmickinfo.m_szGimmickObjName);
    if (!pGameObject)
        return false;

    if (pGameObject->GetType() != GAMEOBJECTTYPE::GIMMICK)
        return false;

    RwV3d vCharacterDirection = Math::VECTOR3_ZERO;
    RwV3d vToGimmickDirection = Math::VECTOR3_ZERO;
    RwMatrix matRotY;

    RwMatrixSetIdentityMacro(&matRotY);
    
    Math::Vec3_Negate(&vToGimmickDirection, &pWallInfo->m_vNormal);
    vToGimmickDirection.y = 0.0f;
    Math::Vec3_Normalize(&vToGimmickDirection, &vToGimmickDirection);

    vCharacterDirection.x = 0.0f;
    vCharacterDirection.y = 0.0f;
    vCharacterDirection.z = 1.0f;
    Math::Matrix_RotateY(&matRotY, m_pPlayerCharacter->GetDirection());
    RwV3dTransformVector(&vCharacterDirection, &vCharacterDirection, &matRotY);

    float fAngle = Math::Vec3_Dot(&vCharacterDirection, &vToGimmickDirection);
    fAngle = Math::ACos(fAngle);

    static const float INTERACTION_RANGE = 0.5215044f;

    if (fAngle >= INTERACTION_RANGE ||
        fAngle <= -INTERACTION_RANGE)
        return false;

    m_pPlayerCharacter->SetDirectionFromVector(&vToGimmickDirection);
    
    return true;
};


void CToGimmickMessageModule::SendMessageCommon(const char* pszGimmickObjName)
{
    CGimmickManager::PostEvent(
        pszGimmickObjName,
        m_pPlayerCharacter->GetName(),
        GIMMICKTYPES::EVENTTYPE_ACTIVATE
    );
};


void CToGimmickMessageModule::SetPrevGimmickObjName(const char* pszGimmickObjName)
{
    if (pszGimmickObjName)
    {
        ASSERT(std::strlen(pszGimmickObjName) < COUNT_OF(m_szPrevGimmickObjName));
        std::strcpy(m_szPrevGimmickObjName, pszGimmickObjName);
    }
    else
    {
        m_szPrevGimmickObjName[0] ='\0';
    };
};


CDefaultCharacterToGimmickMessageModule::CDefaultCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter)
: CToGimmickMessageModule(pPlayerCharacter)
{
    ;
};


CDefaultCharacterToGimmickMessageModule::~CDefaultCharacterToGimmickMessageModule(void)
{
    ;
};


CPowerfullCharacterToGimmickMessageModule::CPowerfullCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter)
: CToGimmickMessageModule(pPlayerCharacter)
{
    ;
};


CPowerfullCharacterToGimmickMessageModule::~CPowerfullCharacterToGimmickMessageModule(void)
{
    ;
};


void CPowerfullCharacterToGimmickMessageModule::Run(void)
{
    const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerCharacter->GetCollisionWall();
    ASSERT(pWallInfo);

    if ((!pWallInfo->m_bHit || pWallInfo->m_hittype != MAPTYPES::HITTYPE_GIMMICK || !m_pPlayerCharacter->IsPlayerFlagSet(PLAYERTYPES::FLAG_PUSH)) &&
        m_pPlayerCharacter->GetStatus() == PLAYERTYPES::STATUS_PUSH)
    {
        m_pPlayerCharacter->ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    if (pWallInfo->m_bHit &&
        pWallInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK &&
        pWallInfo->m_gimmickinfo.m_type == MAPTYPES::GIMMICKTYPE_PUSHOBJ &&
        DirectionCheck() &&
        m_pPlayerCharacter->IsPlayerFlagSet(PLAYERTYPES::FLAG_PUSH))
    {
        m_pPlayerCharacter->ChangeStatus(PLAYERTYPES::STATUS_PUSH);
    };

    CToGimmickMessageModule::Run();
};


CConsoleCharacterToGimmickMessageModule::CConsoleCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter)
: CToGimmickMessageModule(pPlayerCharacter)
{
    ;
};


CConsoleCharacterToGimmickMessageModule::~CConsoleCharacterToGimmickMessageModule(void)
{
    ;
};


void CConsoleCharacterToGimmickMessageModule::Run(void)
{
    const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerCharacter->GetCollisionWall();
    ASSERT(pWallInfo);

    if (pWallInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK &&
        pWallInfo->m_gimmickinfo.m_type == MAPTYPES::GIMMICKTYPE_CONSOLE &&
        DirectionCheck() &&
        DistanceCheck() &&
        m_pPlayerCharacter->IsPlayerFlagSet(PLAYERTYPES::FLAG_CONSOLE))        
    {
        m_pPlayerCharacter->ChangeStatus(PLAYERTYPES::STATUS_CONSOLE);
    };

    CToGimmickMessageModule::Run();
};


bool CConsoleCharacterToGimmickMessageModule::DirectionCheck(void)
{
    return CToGimmickMessageModule::DirectionCheck();
};


bool CConsoleCharacterToGimmickMessageModule::DistanceCheck(void)
{
    const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerCharacter->GetCollisionWall();
    ASSERT(pWallInfo);

    CGameObject* pGameObject = CGameObjectManager::GetObject(pWallInfo->m_gimmickinfo.m_szGimmickObjName);
    if (!pGameObject)
        return false;

    if (pGameObject->GetType() != GAMEOBJECTTYPE::GIMMICK)
        return false;

    CGimmick* pGimmick = (CGimmick*)pGameObject;
    if (pGimmick->GetID() != GIMMICKID::ID_N_CONSOL)
        return false;

    CConsoleGimmick* pConsoleGimmick = (CConsoleGimmick*)pGimmick;

    RwV3d vPosChara = Math::VECTOR3_ZERO;
    RwV3d vPosGimmick = Math::VECTOR3_ZERO;

    m_pPlayerCharacter->GetPosition(&vPosChara);
    pConsoleGimmick->GetPosition(&vPosGimmick);

    RwV3d vDistance = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vDistance, &vPosGimmick, &vPosChara);

    if (Math::Vec3_Length(&vDistance) >= 1.2f)
        return false;

    RwV3d vConsoleDir = Math::VECTOR3_ZERO;
    pConsoleGimmick->GetDirection(&vConsoleDir);

    Math::Vec3_Negate(&vConsoleDir, &vConsoleDir);
    vConsoleDir.y = 0.0f;

    Math::Vec3_Normalize(&vConsoleDir, &vConsoleDir);
    Math::Vec3_Normalize(&vDistance, &vDistance);

    float fAngle = Math::ACos(Math::Vec3_Dot(&vDistance, &vConsoleDir));
    if (fAngle > 0.78539819f && fAngle < -0.78539819f)
        return false;

    return true;
};