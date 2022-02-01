#include "ManipulatorMichelangero.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"


CMichelangeroManipulator::CMichelangeroManipulator(CPlayerCharacter* pPlayerChr, int32 nControllerNo)
: CManipulator("mic_mp", pPlayerChr, nControllerNo)
{
    ;
};


CMichelangeroManipulator::~CMichelangeroManipulator(void)
{
    ;
};


void CMichelangeroManipulator::BranchForStatus(PLAYERTYPES::STATUS status)
{
    switch (status)
    {
    case PLAYERTYPES::STATUS_ATTACK_JUMP:
        RunAttackJump();
        break;

    default:
        CManipulator::BranchForStatus(status);
        break;
    };
};


void CMichelangeroManipulator::RunAttackJump(void)
{
    if (m_input.m_uMove)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);

    switch (m_input.m_uAttack)
    {
	case 0:
		break;

    case 1:
        m_pPlayerChr->RequestAttackA();
        break;
        
    case 2:
        m_pPlayerChr->RequestAttackB();
        break;

    case 3:
        m_pPlayerChr->RequestAttackC();
        break;

    default:
        ASSERT(false);
        break;
    };
};