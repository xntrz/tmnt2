#include "EnemyCharacter.hpp"
#include "CharacterCompositor.hpp"
#include "EnemyParameter.hpp"
#include "Enemy.hpp"
#include "AIModerator.hpp"

#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


CEnemyCharacter::CStatusObserver::CStatusObserver(CEnemyCharacter& rEnemyCharacter, ENEMYTYPES::STATUS eOwnStatus)
: m_rEnemyCharacter(rEnemyCharacter)
, m_pSharedData(m_rEnemyCharacter.StatusSubject().GetSharedData())
, m_eOwnStatus(eOwnStatus)
{
    ;
};


CEnemyCharacter::CStatusObserver::~CStatusObserver(void)
{
    ;
};


void CEnemyCharacter::CStatusObserver::Append(void)
{
    ;
};


void CEnemyCharacter::CStatusObserver::Remove(void)
{
    ;
};


void CEnemyCharacter::CStatusObserver::Update(void)
{
    ;
};


ENEMYTYPES::STATUS CEnemyCharacter::CStatusObserver::Status(void) const
{
    return m_eOwnStatus;
};


CEnemyCharacter::CStatusSubject::CStatusSubject(void)
: m_eStatusPrev(ENEMYTYPES::STATUS_HIDE)
, m_eStatus(ENEMYTYPES::STATUS_HIDE)
, m_pSharedData(nullptr)
{
    m_apStatusObservers.reserve(ENEMYTYPES::STATUS_MAX);
};


CEnemyCharacter::CStatusSubject::~CStatusSubject(void)
{
    for (int32 i = 0; i < int32(m_apStatusObservers.size()); ++i)
    {
        CStatusObserver* pStatusObserer = m_apStatusObservers[i];
        if (pStatusObserer)
        {
            pStatusObserer->Remove();
            delete pStatusObserer;
            m_apStatusObservers[i] = nullptr;
        };
    };

    m_apStatusObservers.clear();
};


bool CEnemyCharacter::CStatusSubject::Attach(CStatusObserver* pStatusObserver)
{
    ASSERT(pStatusObserver);

    ENEMYTYPES::STATUS StatusObserverID = pStatusObserver->Status();
    ASSERT( (StatusObserverID >= 0) && (StatusObserverID < int32(m_apStatusObservers.size())) );
    
    if (m_apStatusObservers[StatusObserverID] != pStatusObserver)
    {
        if (m_apStatusObservers[StatusObserverID])
            Detach(StatusObserverID);

        m_apStatusObservers[StatusObserverID] = pStatusObserver;
        m_apStatusObservers[StatusObserverID]->Append();

        return true;
    };

    return false;
};


void CEnemyCharacter::CStatusSubject::Detach(ENEMYTYPES::STATUS eStatus)
{
    ASSERT( (eStatus >= 0) && (eStatus < int32(m_apStatusObservers.size())) );
    
    if (m_apStatusObservers[eStatus])
    {
        m_apStatusObservers[eStatus]->Remove();
        delete m_apStatusObservers[eStatus];
        m_apStatusObservers[eStatus] = nullptr;
    };
};


void CEnemyCharacter::CStatusSubject::Update(void)
{
    if (m_apStatusObservers[m_eStatus])
        m_apStatusObservers[m_eStatus]->Update();
};


void CEnemyCharacter::CStatusSubject::OnStart(void)
{
    ASSERT(m_apStatusObservers[m_eStatus]);
    m_apStatusObservers[m_eStatus]->OnStart();
};


void CEnemyCharacter::CStatusSubject::OnEnd(void)
{
    ASSERT(m_apStatusObservers[m_eStatus]);
    m_apStatusObservers[m_eStatus]->OnEnd();
};


void CEnemyCharacter::CStatusSubject::Observing(void)
{
    ASSERT(m_apStatusObservers[m_eStatus]);
    m_apStatusObservers[m_eStatus]->Observing();
};


void CEnemyCharacter::CStatusSubject::Status(ENEMYTYPES::STATUS eNewStatus)
{
    m_eStatusPrev = m_eStatus;
    m_eStatus = eNewStatus;
};


bool CEnemyCharacter::CStatusSubject::IsContained(ENEMYTYPES::STATUS eStatus) const
{
    return (m_apStatusObservers[eStatus] != nullptr);
};


ENEMYTYPES::STATUS CEnemyCharacter::CStatusSubject::Status(void)
{
    return m_eStatus;
};


ENEMYTYPES::STATUS CEnemyCharacter::CStatusSubject::StatusPrev(void)
{
    return m_eStatusPrev;
};


void CEnemyCharacter::CStatusSubject::SetSharedData(void* pSharedData)
{
    m_pSharedData = pSharedData;
};


