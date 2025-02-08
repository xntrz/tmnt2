#pragma once

#include "EnemyTypes.hpp"
#include "AIThinkOrder.hpp"

#include "Game/System/Character/CharacterTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Effect/EffectID.hpp"


/**
 *  Defines array of required effects for enemy
 *
 *  USAGE example: DEFINE_NEEDED_EFFECTS_FOR(C002FootNinja, { EFFECTID::ID_KO_FLASH });
 */
#define DEFINE_NEEDED_EFFECTS_FOR(T, ...)                       \
    EFFECTID::VALUE T::GetNeededEffect(int32 no)                \
    {                                                           \
        static EFFECTID::VALUE s_aNeededEffect[] = __VA_ARGS__; \
                                                                \
        if ((no >= 0) && (no < COUNT_OF(s_aNeededEffect)))      \
            return s_aNeededEffect[no];                         \
                                                                \
        return EFFECTID::ID_UNKNOWN;                            \
    }


/**
 *  Defines that there is no effect required for specified enemy
 *
 *  USAGE example: DEFINE_EMPTY_NEEDED_EFFECTS_FOR(C002FootNinja);
 */
#define DEFINE_EMPTY_NEEDED_EFFECTS_FOR(T)                      \
    EFFECTID::VALUE T::GetNeededEffect(int32 no)                \
    {                                                           \
        return EFFECTID::ID_UNKNOWN;                            \
    }


class CAIModerator;
class CHitCatchData;
class CHitAttackData;
class CCharacterAttackCalculator;
class CCharacterCompositor;


class CEnemyCharacter
{
public:
    enum CHRTYPE
    {
        CHRTYPE_ENEMY = 0,
        CHRTYPE_PLAYER,
    };
    
    struct PARAMETER
    {
        CAIModerator*               (*m_pfnAIInstance)(CEnemyCharacter* pEnemyChr);
        bool                        m_bToon;
        float                       m_fShadowRadius;
        ENEMYTYPES::FEATURE         m_feature;
        ENEMYTYPES::CHARACTERISTIC  m_AICharacteristic;
        int32                       m_iFrequencyMax;
        uint8*                      m_puFrequencyParam;
    };

    class CStatusSubject;

    class CStatusObserver
    {
    public:
        friend CStatusSubject;

        enum RESULT
        {
            RESULT_CONTINUE = 0,
            RESULT_END,
        };

    public:
        inline CStatusObserver(void) : m_pEnemyChr(nullptr), m_pSharedData(nullptr), m_eOwnStatus(ENEMYTYPES::STATUS_MAX) {};
        virtual ~CStatusObserver(void) {};
        virtual void Append(void) {};
        virtual void Remove(void) {};
        virtual void Update(void) {};
        virtual void OnStart(void) = 0;
        virtual RESULT Observing(void) = 0;
        virtual ENEMYTYPES::STATUS OnEnd(void) = 0;

        inline ENEMYTYPES::STATUS Status(void) const { return m_eOwnStatus; };
        inline CEnemyCharacter& EnemyChr(void) { return *m_pEnemyChr; };
        inline const CEnemyCharacter& EnemyChr(void) const { return *m_pEnemyChr; };
        
    private:
        void*               m_pSharedData;
        CEnemyCharacter*    m_pEnemyChr;
        ENEMYTYPES::STATUS  m_eOwnStatus;
    };
    
    class CStatusSubject
    {
    public:
        CStatusSubject(CEnemyCharacter* pSubject);
        ~CStatusSubject(void);
        bool Attach(ENEMYTYPES::STATUS status, CStatusObserver* pStatusObserver);
        void Detach(ENEMYTYPES::STATUS eStatus);
        void Update(void);
        void OnStart(void);
        ENEMYTYPES::STATUS OnEnd(void);
        CStatusObserver::RESULT Observing(void);
        void Status(ENEMYTYPES::STATUS eNewStatus);
        bool IsContained(ENEMYTYPES::STATUS eStatus) const;
        ENEMYTYPES::STATUS Status(void);
        ENEMYTYPES::STATUS StatusPrev(void);
        void SetSharedData(void* pSharedData);
        void* GetSharedData(void);

