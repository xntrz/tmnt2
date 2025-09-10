#include "HomeCharacter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Player/Manipulator.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


class CHomeCharacterManipulator : public CManipulator
{
public:
    CHomeCharacterManipulator(CPlayerCharacter* pPlayerChr);
    virtual ~CHomeCharacterManipulator(void);
    virtual void Run(void) override;
};


static CHomeCharacterManipulator* s_apHomeCharacterManipulator[PLAYERID::ID_MAX] = { nullptr };


static inline float RandDirection(void)
{
    return (Math::RandFloat() - 0.5f);
};


CHomeCharacterManipulator::CHomeCharacterManipulator(CPlayerCharacter* pPlayerChr)
: CManipulator("HomeChr_MP", pPlayerChr, -1)
{
    RwV3d vPosition = { 0.0f, 0.0f, 10.0f };
    m_pPlayerChr->SetPosition(&vPosition);
    m_pPlayerChr->SetDirection(Math::RandFloat() * MATH_PI2);
};


CHomeCharacterManipulator::~CHomeCharacterManipulator(void)
{
    ;
};


void CHomeCharacterManipulator::Run(void)
{
    float fDirection = m_pPlayerChr->GetDirection();

    switch (m_pPlayerChr->GetStatus())
    {
    case PLAYERTYPES::STATUS_IDLE:
        {
            m_pPlayerChr->SetDirection(fDirection + (RandDirection() * MATH_PI));
            m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_RUN);
        }
        break;

    case PLAYERTYPES::STATUS_RUN:
        {
            RwV3d vPos = Math::VECTOR3_ZERO;
            m_pPlayerChr->GetFootPosition(&vPos);

            const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerChr->GetCollisionWall();
            ASSERT(pWallInfo);

            if (pWallInfo->m_bHit)
            {
                RwV3d vecRot = Math::VECTOR3_ZERO;
                Math::GetRotateFromVector(&vecRot, &pWallInfo->m_vNormal);

                float fYaw = vecRot.y;

                fYaw = (fYaw * 2.0f) - (fDirection + MATH_PI);
                fYaw = Math::RadianNormalize(fYaw);

                m_pPlayerChr->SetDirection(fYaw);
            }
            else
            {
                m_pPlayerChr->SetDirection(fDirection + (RandDirection() * 0.2f));
            };

            float fRand = 0.0f;
            if (vPos.y >= -1.0f)
                fRand = (CGameProperty::GetElapsedTime() / 3.0f);
            else
                fRand = (3.0f * CGameProperty::GetElapsedTime());

            if (Math::RandFloat() < fRand)
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP);
        }
        break;

    case PLAYERTYPES::STATUS_JUMP:
        {
            if (m_pPlayerChr->IsEnableJumpWall())
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_WALL);
            }
            else if (m_pPlayerChr->IsEnableJump2nd())
            {
                if ((CGameProperty::GetElapsedTime() * 0.5f) > Math::RandFloat())
                    m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_JUMP_2ND);
            };
        }
        break;

    case PLAYERTYPES::STATUS_AERIAL:
        {
            RwV3d vVel = Math::VECTOR3_ZERO;
            m_pPlayerChr->GetVelocity(&vVel);

            if ((vVel.x == 0.0f) && (vVel.z == 0.0f))
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);

            m_pPlayerChr->SetDirection(fDirection + (RandDirection() * 0.2f));
        }
        break;

    case PLAYERTYPES::STATUS_TOUCHDOWN:
        {
            m_pPlayerChr->SetDirection(fDirection + (RandDirection() * 0.2f));
        }
        break;

    default:
        break;
    };
};


/*static*/ void CHomeCharacter::Initialize(void)
{
    for (int32 i = 0; i < PLAYERID::ID_MAX; ++i)
    {
        if (!CheckAppear(PLAYERID::VALUE(i)))
            continue;

        CPlayerCharacter* pPlayerCharacter = CPlayerCharacter::New(PLAYERID::VALUE(i), GAMETYPES::COSTUME_NONE);
        ASSERT(pPlayerCharacter);

        ASSERT(i >= 0);
        ASSERT(i < COUNT_OF(s_apHomeCharacterManipulator));
        s_apHomeCharacterManipulator[i] = new CHomeCharacterManipulator(pPlayerCharacter);
    };
};


/*static*/ void CHomeCharacter::Terminate(void)
{
    ;
};


/*static*/ bool CHomeCharacter::CheckAppear(PLAYERID::VALUE idPlayer)
{
    return PLAYERID::IsEnabled(idPlayer);
};