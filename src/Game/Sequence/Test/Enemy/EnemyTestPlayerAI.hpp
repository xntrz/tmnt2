#pragma once

#ifdef _DEBUG

/**
 *  Debug Tool process that intercepts required stage sequence and add custom player AI for all inactive game players.
 */

void EnemyTestPlayerAILaunch(class CProcess* pSender);
void EnemyTestPlayerAIKill(class CProcess* pSender);
class CProcess* EnemyTestPlayerAIInstance(void);

#endif /* _DEBUG */