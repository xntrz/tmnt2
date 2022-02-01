#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CStageCommonLoadInfo final : public CGameObjLoadInfo
{
public:
    CStageCommonLoadInfo(GAMETYPES::STAGEMODE stagemode);
    virtual ~CStageCommonLoadInfo(void);
    virtual int32 GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    GAMETYPES::STAGEMODE m_stagemode;
};