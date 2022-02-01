#pragma once


class CMapCamera;


class IGameStageCameraUpdater
{
public:
    virtual void Update(CMapCamera* pMapCamera) = 0;
};


class CDefaultCameraUpdater : public IGameStageCameraUpdater
{
public:
    static CDefaultCameraUpdater* Instance(void);
    
    virtual void Update(CMapCamera* pMapCamera) override;
};


class CBossBattleCameraUpdater : public IGameStageCameraUpdater
{
public:
    static CBossBattleCameraUpdater* Instance(void);

    virtual void Update(CMapCamera* pMapCamera) override;
};