#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CSpaceShipGimmick final : public CGimmick
{
private:
    struct SMOKEDATA
    {
        enum STATUS
        {
            STATUS_GENERATION = 0,
            STATUS_WAIT_ERASE,
            STATUS_END,
        };

        uint32 m_hEffect;
        float m_fSmokeTimer;
        float m_fSmokeGenerateTime;
        STATUS m_status;
    };

    enum SHIPTYPE
    {
        SHIPTYPE_UNKNOWN = -1,
        SHIPTYPE_FED = 0,
        SHIPTYPE_TRI,

        SHIPTYPENUM,
    };

    enum STATUS
    {
        STATUS_IDLE = 0,
        STATUS_RUN,
        STATUS_EXPLOSION,
        STATUS_SMOKE_GENERATION,
        STATUS_END,
    };

public:
    CSpaceShipGimmick(const char* pszName, void* pParam);
    virtual ~CSpaceShipGimmick(void);
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void RegistHitAttack(void);
    bool ExplosionAndModelChange(void);
    void Explosion(void);
    void SeControl(void);
    void GetRandomPosInsideBody(RwV3d* pvPos) const;
    bool SmokeOperation(void);
    bool SmokeGeneration(SMOKEDATA* pSmokeData);

private:
    CMultipleEnemyGenerator m_generator;
    CNormalGimmickModel     m_model;
    int32                   m_nLife;
    STATUS                  m_status;
    float                   m_fTimer;
    float                   m_fInterval;
    float                   m_fDecreaseRate;
    float                   m_fSeTimer;
    SHIPTYPE                m_shipType;
    SMOKEDATA               m_aSmokeData[3];
    uint32                  m_hAtari;
};