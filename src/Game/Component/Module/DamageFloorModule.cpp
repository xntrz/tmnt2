#include "DamageFloorModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Misc/Gamepad.hpp"


static const int32 DAMAGEAMOUNT = 5;


CDamageFloorModule::CDamageFloorModule(CCharacter* pCharacter)
: IModule(MODULETYPE::DAMAGE_FLOOR)
, m_pCharacter(pCharacter)
, m_fTimer(CGameProperty::GetTotalElapsedTime())
{
    ASSERT(m_pCharacter);
};


CDamageFloorModule::~CDamageFloorModule(void)
{
    ;
};


void CDamageFloorModule::Run(void)
{
    const CCharacter::COLLISIONGROUNDINFO* pGroundInfo = m_pCharacter->GetCollisionGround();
    ASSERT(pGroundInfo);

    if (pGroundInfo->m_bHit)
    {
		uint32 uAttributeMask = MAPTYPES::ATTRIBUTE_DAMAGE | MAPTYPES::ATTRIBUTE_MAGMA | MAPTYPES::ATTRIBUTE_POISON;

		if (FLAG_TEST_ANY(pGroundInfo->m_attribute, uAttributeMask))
		{
			if (CGameProperty::GetTotalElapsedTime() - m_fTimer > 0.5f)
			{
				m_fTimer = CGameProperty::GetTotalElapsedTime();

				CGameObjectManager::SendMessage(
					m_pCharacter,
					CHARACTERTYPES::MESSAGEID_RECVDMG,
					(void*)DAMAGEAMOUNT
				);

				if (m_pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
				{
					CPlayerCharacter* pPlayerChr = (CPlayerCharacter*)m_pCharacter;

					IGamepad::StartVibration(
						pPlayerChr->GetPadID(),
						IGamepad::VIBRATIONTYPE_LOW,
						0.2f
					);
				};
			};
		};
    }
    else
    {
        m_fTimer = CGameProperty::GetTotalElapsedTime();
    };
};


void CDamageFloorModule::Draw(void)
{
    ;
};