void* CEnemyCharacter::CStatusSubject::GetSharedData(void)
{
    return m_pSharedData;
};


/*static*/ ENEMYTYPES::CREATEINFO CEnemyCharacter::m_createinfo;


/*static*/ void CEnemyCharacter::PreInitialize(const ENEMYTYPES::CREATEINFO* pCreateInfo)
{
    std::memcpy(&m_createinfo, pCreateInfo, sizeof(m_createinfo));
};


CEnemyCharacter::CEnemyCharacter(ENEMYID::VALUE idEnemy)
: m_pStatusSubject(nullptr)
, m_hCharacter(0)
, m_pAIModerator(nullptr)
, m_ID(idEnemy)
, m_pParameter(nullptr)
, m_eflag(ENEMYTYPES::FLAG_NONE)
, m_puFrequencyParam(nullptr)
, m_bRunning(false)
, m_bRunningAI(false)
{
    char szName[GAMEOBJECTTYPES::NAME_MAX];
    szName[0] = '\0';
    std::sprintf(szName, "%s_%04d", ENEMYID::GetExtName(m_ID), CEnemy::m_iUniqueCount);
    ASSERT(std::strlen(szName) < GAMEOBJECTTYPES::NAME_MAX);

    CCharacterCompositor* pCharacter = new CCharacterCompositor(szName, CCharacter::TYPE_ENEMY, *this);
    ASSERT(pCharacter);

    m_hCharacter = pCharacter->GetHandle();
    CGameObjectManager::SendMessage(pCharacter, GAMEOBJECTTYPES::MESSAGEID_SLEEP);

    m_pStatusSubject = new CStatusSubject();
    ASSERT(m_pStatusSubject);

    m_pParameter = new PARAMETER;
    ASSERT(m_pParameter);
    std::memset(m_pParameter, 0x00, sizeof(*m_pParameter));
    m_pParameter->m_feature.m_iPattern = m_createinfo.m_iPattern;
};


CEnemyCharacter::~CEnemyCharacter(void)
{
    Delete();
    
    CCharacterCompositor* pCharacter = (CCharacterCompositor*)CGameObjectManager::GetObject(m_hCharacter);
    if (pCharacter)
        delete pCharacter;    
};


bool CEnemyCharacter::Initialize(PARAMETER* pParameter, bool bReplaceParameter)
{
    ASSERT(pParameter);
    ASSERT(m_pParameter);

    std::memcpy(m_pParameter, pParameter, sizeof(*m_pParameter));
    m_pParameter->m_feature.m_iPattern = m_createinfo.m_iPattern;

    int32 idx = CEnemyParameter::Search(m_ID, m_pParameter->m_feature.m_iPattern);
    if ((idx >= 0) && bReplaceParameter)
    {
        m_pParameter->m_feature.m_iHPMax = CEnemyParameter::GetHP(idx);

        std::memcpy(
            &m_pParameter->m_AICharacteristic,
            &CEnemyParameter::GetAICharacteristic(idx),
            sizeof(m_pParameter->m_AICharacteristic)
        );

        int32 iFreqMax = CEnemyParameter::GetFrequencyMax(idx);
        if (iFreqMax > 0)
        {
            m_puFrequencyParam = new uint8[GAMETYPES::DIFFICULTY_NUM * iFreqMax];
            ASSERT(m_puFrequencyParam);
            std::memset(m_puFrequencyParam, 0x00, (GAMETYPES::DIFFICULTY_NUM * iFreqMax));
            for (int32 i = 0; i < iFreqMax; ++i)
                m_puFrequencyParam[i] = CEnemyParameter::GetFrequency(idx, i);
        };
    }
    else
    {
        int32 iFreqMax = m_pParameter->m_iFrequencyMax;
        if (iFreqMax > 0)
        {
            m_puFrequencyParam = new uint8[GAMETYPES::DIFFICULTY_NUM * iFreqMax];
            ASSERT(m_puFrequencyParam);
            std::memset(m_puFrequencyParam, 0x00, (GAMETYPES::DIFFICULTY_NUM * iFreqMax));
            for (int32 i = 0; i < iFreqMax; ++i)
                m_puFrequencyParam[i] = m_pParameter->m_puFrequencyParam[i];
        };
    };

    m_pParameter->m_feature.m_iHP = m_pParameter->m_feature.m_iHPMax;
    m_pParameter->m_feature.m_vPatrolOrigin = m_createinfo.m_vPosition;
    m_pParameter->m_feature.m_fPatrolRadius = m_createinfo.m_fRadius;

    if (m_pParameter->m_AICharacteristic.m_fRatioOfMove < 0.05f)
        m_pParameter->m_AICharacteristic.m_fRatioOfMove = 0.0f;

    if (Math::FEqual(m_pParameter->m_AICharacteristic.m_fRatioOfMove, 0.0f))
    {
        m_pParameter->m_feature.m_fPatrolRadius = 0.0f;
    }
    else
    {
        if ((m_pParameter->m_feature.m_fPatrolRadius > 0.0f) &&
            (m_pParameter->m_feature.m_fPatrolRadius < m_pParameter->m_AICharacteristic.m_fDistanceOfSuitable))
        {
            m_pParameter->m_feature.m_fPatrolRadius = m_pParameter->m_AICharacteristic.m_fDistanceOfSuitable;
        };
    };

    CCharacter::PARAMETER ChrParameter;
    std::memset(&ChrParameter, 0x00, sizeof(ChrParameter));
    ChrParameter.m_pszMotionSetName = ENEMYID::GetExtName(m_ID);
    ChrParameter.m_bToon = m_pParameter->m_bToon;
    CharacterCompositor().Initialize(&ChrParameter);
    CharacterCompositor().ModuleShadow(m_pParameter->m_fShadowRadius);
    CharacterCompositor().ModuleRipple(m_pParameter->m_fShadowRadius);
    CharacterCompositor().SetPosition(&m_createinfo.m_vPosition);
    CharacterCompositor().SetDirection(m_createinfo.m_fDirection);

    if (m_pParameter->m_pfnAIInstance)
    {
        m_pAIModerator = m_pParameter->m_pfnAIInstance();
        ASSERT(m_pAIModerator);
    };

    SetStatus(m_createinfo.m_status);
	return true;
};


