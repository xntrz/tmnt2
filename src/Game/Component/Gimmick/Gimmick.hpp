#pragma once

#include "GimmickTypes.hpp"
#include "GimmickID.hpp"

#include "Game/System/GameObject/GameObject.hpp"


class CGimmickModel;
class CGimmickMove;
class CGimmickQuery;
class CHitAttackData;
class CHitCatchData;


class CGimmick : public CGameObject
{
public:
    CGimmick(const char* pszName, void* pParam);
    virtual ~CGimmick(void);
    virtual void Run(void) override;
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void Draw(void) const;
    virtual void GetPosition(RwV3d* pvPosition) const;
    virtual void GetCenterPosition(RwV3d* pvPosition) const;
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const;
    virtual bool Query(CGimmickQuery* pQuery) const;
    virtual void PreMove(void);
    virtual void PostMove(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);
    virtual void OnTouchedDown(void);
    virtual void OnDamaged(void);
    virtual void OnDie(void);
    virtual void OnCatchAttack(CHitAttackData* pAttack);
    virtual void OnAttackResult(CHitCatchData* pCatch);
    void Release(void);
    bool IsSleep(void) const;
    bool CheckFeature(GIMMICKTYPES::FEATURE feature) const;

    inline void Init(GIMMICKID::VALUE idGimmick)        { m_id = idGimmick; };
    inline GIMMICKID::VALUE GetID(void) const           { return m_id; };
    inline void SetModelStrategy(CGimmickModel* pModel) { m_pModelStrategy = pModel; };
    inline void SetMoveStrategy(CGimmickMove* pMove)    { m_pMoveStrategy = pMove; };
    inline CGimmickMove* GetMoveStrategy(void) const    { return m_pMoveStrategy; };
    
protected:
    GIMMICKID::VALUE m_id;
    CGimmickModel* m_pModelStrategy;
    CGimmickMove* m_pMoveStrategy;
#ifdef _DEBUG
    RwV3d m_vDebugInitialPos;
#endif    
};