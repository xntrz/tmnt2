#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CTransferFloorGimmickMove;

class CUniqueTransferFloorGimmickModel final : public CGimmickModel
{
public:
    enum TYPE
    {
        TYPE_DISP = 0,
        TYPE_ATARI0,
        TYPE_ATARI1,

        TYPENUM,
    };
    
public:
    CUniqueTransferFloorGimmickModel(void);
    virtual ~CUniqueTransferFloorGimmickModel(void) {};
    void SetModel(TYPE type, CModel* pModel);
    RpClump* GetModelClump(TYPE type);
};


class CUniqueTransferFloorGimmick : public CGimmick
{
public:
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_RUN,
        STATE_STOP,
    };

public:
    CUniqueTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CUniqueTransferFloorGimmick(void);
    uint32 CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE modelType, float fInitialRadius);

protected:
    CTransferFloorGimmickMove*          m_pTransferMove;
    CUniqueTransferFloorGimmickModel    m_model;
    uint32                              m_hAtari0;
    uint32                              m_hAtari1;
    RwV3d                               m_vPreMovePosition;
    float                               m_fMoveTime;
    float                               m_fOneWayMoveTime;
    STATE                               m_eState;
};


class CUniqueLinearTransferFloorGimmick : public CUniqueTransferFloorGimmick
{
public:    
    CUniqueLinearTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CUniqueLinearTransferFloorGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
};


class CUniquePathTransferFloorGimmick : public CUniqueTransferFloorGimmick
{
public:
    CUniquePathTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CUniquePathTransferFloorGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
};