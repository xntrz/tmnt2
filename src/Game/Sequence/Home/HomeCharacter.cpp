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


CHomeCharacterManipulator::CHomeCharacterManipulator(CPlayerCharacter* pPlayerChr)
: CManipulator("HomeChr_MP", pPlayerChr, -1)
{
    RwV3d vPosition = { 0.0f, 0.0f, 10.0f };
    m_pPlayerChr->SetPosition(&vPosition);
    m_pPlayerChr->SetDirection(Math::RandFloat() * Math::PI2);
};


CHomeCharacterManipulator::~CHomeCharacterManipulator(void)
{
    ;
};


void CHomeCharacterManipulator::Run(void)
{
    switch (m_pPlayerChr->GetStatus())
    {
    case PLAYERTYPES::STATUS_IDLE:
        {
            m_pPlayerChr->SetDirection(
                m_pPlayerChr->GetDirection() + ((Math::RandFloat() - 0.5f) * Math::PI)
            );
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
				m_pPlayerChr->SetDirection(
					Math::ATan2(pWallInfo->m_vNormal.x, pWallInfo->m_vNormal.z) + Math::RandFloat()
                );
            }
            else
            {
                m_pPlayerChr->SetDirection(
                    m_pPlayerChr->GetDirection() + ((Math::RandFloat() - 0.5f) * 0.2f)
                );
            };

            float fRand = 0.0f;
            if (vPos.y >= -1.0f)
                fRand = CGameProperty::GetElapsedTime() / 3.0f;
            else
                fRand = 3.0f * CGameProperty::GetElapsedTime();

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

            if (Math::FEqual(vVel.x, 0.0f) &&
                Math::FEqual(vVel.z, 0.0f))
            {
                m_pPlayerChr->ChangeStatus(PLAYERTYPES::STATUS_AERIAL_MOVE);
            };

            m_pPlayerChr->SetDirection(
                m_pPlayerChr->GetDirection() + ((Math::RandFloat() - 0.5f) * 0.2f)
            );
        }
        break;

    case PLAYERTYPES::STATUS_TOUCHDOWN:
        {
            m_pPlayerChr->SetDirection(
                m_pPlayerChr->GetDirection() + ((Math::RandFloat() - 0.5f) * 0.2f)
            );
        }
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

        CHomeCharacterManipulator* pHomeCharacterManipulator = new CHomeCharacterManipulator(pPlayerCharacter);
        ASSERT(pHomeCharacterManipulator);
        
        ASSERT(i >= 0 && i < COUNT_OF(s_apHomeCharacterManipulator));
        s_apHomeCharacterManipulator[i] = pHomeCharacterManipulator;
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