void CEnemyCharacter::Run(void)
{
    if (!m_bRunning)
        return;

    if (m_pAIModerator && m_bRunningAI)
        m_pAIModerator->Run();

    StatusSubject().Observing();

    
};


void CEnemyCharacter::Draw(void)
{
    ;
};


void CEnemyCharacter::Delete(void)
{
    if (m_pAIModerator)
    {
        delete m_pAIModerator;
        m_pAIModerator = nullptr;
    };

    if (m_pStatusSubject)
    {
        delete m_pStatusSubject;
        m_pStatusSubject = nullptr;
    };

    if (m_pParameter)
    {
        if (m_pParameter->m_iFrequencyMax)
        {
            delete[] m_puFrequencyParam;
            m_puFrequencyParam = nullptr;
        };

        delete m_pParameter;
        m_pParameter = nullptr;
    };

    m_bRunning = false;
};


void CEnemyCharacter::OnStart(void)
{
    ;
};


void CEnemyCharacter::OnStop(void)
{
    ;
};


bool CEnemyCharacter::OnMessageCanChangingAerial(float fHeight)
{
    if (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
        return false;

    switch (GetStatus())
    {
    case ENEMYTYPES::STATUS_HIDE:
    case ENEMYTYPES::STATUS_DEATH:
    case ENEMYTYPES::STATUS_APPEAR:
    case ENEMYTYPES::STATUS_AERIAL:
    case ENEMYTYPES::STATUS_JUMP:
    case ENEMYTYPES::STATUS_JUMP_2ND:
    case ENEMYTYPES::STATUS_JUMP_ATTACK:
    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK:
    case ENEMYTYPES::STATUS_CRASHWALL_FRONT:
    case ENEMYTYPES::STATUS_CRASHWALL_BACK:
    case ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT:
    case ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK:
    case ENEMYTYPES::STATUS_CAUGHT:
    case ENEMYTYPES::STATUS_LIFTED:
    case ENEMYTYPES::STATUS_LIFTED_WALK:
    case ENEMYTYPES::STATUS_THROWN_FRONT:
    case ENEMYTYPES::STATUS_THROWN_BACK:
        return false;

    default:
        return true;
    };
};


void CEnemyCharacter::OnMessageDoChangingAerial(void)
{
    SetStatus(ENEMYTYPES::STATUS_AERIAL);
};


void CEnemyCharacter::OnMessageTouchdown(float fHeight)
{
    ENEMYTYPES::STATUS ePrevStatus = GetStatus();
    
    if (GetStatus() == ENEMYTYPES::STATUS_THROWN_BACK)
        return;

    switch (GetStatus())
    {
    case ENEMYTYPES::STATUS_THROWN_FRONT:
        {
            SetStatus(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT);
            if (CharacterCompositor().GetRequestedDamage() > 0)
            {
                CGameObjectManager::SendMessage(
                    &CharacterCompositor(),
                    CHARACTERTYPES::MESSAGEID_RECVDMG,
                    (void*)CharacterCompositor().GetRequestedDamage()
                );
                CharacterCompositor().RequestDamage(0);
            };
        }
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK:
        {
            SetStatus(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK);
            CGameObjectManager::SendMessage(&CharacterCompositor(), CHARACTERTYPES::MESSAGEID_RECVDMG, (void*)20);
        }
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT:
        {
            SetStatus(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT);
            CGameObjectManager::SendMessage(&CharacterCompositor(), CHARACTERTYPES::MESSAGEID_RECVDMG, (void*)20);
        }
        break;

    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
        {
            SetStatus(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK);
        }        
        break;

    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
        {
            SetStatus(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT);            
        }
        break;

    case ENEMYTYPES::STATUS_AERIAL:
    case ENEMYTYPES::STATUS_JUMP_ATTACK:
    case ENEMYTYPES::STATUS_JUMP_2ND:
    case ENEMYTYPES::STATUS_JUMP:
        {
            ;
        }
        break;

    default:
        return;
    };

    if (GetStatus() == ePrevStatus)
    {
        if (!SetStatus(ENEMYTYPES::STATUS_TOUCHDOWN))
        {
            if (!SetStatus(ENEMYTYPES::STATUS_IDLE))
                ASSERT(false);
        };
    };
};


void CEnemyCharacter::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    ;
};


