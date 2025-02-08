#pragma once

#ifdef _DEBUG

/**
 *  This sequence will allow to select enemy ID that allow to spawn at enemy stage test
 *
 *  WHY: Data managers capabilities are limited to load/unload enemies on the fly.
 *       For example "MotionParameterManager" - with retail implementation its impossible
 *       to clean unused packages without reloading whole stage and many others.
 *       So currently enemy test is limited to just "one" enemy type per test
 */

class CProcess* EnemyTestSelectSequenceInstance(void);

#endif /* _DEBUG */