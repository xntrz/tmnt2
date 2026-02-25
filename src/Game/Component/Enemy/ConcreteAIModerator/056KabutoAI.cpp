#include "056KabutoAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/WorldMap.hpp"


/*static*/ CAIModerator* C056KabutoAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C056KabutoAI(pEnemyChr);
};


C056KabutoAI::C056KabutoAI(CEnemyCharacter* pEnemyChr)
: CBaseBatAI(pEnemyChr)
{
    ;
};


/*virtual*/ C056KabutoAI::~C056KabutoAI(void)
{
    ;
};


/*virtual*/ bool C056KabutoAI::OnActionOfAttack(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);

    if (!pPlayerChr)
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN) ||
        m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;
    
    RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

    RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetBodyPosition(&vecBodyPosEnemy);

    bool bCollision = CWorldMap::CheckCollisionLine(&vecBodyPosEnemy, &vecBodyPosPlayer);
    if (bCollision)
    {
        const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
        ASSERT(pCollisionResult != nullptr);

        MAPTYPES::ATTRIBUTE attribute = pCollisionResult->m_attribute;
        if (!(attribute & MAPTYPES::ATTRIBUTE_ONEWAY))
        {
            RwV3d vecClosestPt = pCollisionResult->m_vClosestPt;

            float fDistToPlayer = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &vecBodyPosPlayer);
            float fDistToMap    = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &vecClosestPt);

            if (fDistToMap < fDistToPlayer)
                return false;
        };
    };

    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, &vecBodyPosPlayer, &vecBodyPosEnemy);

    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, -std::atan2(vec.x, vec.z));

    RwV3dTransformVector(&vec, &vec, &matRotY);

    float fPitch = std::atan2(vec.y, vec.z);

    int32 attackType = AIOT::Null;

    if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        attackType = AIOT::FireB;
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_REPEATABLE))
        attackType = AIOT::FireRepeatB;

    if (attackType != AIOT::Null)
    {
        RwV3d vecAt = { -fPitch, 0.0f, 0.0f };
        AIOT::SetFireOrder(ThinkOrder(), attackType, iNo, &vecAt);

        SetState(STATE_ATTACK);
        SetCancelTimer(0.0f);

        return true;
    };

    return false;
};