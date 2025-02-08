#pragma once

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Component/Effect/EffectID.hpp"

#ifdef _DEBUG

void EnemyTestMakeEnemyTable(class CSequence* pCaller);
void EnemyTestMakeEffectTable(class CSequence* pCaller);
void EnemyTestMakeFileEnemy(class CSequence* pCaller, ENEMYID::VALUE enemyId);
void EnemyTestMakeFileEnemyAll(class CSequence* pCaller);
void EnemyTestMakeFileEffectAll(class CSequence* pCaller);
void EnemyTestMakeFileItems(class CSequence* pCaller);
const char* EnemyTestGetItemNeededFile(int32 no);
const char* EnemyTestGetFilename(ENEMYID::VALUE enemyId);
int32 EnemyTestGetFileId(ENEMYID::VALUE enemyId);
const char* EnemyTestGetEffectFilename(EFFECTID::VALUE effectId);
int32 EnemyTestGetEffectFileId(EFFECTID::VALUE effectId);
class CProcess* EnemyTestMakeFileSequenceInstance(void);

#endif /* _DEBUG */