void CEnemyCharacter::OnMessageAttackResult(CHitCatchData* pCatch)
{
    if (pCatch->GetResult() == CHitCatchData::RESULT_GUARDIMPACT)
        SetStatus(ENEMYTYPES::STATUS_COUNTERACT);
};


CHARACTERTYPES::ATTACKRESULTTYPE CEnemyCharacter::OnDamage(CCharacterAttackCalculator& rCalc)
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG DefenceStatusFlag = CheckDefenceStatusFlag();    
    CHARACTERTYPES::ATTACKRESULTTYPE AttackResult = rCalc.CalcAtackParameter(
        CharacterCompositor().AttackParameter(),
        DefenceStatusFlag
    );

    if (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
    {
        AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    }
    else if (TestFlag(ENEMYTYPES::FLAG_INVINCIBILITY) || CharacterCompositor().IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING))
    {
        AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
        rCalc.GetAttack().SetPower(0);
    }
    else
    {
        if ((AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)
            && (AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK)
            && (AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT)
            && TestFlag(ENEMYTYPES::FLAG_NOREACTION))
        {
            AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION;
        };

        bool fSetCompleted = false;
        do
        {
            switch (AttackResult)
            {
            case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
                {                
                    if (!SetStatus(ENEMYTYPES::STATUS_KNOCK))
                    {
                        ENEMYTYPES::STATUS eEnemyStatus = ENEMYTYPES::STATUS_HIDE;
                        if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
                        {
                            eEnemyStatus = ENEMYTYPES::STATUS_KNOCK_FRONT;
                        }
                        else if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
                        {
                            eEnemyStatus = ENEMYTYPES::STATUS_KNOCK_BACK;
                        };

                        if (!SetStatus(eEnemyStatus))
                        {
                            if (!SetStatus(ENEMYTYPES::STATUS_IDLE))
                                ASSERT(false);
                        };
                    };

                    fSetCompleted = true;
                    rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                }
                break;
                
            case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY:
                {
                    if (GetStatus() == ENEMYTYPES::STATUS_THROWN_BACK)
                    {
                        RwV3d vecVel = Math::VECTOR3_ZERO;                        
                        CharacterCompositor().GetVelocity(&vecVel);
                        vecVel.y = 0.0f;                        
                        float fSpeed = Math::Vec3_Length(&vecVel);
                        vecVel.z = Math::FNegate(fSpeed);
                        
                        RwV3d vecDir = Math::VECTOR3_ZERO;
                        RwMatrix matRotY;
                        RwMatrixSetIdentityMacro(&matRotY);
                        Math::Matrix_RotateY(&matRotY, CharacterCompositor().GetDirection() + Math::PI);
                        RwV3dTransformVector(&vecVel, &vecVel, &matRotY);
                        RwV3dTransformVector(&vecDir, &vecDir, &matRotY);

                        CharacterCompositor().AttackParameter().m_vVelocity = vecVel;
                        CharacterCompositor().AttackParameter().m_vDirection = vecDir;
                    };

                    ENEMYTYPES::STATUS eEnemyStatus = ENEMYTYPES::STATUS_HIDE;
                    if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
                    {
                        eEnemyStatus = ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT;
                    }
                    else if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
                    {
                        eEnemyStatus = ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK;
                    };

                    if (SetStatus(eEnemyStatus))
                    {
                        fSetCompleted = true;
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                    }
                    else
                    {
                        AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
                    };
                }
                break;
                
            case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION:
                {
                    fSetCompleted = true;
                }
                break;
                
            case CHARACTERTYPES::ATTACKRESULTTYPE_THROW:
                {
                    switch (rCalc.DirectionType())
                    {
                    case CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT:
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_THROWFRONT);
                        break;
                        
                    case CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK:
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_THROWBACK);
                        break;

                    default:
                        ASSERT(false);
                        break;
                    };

                    std::strcpy(CharacterCompositor().LiftInfo().m_szLiftObjectName, rCalc.GetAttacker()->GetName());
                    
                    fSetCompleted = true;
                }
                break;
                
            case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
            case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
            case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
            case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
            case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
                {
                    ENEMYTYPES::STATUS eEnemyStatus = ENEMYTYPES::STATUS_HIDE;                    
                    switch (AttackResult)
                    {
                    case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
                        eEnemyStatus = ENEMYTYPES::STATUS_DINDLE;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
                        eEnemyStatus = ENEMYTYPES::STATUS_STUN;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
                        eEnemyStatus = ENEMYTYPES::STATUS_SLEEP;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
                        eEnemyStatus = ENEMYTYPES::STATUS_FREEZE;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
                        eEnemyStatus = ENEMYTYPES::STATUS_BIND;
                        break;
                    };

                    if (SetStatus(eEnemyStatus))
                    {
                        fSetCompleted = true;
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                    }
                    else
                    {
                        AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
                    };
                }
                break;

            case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD:
                {
                    ASSERT(GetStatus() == ENEMYTYPES::STATUS_GUARD);
                    fSetCompleted = true;
                    rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_GUARD);
                }
                break;

            case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT:
                {
                    fSetCompleted = true;
                    rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_GUARDIMPACT);
                }
                break;

            case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK:
                {
                    if (SetStatus(ENEMYTYPES::STATUS_COUNTERACT))
                        fSetCompleted = true;
                    else
                        AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
                }
                break;

            default:
                {
                    fSetCompleted = true;
                    AttackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
                }
                break;
            };
        } while (!fSetCompleted);
    };

    return AttackResult;
};


