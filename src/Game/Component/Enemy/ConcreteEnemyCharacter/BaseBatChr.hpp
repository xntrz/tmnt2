#pragma once

#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/BaseBatAI.hpp"


class CBaseBatChr : public CBaseFlyingEnemyChr
{
public:
    class CMoveStatusObserver : public CBaseFlyingEnemyChr::CMoveStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual float AltitudeNoise(void) override;

    private:
        int32 m_target;
    };

public:
    CBaseBatChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseBatChr(void);
    virtual void Run(void) override;
};