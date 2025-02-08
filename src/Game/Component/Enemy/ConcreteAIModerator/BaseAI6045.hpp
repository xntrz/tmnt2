#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"


class CEnemyAIDecisionUnitManager
{
public:
    
};

enum SPECIAL_FLAG
{
    FLAG_COUNTERATTACK              = 0x1,
    FLAG_CHANGEDECISION             = 0x2,
    FLAG_MOVE_TURN                  = 0x4,
    FLAG_MOVE_BACK                  = 0x8,
    FLAG_MOVE_HITANDAWAY            = 0x10,
    FLAG_TRIGGER_MOVERUN_ENABLE     = 0x20,
    FLAG_TRIGGER_ATTACK_ENABLE      = 0x40,
    FLAG_TRIGGER_ESCAPE_ENABLE      = 0x80,
    FLAG_TRIGGER_REDUCTIONHP_ENABLE = 0x100,
    FLAG_TRIGGER_MOVERUN_PERMIT     = 0x200,
    FLAG_TRIGGER_ATTACK_PERMIT      = 0x400,
    FLAG_TRIGGER_ESCAPE_PERMIT      = 0x800,
    FLAG_TRIGGER_REDUCTIONHP_PERMIT = 0x1000,

    FLAG_DEFAULT                    = 0,
    
    FLAG_MOVE_ALL                   = FLAG_MOVE_TURN
                                    | FLAG_MOVE_BACK
                                    | FLAG_MOVE_HITANDAWAY,

    FLAG_TRIGGER_ENABLE_ALL         = FLAG_TRIGGER_MOVERUN_ENABLE
                                    | FLAG_TRIGGER_ATTACK_ENABLE
                                    | FLAG_TRIGGER_ESCAPE_ENABLE
                                    | FLAG_TRIGGER_REDUCTIONHP_ENABLE,

    FLAG_TRIGGER_PERMIT_ALL         = FLAG_TRIGGER_MOVERUN_PERMIT
                                    | FLAG_TRIGGER_ATTACK_PERMIT
                                    | FLAG_TRIGGER_ESCAPE_PERMIT
                                    | FLAG_TRIGGER_REDUCTIONHP_PERMIT,
};

class CBaseAI6045 : public CAIModerator
{
public:

private:

    
};