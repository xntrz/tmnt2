#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CStageCommonLoadInfo final : public CGameObjLoadInfo
{
public:
    CStageCommonLoadInfo(GAMETYPES::STAGEMODE stagemode);
    virtual ~CStageCommonLoadInfo(void);
    virtual FNAME GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    GAMETYPES::STAGEMODE m_stagemode;
};


class CStageGimmickSetDataLoadInfo final : public CGameObjLoadInfo
{
public:
    CStageGimmickSetDataLoadInfo(STAGEID::VALUE idStage);
    virtual ~CStageGimmickSetDataLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    STAGEID::VALUE m_idStage;
};


class CStageLoadInfo final : public CGameObjLoadInfo
{
public:
    CStageLoadInfo(STAGEID::VALUE idStage);
    virtual ~CStageLoadInfo(void);
    virtual FNAME GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    STAGEID::VALUE m_idStage;
};