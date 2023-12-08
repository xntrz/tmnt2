#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CRideGimmickMove;
class CModuleManager;


class CRideGimmick : public CGimmick
{
protected:
    enum PROCESSTYPE
    {
        PROCESSTYPE_DRAW_SHADOW = 0,
        PROCESSTYPE_ATTACK,
        PROCESSTYPE_DAMAGE,
        PROCESSTYPE_DRAW_MODEL,
        
        PROCESSTYPENUM,
    };
    
public:
    CRideGimmick(const char* pszName, void* pParam);
    virtual ~CRideGimmick(void);
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    bool IsProcess(PROCESSTYPE processtype) const;
    
protected:
    CRideGimmickMove* m_pRideMove;
    CModuleManager* m_pModuleManager;
    CNormalGimmickModel m_model;
    bool m_bCatchHit;
    int32 m_iPower;
    float m_fRadius;
    float m_fOffset;
};