void CEnemyCharacter::OnMessageReceivedDamage(int32 iAmount)
{
    if (CharacterParameter().m_feature.m_iHP <= iAmount)
    {
        CharacterParameter().m_feature.m_iHP = 0;
        if (!TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
        {
            SetFlag(ENEMYTYPES::FLAG_DEATH_STATUS);

            if (IsAttachedStatusObserver(ENEMYTYPES::STATUS_GETUP))
            {
                switch (GetStatus())
                {
                case ENEMYTYPES::STATUS_KNOCK_BACK:
                    CGameSound::PlayDeathSE(&CharacterCompositor());
                    SetStatus(ENEMYTYPES::STATUS_FLYAWAY_BACK);
                    break;

                case ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT:
                case ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK:
                case ENEMYTYPES::STATUS_CRASHWALL_FRONT:
                case ENEMYTYPES::STATUS_CRASHWALL_BACK:
                case ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT:
                case ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK:
                case ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT:
                case ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK:
                    CGameSound::PlayDeathSE(&CharacterCompositor());
                    break;

                default:
                    CGameSound::PlayDeathSE(&CharacterCompositor());
                    SetStatus(ENEMYTYPES::STATUS_FLYAWAY_FRONT);
                    break;
                }
            }
            else if (!SetStatus(ENEMYTYPES::STATUS_DEATH))
            {
                CGameSound::PlayDeathSE(&CharacterCompositor());
                SetStatus(ENEMYTYPES::STATUS_QUIT);
            };
        };
    }
    else
    {
        CharacterParameter().m_feature.m_iHP -= iAmount;
    };

    CEnemy* pOwner = (CEnemy*)CGameObjectManager::GetObject(m_hOwner);
    CGameEvent::SetEnemyDamaged(pOwner, CharacterParameter().m_feature.m_iHP);
};


void CEnemyCharacter::OnSteppedDeathFloor(void)
{
    SetStatus(ENEMYTYPES::STATUS_QUIT);
};


void CEnemyCharacter::OnMessageCatch(void* pParam)
{
    CHitCatchData::RESULT* pCatchResult = (CHitCatchData::RESULT*)pParam;
    ASSERT(pCatchResult);
    switch (*pCatchResult)
    {
    case CHitCatchData::RESULT_THROWFRONT:
        {
            if (!SetStatus(ENEMYTYPES::STATUS_CAUGHT))
                ASSERT(false);
        }
        break;

    case CHitCatchData::RESULT_THROWBACK:
        {
            if (!SetStatus(ENEMYTYPES::STATUS_THROWN_BACK))
                ASSERT(false);

            CCharacterCompositor& Compositor = CharacterCompositor();
            CGameObject* pObj = CGameObjectManager::GetObject(Compositor.LiftInfo().m_szLiftObjectName);
            if (pObj)
            {
                if (pObj->GetType() == GAMEOBJECTTYPE::CHARACTER)
                {
                    CCharacter* pCharacter = (CCharacter*)pObj;

                    RwV3d vec = { 0.0f, 0.0f, 1.0f };
                    pCharacter->RotateVectorByDirection(&vec, &vec);

                    RwV3d posobj = Math::VECTOR3_ZERO;
                    pCharacter->GetPosition(&posobj);

                    RwV3d posme = Math::VECTOR3_ZERO;
                    Compositor.GetPosition(&posme);

                    Math::Vec3_Add(&vec, &vec, &posobj);
                    Math::Vec3_Sub(&vec, &vec, &posme);

                    Compositor.SetVelocity(&vec);
                    Compositor.SetDirection(pCharacter->GetDirection());
                };
            };
        }
        break;
    };
};


void CEnemyCharacter::OnMessageLift(void* pParam)
{
    CCharacter::MSG_LIFT_INFO* pMsgLiftInfo = (CCharacter::MSG_LIFT_INFO*)pParam;

    CharacterCompositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, true);
    CharacterCompositor().SetDirection(pMsgLiftInfo->m_fDirection);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    CharacterCompositor().GetPosition(&vPosition);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.x = (pMsgLiftInfo->m_vPosition.x - vPosition.x) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.y = (pMsgLiftInfo->m_vPosition.y - vPosition.y) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.z = (pMsgLiftInfo->m_vPosition.z - vPosition.z) * (1.0f / CGameProperty::GetElapsedTime());

    CharacterCompositor().SetVelocity(&vVelocity);

    switch (pMsgLiftInfo->m_iStatus)
    {
    case PLAYERTYPES::STATUS_LIFT:
        {
            if (GetStatus() != ENEMYTYPES::STATUS_LIFTED)
            {
                if (!SetStatus(ENEMYTYPES::STATUS_LIFTED))
                    ASSERT(false);
            };
        }
        break;

    case PLAYERTYPES::STATUS_LIFT_WALK:
        {
            if (GetStatus() != ENEMYTYPES::STATUS_LIFTED_WALK)
            {
                if (!SetStatus(ENEMYTYPES::STATUS_LIFTED_WALK))
                    ASSERT(false);
            };
        }
        break;
    };
};


