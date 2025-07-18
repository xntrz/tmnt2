#include "ToGimmickMessageModule.hpp"

#include "Game/Component/Gimmick/ConcreteGimmick/ConsoleGimmick.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CToGimmickMessageModule*
CToGimmickMessageModule::New(CPlayerCharacter* pPlayerCharacter)
{
    switch (pPlayerCharacter->GetID())
    {
    case PLAYERID::ID_RAP:
    case PLAYERID::ID_CAS:
        return new CPowerfullCharacterToGimmickMessageModule(pPlayerCharacter);

    case PLAYERID::ID_DON:
        return new CConsoleCharacterToGimmickMessageModule(pPlayerCharacter);

    default:
        return new CDefaultCharacterToGimmickMessageModule(pPlayerCharacter);
    };
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

    if (pGroundInfo->m_bHit && (pGroundInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK))
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

        default:
            break;
        };
    };

    if (pGroundInfo->m_bHit)
    {
        if (!(pGroundInfo->m_attribute & MAPTYPES::ATTRIBUTE_DEATH))
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

    RwV3d vToGimmickDirection = Math::VECTOR3_ZERO;    
    Math::Vec3_Negate(&vToGimmickDirection, &pWallInfo->m_vNormal);
    vToGimmickDirection.y = 0.0f;
    Math::Vec3_Normalize(&vToGimmickDirection, &vToGimmickDirection);

    RwMatrix matRotY;
    RwMatrixSetIdentityMacro(&matRotY);
    Math::Matrix_RotateY(&matRotY, m_pPlayerCharacter->GetDirection());

    RwV3d vCharacterDirection = Math::VECTOR3_AXIS_Z;
    RwV3dTransformVector(&vCharacterDirection, &vCharacterDirection, &matRotY);

    float fAngle = Math::Vec3_Dot(&vCharacterDirection, &vToGimmickDirection);
    fAngle = Math::ACos(fAngle);
    
    if ((fAngle >=  MATH_DEG2RAD(29.880f)) ||
        (fAngle <= -MATH_DEG2RAD(29.880f)))
        return false;

    m_pPlayerCharacter->SetDirectionFromVector(&vToGimmickDirection);
    
    return true;
};


void CToGimmickMessageModule::SendMessageCommon(const char* pszGimmickObjName)
{
    CGimmickManager::PostEvent(pszGimmickObjName,
                               m_pPlayerCharacter->GetName(),
                               GIMMICKTYPES::EVENTTYPE_ACTIVATE);
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


//
// *********************************************************************************
//


CDefaultCharacterToGimmickMessageModule::CDefaultCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter)
: CToGimmickMessageModule(pPlayerCharacter)
{
    ;
};


CDefaultCharacterToGimmickMessageModule::~CDefaultCharacterToGimmickMessageModule(void)
{
    ;
};


//
// *********************************************************************************
//


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

    if (!pWallInfo->m_bHit || (pWallInfo->m_hittype != MAPTYPES::HITTYPE_GIMMICK) || !m_pPlayerCharacter->TestPlayerFlag(PLAYERTYPES::FLAG_PUSH))
    {
        if (m_pPlayerCharacter->GetStatus() == PLAYERTYPES::STATUS_PUSH)
            m_pPlayerCharacter->ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    if ((pWallInfo->m_bHit) &&
        (pWallInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK) &&
        (pWallInfo->m_gimmickinfo.m_type == MAPTYPES::GIMMICKTYPE_PUSHOBJ))
    {
        if (DirectionCheck() && m_pPlayerCharacter->TestPlayerFlag(PLAYERTYPES::FLAG_PUSH))
            m_pPlayerCharacter->ChangeStatus(PLAYERTYPES::STATUS_PUSH);
    };

    CToGimmickMessageModule::Run();
};


//
// *********************************************************************************
//


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

    if ((pWallInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK) &&
        (pWallInfo->m_gimmickinfo.m_type == MAPTYPES::GIMMICKTYPE_CONSOLE) &&
        DirectionCheck() &&
        DistanceCheck())        
    {
        if (m_pPlayerCharacter->TestPlayerFlag(PLAYERTYPES::FLAG_CONSOLE))
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

    CGimmick* pGimmick = static_cast<CGimmick*>(pGameObject);
    if (pGimmick->GetID() != GIMMICKID::ID_N_CONSOL)
        return false;

    CConsoleGimmick* pConsoleGimmick = static_cast<CConsoleGimmick*>(pGimmick);

    RwV3d vPosChara = Math::VECTOR3_ZERO;
    m_pPlayerCharacter->GetPosition(&vPosChara);

    RwV3d vPosGimmick = Math::VECTOR3_ZERO;
    pConsoleGimmick->GetPosition(&vPosGimmick);

    RwV3d vDirToGimmick = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vDirToGimmick, &vPosGimmick, &vPosChara);

    float fDistToGimmick = Math::Vec3_Length(&vDirToGimmick);
    if (fDistToGimmick >= 1.2f)
        return false;

    RwV3d vConsoleDir = Math::VECTOR3_ZERO;
    pConsoleGimmick->GetDirection(&vConsoleDir);

    Math::Vec3_Negate(&vConsoleDir, &vConsoleDir);
    vConsoleDir.y = 0.0f;
    Math::Vec3_Normalize(&vConsoleDir, &vConsoleDir);
    Math::Vec3_Normalize(&vDirToGimmick, &vDirToGimmick);

    float fAngle = Math::ACos(Math::Vec3_Dot(&vDirToGimmick, &vConsoleDir));
    if ((fAngle >=  MATH_DEG2RAD(45.0f)) &&
        (fAngle <= -MATH_DEG2RAD(45.0f)))
        return false;

    return true;
};