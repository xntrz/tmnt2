#include "EnemySelTestSequence.hpp"

#ifdef _DEBUG

#include "Game/ProcessList.hpp"


/*static*/ ENEMYID::VALUE CEnemySelTestSequence::m_aeID[] =
{
    ENEMYID::ID_DUMMY,
    ENEMYID::ID_PURPLE_DRAGON_GANG,
    ENEMYID::ID_FOOT_NINJA_SWORD,
    ENEMYID::ID_FOOT_NINJA_STAFF,
    ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD,
    ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF,
    ENEMYID::ID_UTROMS_SECURITY_PATROL,
    ENEMYID::ID_FEDERATION_SOLDIER,
    ENEMYID::ID_MOBSTER,
    ENEMYID::ID_FOOT_NINJA_ARCHER,
    ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER,
    ENEMYID::ID_FEDERATION_SHOOTER,
    ENEMYID::ID_EXO_SUITED_UTROMS,
    ENEMYID::ID_CAPTAIN_OF_FEDERATION,
    ENEMYID::ID_LARGE_MOBSTER,
    ENEMYID::ID_LARGE_FOOT_NINJA,
    ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA,
    ENEMYID::ID_LARGE_FEDERATION_SOLDIET,
    ENEMYID::ID_LARGE_EXO_SUITED_UTROMS,
    ENEMYID::ID_FOOT_TECH_NINJA,
    ENEMYID::ID_FOOT_SUMO_NINJA,
    ENEMYID::ID_FOOT_GUNNER_FOOT_MECH,
    ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA,
    ENEMYID::ID_TRICERATION_SOLDIER,
    ENEMYID::ID_TRICERATION_SHOOTER,
    ENEMYID::ID_TRICERATION_FLYING_HARNESS,
    ENEMYID::ID_STONE_BITER,
    ENEMYID::ID_BERGOX,
    ENEMYID::ID_SPIDER_MONSTER,
    ENEMYID::ID_REDURION,
    ENEMYID::ID_KING_NAIL,
    ENEMYID::ID_GREENPUS,
    ENEMYID::ID_RAZORFIST,
    ENEMYID::ID_MAD_MUNCHER,
    ENEMYID::ID_RYNOKK,
    ENEMYID::ID_BLOOD_SUCKER,
    ENEMYID::ID_POISON_BAT,
    ENEMYID::ID_HYPNOSIS_BAT,
    ENEMYID::ID_BOMB_BAT_MECH,
    ENEMYID::ID_KURAGE,
    ENEMYID::ID_KABUTO,
    ENEMYID::ID_UTROM_SAUCER,
    ENEMYID::ID_MOUSER_ROBOT,
    ENEMYID::ID_MOUSER_ROBOT_B,
    ENEMYID::ID_KROKODIL_MOUSER,
    ENEMYID::ID_RAPTOR,
    ENEMYID::ID_CARNIVOROUS_PLANT,
    ENEMYID::ID_MURAL_CARNIVOROUS_PLANT,
    ENEMYID::ID_NINJA_RATS_A,
    ENEMYID::ID_NINJA_RATS_B,
    ENEMYID::ID_NINJA_RATS_C,
    ENEMYID::ID_NINJA_RATS_D,
    ENEMYID::ID_TRAXIMUS,
    ENEMYID::ID_LEATHER_HEAD,
    ENEMYID::ID_ELITE_FOOT_A,
    ENEMYID::ID_ELITE_FOOT_B,
    ENEMYID::ID_ELITE_FOOT_C,
    ENEMYID::ID_ELITE_FOOT_D,
    ENEMYID::ID_ULTIMATE_NINJA,
    ENEMYID::ID_SPASMOSAUR,
    ENEMYID::ID_HUN_A,
    ENEMYID::ID_HUN_B,
    ENEMYID::ID_KARAI,
    ENEMYID::ID_MIYAMOTO_USAGI,
    ENEMYID::ID_FOOT_MECH_SPLINTER,
    ENEMYID::ID_SLASSHUR,
    ENEMYID::ID_SHREDDER,
    ENEMYID::ID_FEUDAL_JAPAN_SHREDDER,
    ENEMYID::ID_ULTIMATE_SHREDDER,
    ENEMYID::ID_DORAKO,
    ENEMYID::ID_SPLINTER,
    ENEMYID::ID_FUGITOID,
};


/*static*/ CProcess* CEnemySelTestSequence::Instance(void)
{
    return new CEnemySelTestSequence;
};


CEnemySelTestSequence::CEnemySelTestSequence(void)
: m_menu()
, m_eIDLast(ENEMYID::ID_NINJA_RATS_D)
{

};


CEnemySelTestSequence::~CEnemySelTestSequence(void)
{

};


bool CEnemySelTestSequence::OnAttach(const void* pParam)
{
    m_menu.SetDispMax(20);

    m_menu.AddText("<< ENEMY TEST >>");
    m_menu.AddSeparator(true);

    int32 index = -1;
    for (int32 i = 0; i < COUNT_OF(m_aeID); ++i)
    {
        if (m_aeID[i] == m_eIDLast)
            index = m_menu.GetSelectMax();
        
        char szName[256];
        std::sprintf(szName, "%02d:%s", m_aeID[i], ENEMYID::GetName(m_aeID[i]));

        m_menu.AddTrigger(szName, nullptr, reinterpret_cast<void*>(m_aeID[i]));

        if (!ENEMYID::GetImplementProgress(m_aeID[i]))
            m_menu.SetLastItemEnable(false);
    };

    if (index != -1)
        m_menu.SetSelectAtTop(index);

    return true;
};


void CEnemySelTestSequence::OnDetach(void)
{

};


void CEnemySelTestSequence::OnMove(bool bRet, const void* pReturnValue)
{
    m_menu.Period();

    CDebugMenuCtrl::RESULT result = m_menu.GetResult();
    switch (result)
    {
    case CDebugMenuCtrl::RESULT_OK:
        Call(PROCLABEL_SEQ_TESTENEMY, m_menu.GetItemParam());
        break;

    case CDebugMenuCtrl::RESULT_CANCEL:
        Ret();
        break;

    default:
        break;
    };
};


void CEnemySelTestSequence::OnDraw(void) const
{
    m_menu.Draw();
};

#endif /* _DEBUG */