void CEnemyCharacter::OnMessageThrow(void* pParam)
{
    RwV3d* pvVelocity = (RwV3d*)pParam;
    ASSERT(pvVelocity);

    CharacterCompositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, false);
    CharacterCompositor().SetVelocity(pvVelocity);
    
    SetStatus(ENEMYTYPES::STATUS_THROWN_FRONT);
};


void CEnemyCharacter::OnMessageMissThrow(void* pParam)
{
    RwV3d* pvVelocity = (RwV3d*)pParam;
    ASSERT(pvVelocity);

    CharacterCompositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, false);
    CharacterCompositor().SetVelocity(pvVelocity);
    CharacterCompositor().RequestDamage(0);
    
    SetStatus(ENEMYTYPES::STATUS_AERIAL);
};


bool CEnemyCharacter::AttachStatusObserver(CStatusObserver* pStatusObserver)
{
    if (m_pStatusSubject)
        return m_pStatusSubject->Attach(pStatusObserver);
    else
        return false;
};


void CEnemyCharacter::DetachStatusObserver(ENEMYTYPES::STATUS eStatus)
{
    if (m_pStatusSubject)
        m_pStatusSubject->Detach(eStatus);
};


bool CEnemyCharacter::IsAttachedStatusObserver(ENEMYTYPES::STATUS eStatus)
{
    if (m_pStatusSubject)
        return m_pStatusSubject->IsContained(eStatus);
    else
        return false;
};


void CEnemyCharacter::SetOwner(uint32 hOwner)
{
    m_hOwner = hOwner;
};


uint32 CEnemyCharacter::GetOwner(void)
{
    return m_hOwner;
};


bool CEnemyCharacter::SetStatus(ENEMYTYPES::STATUS eStatus)
{
    ASSERT((eStatus >= ENEMYTYPES::STATUS_HIDE) && (eStatus < ENEMYTYPES::STATUS_MAX));

    bool bResult = false;
    CGameObject* pObj = CGameObjectManager::GetObject(m_hOwner);
    
    if ((eStatus == ENEMYTYPES::STATUS_QUIT) && pObj)
    {
        ASSERT(pObj->GetType() == GAMEOBJECTTYPE::ENEMY);
        CGameObjectManager::DeleteObject(pObj);
        StatusSubject().OnEnd();
        Stop();

        bResult = true;
    }
    else if(StatusSubject().IsContained(eStatus))
    {
        if (StatusSubject().Status() != eStatus)
        {
            StatusSubject().OnEnd();
            StatusSubject().Status(eStatus);
            StatusSubject().OnStart();
            
            bResult = true;
        };
    };

    return bResult;
};


