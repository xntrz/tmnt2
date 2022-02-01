#pragma once

#include "GimmickTypes.hpp"
#include "GimmickID.hpp"

#include "Game/Component/GameMain/StageID.hpp"


class CGimmick;


class CGimmickManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Draw(int32 nPriority);
    static void DispatchEvent(void);
    static CGimmick* Create(GIMMICKID::VALUE idGimmick, int32 subid, void* pParam);
    static void Release(CGimmick* pGimmick);
    static CGimmick* Find(const char* pszName);    
    static void NotifyPlayStarted(void);
    static void CreateSetGimmick(void);
    static bool IsPlayStarted(void);
    static void SetPlayerStartPosition(int32 nPlayerNo, bool bProtect);
    static void ReplacePlayer(int32 nPlayerNo, const RwV3d* pvPosition, bool bProtect);
    static void PostEvent(const char* pszReceiver, const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype, void* pParam = nullptr);
    static void CreateStageSpecialGimmick(STAGEID::VALUE idStage);
};