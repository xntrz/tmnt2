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
    if (m_input.m_move)
        m_pPlayerChr->SetDirection(m_input.m_fDirection);

    switch (m_input.m_attack)
    {
	case ATTACK_OFF:
		break;

    case ATTACK_A:
        m_pPlayerChr->RequestAttackA();
        break;
        
    case ATTACK_B:
        m_pPlayerChr->RequestAttackB();
        break;

    case ATTACK_C:
        m_pPlayerChr->RequestAttackC();
        break;

    default:
        ASSERT(false);
        break;
    };
};