ENEMYTYPES::STATUS CEnemyCharacter::GetStatus(void)
{
    ASSERT(m_pStatusSubject);
    return m_pStatusSubject->Status();
};


void CEnemyCharacter::SetPatrolArea(const RwV3d* pvPosition, float fRadius)
{
    ASSERT(pvPosition);
    ASSERT(m_pParameter);

    m_pParameter->m_feature.m_vPatrolOrigin = *pvPosition;
    m_pParameter->m_feature.m_fPatrolRadius = fRadius;
};


void CEnemyCharacter::SetPosition(const RwV3d* pvPosition)
{
    CharacterCompositor().SetPosition(pvPosition);
};


void CEnemyCharacter::GetBodyPosition(RwV3d* pvPosition)
{
    CharacterCompositor().GetBodyPosition(pvPosition);
};


void CEnemyCharacter::SetDirection(float fDirection)
{
    CharacterCompositor().SetDirection(fDirection);
};


void CEnemyCharacter::SetSharedData(void* pSharedData)
{
    if (m_pStatusSubject)
        m_pStatusSubject->SetSharedData(pSharedData);
};


CEnemyCharacter::CHRTYPE CEnemyCharacter::GetAttackCharacterType(void)
{
    CCharacter::TYPE Type = CharacterCompositor().GetCharacterType();
    switch (Type)
    {
    case CCharacter::TYPE_ENEMY:
        return CHRTYPE_ENEMY;

    case CCharacter::TYPE_PLAYER:
        return CHRTYPE_PLAYER;

    default:
        ASSERT(false);
        return CHRTYPE(-1);
    };
};


ENEMYID::VALUE CEnemyCharacter::GetID(void)
{
    return m_ID;
};


const char* CEnemyCharacter::GetName(void)
{
    return CharacterCompositor().GetName();
};


CHARACTERTYPES::DEFENCERSTATUSFLAG CEnemyCharacter::CheckDefenceStatusFlag(void)
{
    uint32 Flag = 0;

    if (TestFlag(ENEMYTYPES::FLAG_AERIAL_STATUS))
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_AERIAL);

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_SUPERTOUGH))
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP);
    
    switch (GetStatus())
    {
    case ENEMYTYPES::STATUS_DINDLE:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DINDLE);
        break;

    case ENEMYTYPES::STATUS_STUN:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_STUN);
        break;

    case ENEMYTYPES::STATUS_COUNTERACT:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_STAGGER);
        break;

    case ENEMYTYPES::STATUS_SLEEP:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_SLEEP);
        break;

    case ENEMYTYPES::STATUS_FREEZE:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_FREEZE);
        break;

    case ENEMYTYPES::STATUS_BIND:
        FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_BIND);
        break;

    case ENEMYTYPES::STATUS_GUARD:
        if (TestFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE))
            FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD_ALLRANGE);
        else
            FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD);
        break;
    };
    
    switch (GetStatus())
    {
    case ENEMYTYPES::STATUS_THINKING:
    case ENEMYTYPES::STATUS_IDLE:
    case ENEMYTYPES::STATUS_TURN:
    case ENEMYTYPES::STATUS_WALK:
    case ENEMYTYPES::STATUS_WALK_RIGHT:
    case ENEMYTYPES::STATUS_WALK_LEFT:
    case ENEMYTYPES::STATUS_WALK_BACK:
    case ENEMYTYPES::STATUS_RUN:
    case ENEMYTYPES::STATUS_ATTACK_A:
    case ENEMYTYPES::STATUS_ATTACK_AA:
    case ENEMYTYPES::STATUS_ATTACK_AAA:
    case ENEMYTYPES::STATUS_ATTACK_B:
    case ENEMYTYPES::STATUS_ATTACK_C:
    case ENEMYTYPES::STATUS_ATTACK_RUN:
    case ENEMYTYPES::STATUS_FIRE_A:
    case ENEMYTYPES::STATUS_FIRE_B:
    case ENEMYTYPES::STATUS_FIRE_C:
    case ENEMYTYPES::STATUS_FIRE_AA:
    case ENEMYTYPES::STATUS_FIRE_BB:
    case ENEMYTYPES::STATUS_FIRE_CC:
    case ENEMYTYPES::STATUS_KNOCK:
    case ENEMYTYPES::STATUS_KNOCK_FRONT:
    case ENEMYTYPES::STATUS_KNOCK_BACK:
    case ENEMYTYPES::STATUS_DINDLE:
    case ENEMYTYPES::STATUS_STUN:
    case ENEMYTYPES::STATUS_COUNTERACT:
    case ENEMYTYPES::STATUS_SLEEP:
    case ENEMYTYPES::STATUS_GUARD:
        {
            if (!StatusSubject().IsContained(ENEMYTYPES::STATUS_CAUGHT))
            {
                FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW);
            }
            else
            {
                ASSERT(StatusSubject().IsContained(ENEMYTYPES::STATUS_LIFTED));
                ASSERT(StatusSubject().IsContained(ENEMYTYPES::STATUS_LIFTED_WALK));
                ASSERT(StatusSubject().IsContained(ENEMYTYPES::STATUS_THROWN_FRONT));
                ASSERT(StatusSubject().IsContained(ENEMYTYPES::STATUS_THROWN_BACK));
            };
        }
        break;

    default:
        {
            FLAG_SET(Flag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW);
        }
        break;
    };
    
    return CHARACTERTYPES::DEFENCERSTATUSFLAG(Flag);
};


