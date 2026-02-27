#pragma once

#include "BaseGeneralEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/BaseGunnerAI.hpp"
#include "Game/Component/Shot/ShotID.hpp"
#include "Game/Component/Effect/MagicID.hpp"
#include "Game/Component/Effect/EffectID.hpp"


class CBaseGunnerEnemyChr : public CBaseGeneralEnemyChr
{
public:
    class CHideStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CFireStatusObserver : public CMotionManagedStatusObserver
    {
    public:
        struct PARAM
        {
            enum TYPE
            {
                TYPE_EFFECT = 0,
                TYPE_MAGIC,
                TYPE_SHOT,
            };

            TYPE type;
            union
            {
                EFFECTID::VALUE effectId;
                MAGICID::VALUE  magicId;
                SHOTID::VALUE   shotId;
            };
            int32 repeatCount;
            bool  bRepeatChase;
            int32 boneId;
        };

    protected:
        enum
        {
            MOTIONID_FIRE_START = 0,
            MOTIONID_FIRE,
            MOTIONID_FIRE_END,
        };

    public:
        CFireStatusObserver(const PARAM* pParam);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void Update(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;
        virtual void Fire(void);

    private:
        PARAM m_param;
        int32 m_fireNo;
        RwV3d m_vecAt;
        int32 m_playerNo;
        bool  m_bContinuationFire;
    };

public:
    CBaseGunnerEnemyChr(ENEMYID::VALUE idEnemy);
    virtual ~CBaseGunnerEnemyChr(void);
};