    private:
        std::vector<CStatusObserver*>   m_apStatusObservers;        
        ENEMYTYPES::STATUS              m_eStatusPrev;
        ENEMYTYPES::STATUS              m_eStatus;
        void*                           m_pSharedData;
        CEnemyCharacter*                m_pEnemyChr;
    };
    
public:
    static void PreInitialize(const ENEMYTYPES::CREATEINFO* pCreateInfo);
    
    CEnemyCharacter(ENEMYID::VALUE idEnemy);
    virtual ~CEnemyCharacter(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter);
    virtual void Run(void);
    virtual void Draw(void);
    virtual void OnStart(void);
    virtual void OnStop(void);
    virtual bool OnMessageCanChangingAerial(float fHeight);
    virtual void OnMessageDoChangingAerial(void);
    virtual void OnMessageTouchdown(float fHeight);
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc);
    virtual void OnMessageReceivedDamage(int32 iAmount);
    virtual void OnSteppedDeathFloor(void);
    virtual void OnMessageCatch(void* pParam);
    virtual void OnMessageLift(void* pParam);
    virtual void OnMessageThrow(void* pParam);
    virtual void OnMessageMissThrow(void* pParam);
    virtual bool AttachStatusObserver(ENEMYTYPES::STATUS status, CStatusObserver* pStatusObserver);
    virtual void DetachStatusObserver(ENEMYTYPES::STATUS eStatus);
    virtual bool IsAttachedStatusObserver(ENEMYTYPES::STATUS eStatus);
    virtual void SetOwner(uint32 hOwner);
    virtual uint32 GetOwner(void);
    virtual bool SetStatus(ENEMYTYPES::STATUS eStatus);
    virtual ENEMYTYPES::STATUS GetStatus(void) const;
    virtual void SetPatrolArea(const RwV3d* pvPosition, float fRadius);
    virtual void SetPosition(const RwV3d* pvPosition);
    virtual void GetBodyPosition(RwV3d* pvPosition) const;
    virtual void SetDirection(float fDirection);
    virtual void SetSharedData(void* pSharedData);
    virtual CHRTYPE GetAttackCharacterType(void);
    virtual ENEMYID::VALUE GetID(void);
    virtual const char* GetName(void);
    virtual CHARACTERTYPES::DEFENCERSTATUSFLAG CheckDefenceStatusFlag(void);
    void Delete(void);
    void Start(void);
    void Stop(void);
    void StartAI(void);
    void StopAI(void);
    void SetFlag(ENEMYTYPES::FLAG flag);
    void ClearFlag(ENEMYTYPES::FLAG flag);
    bool TestFlag(ENEMYTYPES::FLAG flag) const;
    void* SharedData(void) const;
    CAIModerator& AIModerator(void) const;
    CAIThinkOrder& AIThinkOrder(void) const;
    CStatusSubject& StatusSubject(void) const;
    const ENEMYTYPES::CHARACTERISTIC& AICharacteristic(void) const;
    PARAMETER& CharacterParameter(void);
    const PARAMETER& CharacterParameter(void) const;
    uint8* FrequencyParameter(void) const;
    uint8 FrequencyParameter(int32 idx) const;
    CCharacterCompositor& Compositor(void);
    const CCharacterCompositor& Compositor(void) const;
    const ENEMYTYPES::FEATURE& Feature(void) const;

private:
    static ENEMYTYPES::CREATEINFO   m_createinfo;
    CStatusSubject*                 m_pStatusSubject;
    uint32                          m_hCharacter;    // CCharacterCompositor handle
    CAIModerator*                   m_pAIModerator;
    ENEMYID::VALUE                  m_ID;
    uint32                          m_hOwner;        // CEnemy handle
    mutable CAIThinkOrder           m_AIThinkOrder;
    PARAMETER*                      m_pParameter;
    ENEMYTYPES::FLAG                m_eflag;
    uint8*                          m_puFrequencyParam;
    bool                            m_bRunning;
    bool                            m_bRunningAI;
};