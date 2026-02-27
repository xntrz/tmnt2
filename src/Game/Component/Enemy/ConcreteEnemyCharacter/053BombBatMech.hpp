#pragma once

#include "BaseBatChr.hpp"


class C053BombBatMech : public CBaseBatChr
{
public:
    class CBodyBlowStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_IDLE = 0,
            MOTIONID_B1,
            MOTIONID_B2,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;

    private:
        float m_fTimer;
        uint32 m_ahEffect[2];
        int32 m_targetNo;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C053BombBatMech(void);
    virtual ~C053BombBatMech(void);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
};