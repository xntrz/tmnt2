#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CTransferFloorGimmickMove;
class CRotateTransferFloorGimmickMove;


class CTransferFloorGimmick : public CGimmick
{
public:
    CTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CTransferFloorGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    
protected:
    uint32                      m_hAtari;
    CNormalGimmickModel         m_model;
    CTransferFloorGimmickMove*  m_pTransferMove;
    RwV3d                       m_vPreMovePosition;
};


class CLinearTransferFloorGimmick final : public CTransferFloorGimmick
{
public:
    CLinearTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CLinearTransferFloorGimmick(void) {};
};


class CPathTransferFloorGimmick final : public CTransferFloorGimmick
{
public:
    CPathTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CPathTransferFloorGimmick(void) {};
};


class CRotateTransferFloorGimmick final : public CTransferFloorGimmick
{
public:
    CRotateTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CRotateTransferFloorGimmick(void) {};
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CRotateTransferFloorGimmickMove* m_pRotateMove;
    RwV3d                            m_vPreMoveRotation;
    bool                             m_bSE;
};