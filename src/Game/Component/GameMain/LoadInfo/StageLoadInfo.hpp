#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/GameMain/StageID.hpp"


class CStageLoadInfo final : public CGameObjLoadInfo
{
public:
    CStageLoadInfo(STAGEID::VALUE idStage);
    virtual ~CStageLoadInfo(void);
    virtual int32 GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    STAGEID::VALUE m_idStage;
};