#include "057UtromSaucerAI.hpp"
#include "056KabutoAI.hpp"


/*static*/ CAIModerator* C057UtromSaucerAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C056KabutoAI(pEnemyChr);
};