void CEnemyCharacter::Start(void)
{
    if (!m_bRunning)
    {
        m_bRunning = true;

        CCharacterCompositor* pCharacter = (CCharacterCompositor*)CGameObjectManager::GetObject(m_hCharacter);
        CGameObjectManager::SendMessage(pCharacter, GAMEOBJECTTYPES::MESSAGEID_AWAKE);

        StartAI();

        OnStart();
    };
};


void CEnemyCharacter::Stop(void)
{
    if (m_bRunning)
    {
        OnStop();

        StopAI();

        CCharacterCompositor* pCharacter = (CCharacterCompositor*)CGameObjectManager::GetObject(m_hCharacter);
        CGameObjectManager::SendMessage(pCharacter, GAMEOBJECTTYPES::MESSAGEID_SLEEP);

        m_bRunning = false;
    };
};


void CEnemyCharacter::StartAI(void)
{
    if (m_bRunning && (!m_bRunningAI))
    {
        m_bRunningAI = true;

        AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_NOTHING);
        AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
    };
};


void CEnemyCharacter::StopAI(void)
{
    if (m_bRunningAI)
    {
        m_bRunningAI = false;

        AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

        if (m_bRunning)
        {
            if (!TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
            {
                if (GetStatus() > ENEMYTYPES::STATUS_DEATH)
                    SetStatus(ENEMYTYPES::STATUS_IDLE);
            };
        };
    };
};


void CEnemyCharacter::ClearFlag(ENEMYTYPES::FLAG flag)
{
    uint32 eflag = m_eflag;
    FLAG_CLEAR(eflag, flag);
    m_eflag = ENEMYTYPES::FLAG(eflag);
};


bool CEnemyCharacter::TestFlag(ENEMYTYPES::FLAG flag) const
{
    return FLAG_TEST(m_eflag, flag);
};


void CEnemyCharacter::SetFlag(ENEMYTYPES::FLAG flag)
{
    uint32 eflag = m_eflag;
    FLAG_SET(eflag, flag);
    m_eflag = ENEMYTYPES::FLAG(eflag);
};


void* CEnemyCharacter::SharedData(void) const
{
    return StatusSubject().GetSharedData();
};


CAIModerator& CEnemyCharacter::AIModerator(void) const
{
    ASSERT(m_pAIModerator);
    return *m_pAIModerator;
};


CAIThinkOrder& CEnemyCharacter::AIThinkOrder(void) const
{    
    return (CAIThinkOrder&)m_AIThinkOrder;
};


ENEMYTYPES::CHARACTERISTIC& CEnemyCharacter::AICharacteristic(void) const
{
    return CharacterParameter().m_AICharacteristic;
};


CEnemyCharacter::CStatusSubject& CEnemyCharacter::StatusSubject(void) const
{
    ASSERT(m_pStatusSubject);
    return *m_pStatusSubject;
};


CEnemyCharacter::PARAMETER& CEnemyCharacter::CharacterParameter(void) const
{
    ASSERT(m_pParameter);
    return *m_pParameter;
};


uint8* CEnemyCharacter::FrequencyParameter(void) const
{
    ASSERT(m_puFrequencyParam);
    return m_puFrequencyParam;
};


CCharacterCompositor& CEnemyCharacter::CharacterCompositor(void) const
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hCharacter);
    ASSERT(pObj);
    return *(CCharacterCompositor*)pObj;
};