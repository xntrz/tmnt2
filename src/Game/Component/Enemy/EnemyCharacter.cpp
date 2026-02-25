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


CEnemyCharacter::CStatusSubject::CStatusSubject(CEnemyCharacter* pSubject)
: m_apStatusObservers()
, m_eStatusPrev(ENEMYTYPES::STATUS_HIDE)
, m_eStatus(ENEMYTYPES::STATUS_HIDE)
, m_pSharedData(nullptr)
, m_pEnemyChr(pSubject)
{
    m_apStatusObservers.resize(ENEMYTYPES::STATUS_MAX);        
};


CEnemyCharacter::CStatusSubject::~CStatusSubject(void)
{
    int32 count = static_cast<int32>(m_apStatusObservers.size());

    for (int32 i = 0; i < count; ++i)
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


bool CEnemyCharacter::CStatusSubject::Attach(ENEMYTYPES::STATUS status, CStatusObserver* pStatusObserver)
{
    ASSERT(status >= 0);
    ASSERT(status < static_cast<int32>(m_apStatusObservers.size()));
    ASSERT(pStatusObserver);

    if (m_apStatusObservers[status] != pStatusObserver)
    {
        if (m_apStatusObservers[status])
            Detach(status);

        m_apStatusObservers[status] = pStatusObserver;

        m_apStatusObservers[status]->m_pEnemyChr   = m_pEnemyChr;
        m_apStatusObservers[status]->m_pSharedData = m_pEnemyChr->StatusSubject().GetSharedData();
        m_apStatusObservers[status]->m_eOwnStatus  = status;

        m_apStatusObservers[status]->Append();

        return true;
    };

    return false;
};


void CEnemyCharacter::CStatusSubject::Detach(ENEMYTYPES::STATUS eStatus)
{
    ASSERT(eStatus >= 0);
    ASSERT(eStatus < static_cast<int32>(m_apStatusObservers.size()));

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


ENEMYTYPES::STATUS CEnemyCharacter::CStatusSubject::OnEnd(void)
{
    ASSERT(m_apStatusObservers[m_eStatus]);
    return m_apStatusObservers[m_eStatus]->OnEnd();
};


CEnemyCharacter::CStatusObserver::RESULT CEnemyCharacter::CStatusSubject::Observing(void)
{
    ASSERT(m_apStatusObservers[m_eStatus]);
    return m_apStatusObservers[m_eStatus]->Observing();
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


//
// *********************************************************************************
//


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
, m_hOwner(0)
, m_AIThinkOrder()
, m_pParameter(nullptr)
, m_eflag(ENEMYTYPES::FLAG_DEFAULT)
, m_puFrequencyParam(nullptr)
, m_bRunning(false)
, m_bRunningAI(false)
{
    char szName[GAMEOBJECTTYPES::NAME_MAX];
    szName[0] = '\0';

    std::sprintf(szName, "%s_%04d", ENEMYID::GetExtName(m_ID), CEnemy::m_iUniqueCount);
    ASSERT(std::strlen(szName) < GAMEOBJECTTYPES::NAME_MAX);

    CCharacterCompositor* pChrCompositor = new CCharacterCompositor(szName, CCharacter::TYPE_ENEMY, *this);

    m_hCharacter = pChrCompositor->GetHandle();
    CGameObjectManager::SendMessage(pChrCompositor, GAMEOBJECTTYPES::MESSAGEID_SLEEP);

    m_pStatusSubject = new CStatusSubject(this);

    m_pParameter = new PARAMETER;
    std::memset(m_pParameter, 0x00, sizeof(*m_pParameter));
    m_pParameter->m_feature.m_iPattern = m_createinfo.m_iPattern;
};


CEnemyCharacter::~CEnemyCharacter(void)
{
    Delete();

    CGameObject* pObj = CGameObjectManager::GetObject(m_hCharacter);
    if (pObj)
        delete pObj;
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

        const ENEMYTYPES::CHARACTERISTIC& AICharacteristic = CEnemyParameter::GetAICharacteristic(idx);
        std::memcpy(&m_pParameter->m_AICharacteristic, &AICharacteristic, sizeof(m_pParameter->m_AICharacteristic));

        int32 iFreqMax = CEnemyParameter::GetFrequencyMax(idx);
        if (iFreqMax > 0)
        {
            m_puFrequencyParam = new uint8[GAMETYPES::DIFFICULTY_NUM * iFreqMax];
            std::memset(m_puFrequencyParam, 0x00, (GAMETYPES::DIFFICULTY_NUM * iFreqMax));

            GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
            uint8* puFreqParamNode = &m_puFrequencyParam[difficulty * iFreqMax];

            for (int32 i = 0; i < iFreqMax; ++i)
                puFreqParamNode[i] = CEnemyParameter::GetFrequency(idx, i);
        };
    }
    else
    {
        int32 iFreqMax = m_pParameter->m_iFrequencyMax;
        if (iFreqMax > 0)
        {
            m_puFrequencyParam = new uint8[GAMETYPES::DIFFICULTY_NUM * iFreqMax];
            std::memset(m_puFrequencyParam, 0x00, (GAMETYPES::DIFFICULTY_NUM * iFreqMax));

            for (int32 i = 0, j = 0; i < iFreqMax; ++i, j += 3)
            {
                static_assert(GAMETYPES::DIFFICULTY_NUM == 3, "add node init");
                
                m_puFrequencyParam[(GAMETYPES::DIFFICULTY_EASY   * iFreqMax) + i] = pParameter->m_puFrequencyParam[j + 0];
                m_puFrequencyParam[(GAMETYPES::DIFFICULTY_NORMAL * iFreqMax) + i] = pParameter->m_puFrequencyParam[j + 1];
                m_puFrequencyParam[(GAMETYPES::DIFFICULTY_HARD   * iFreqMax) + i] = pParameter->m_puFrequencyParam[j + 2];
            };
        };
    };

#ifdef _DEBUG
    /* for enemy tests */
    if (m_createinfo.m_iHPMax != 0)
        m_pParameter->m_feature.m_iHPMax = m_createinfo.m_iHPMax;
#endif /* _DEBUG */

    m_pParameter->m_feature.m_iHP           = m_pParameter->m_feature.m_iHPMax;
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

    CCharacter::PARAMETER chrParameter;
    std::memset(&chrParameter, 0x00, sizeof(chrParameter));
    chrParameter.m_pszModelName = ENEMYID::GetExtName(m_ID);
    chrParameter.m_pszMotionSetName = ENEMYID::GetExtName(m_ID);
    chrParameter.m_bToon = m_pParameter->m_bToon;

    Compositor().Initialize(&chrParameter);
    Compositor().ModuleShadow(m_pParameter->m_fShadowRadius);
    Compositor().ModuleRipple(m_pParameter->m_fShadowRadius);
    Compositor().SetPosition(&m_createinfo.m_vPosition);
    Compositor().SetDirection(m_createinfo.m_fDirection);

    if (m_pParameter->m_pfnAIInstance)
    {
        m_pAIModerator = m_pParameter->m_pfnAIInstance(this);
        ASSERT(m_pAIModerator);
    };

    bool bResult = SetStatus(m_createinfo.m_status);
	ASSERT(bResult, "cant set default status %" PRId32, m_createinfo.m_status);

	return bResult;
};


void CEnemyCharacter::Run(void)
{
    if (!m_bRunning)
        return;

    if (m_pAIModerator && m_bRunningAI)
        m_pAIModerator->Run();

    if (StatusSubject().Observing() != CStatusObserver::RESULT_END)
    {
        StatusSubject().Update();
        return;
    };

    ENEMYTYPES::STATUS nextStatus = StatusSubject().OnEnd();
    if (nextStatus != ENEMYTYPES::STATUS_QUIT)
    {
        if ((nextStatus == ENEMYTYPES::STATUS_GETUP) &&
            TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS) &&
            (GetStatus() != ENEMYTYPES::STATUS_QUIT) &&
            (GetStatus() != ENEMYTYPES::STATUS_DEATH))
        {
            nextStatus = ENEMYTYPES::STATUS_DEATH;
        };

        StatusSubject().Status(nextStatus);
        StatusSubject().OnStart();
        StatusSubject().Update();
        return;
    };

    CGameObject* pObj = CGameObjectManager::GetObject(m_hOwner);
    if (pObj)
    {
        ASSERT(pObj->GetType() == GAMEOBJECTTYPE::ENEMY);

        CGameObjectManager::DeleteObject(pObj);
        Stop();
    };
};


void CEnemyCharacter::Draw(void)
{
#ifdef _DEBUG
    if (m_pAIModerator)
        m_pAIModerator->Draw();
#endif /* _DEBUG */
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
            
            int32 damageRequested = Compositor().GetRequestedDamage();
            if (damageRequested > 0)
            {
                CGameObjectManager::SendMessage(&Compositor(),
                                                CHARACTERTYPES::MESSAGEID_RECVDMG,
                                                reinterpret_cast<void*>(damageRequested));
                Compositor().RequestDamage(0);
            };
        }
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK:
        {
            SetStatus(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK);

            CGameObjectManager::SendMessage(&Compositor(),
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(20));
        }
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT:
        {
            SetStatus(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT);

            CGameObjectManager::SendMessage(&Compositor(),
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(20));
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
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceFlag = CheckDefenceStatusFlag();    
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = rCalc.CalcAtackParameter(Compositor().AttackParameter(), defenceFlag);

    if (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
    {
        attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    }
    else if (TestFlag(ENEMYTYPES::FLAG_INVINCIBILITY) ||
             Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING))
    {
        attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
        rCalc.GetAttack().SetPower(0);
    }
    else
    {
        if ((attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD)        &&
            (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK)  &&
            (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT) &&
            TestFlag(ENEMYTYPES::FLAG_NOREACTION))
        {
            attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION;
        };

        bool fSetCompleted = false;
        do
        {
            switch (attackResult)
            {
            case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
                {                
                    if (!SetStatus(ENEMYTYPES::STATUS_KNOCK))
                    {
                        ENEMYTYPES::STATUS status = ENEMYTYPES::STATUS_HIDE;

                        if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
                            status = ENEMYTYPES::STATUS_KNOCK_FRONT;
                        else if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
                            status = ENEMYTYPES::STATUS_KNOCK_BACK;

                        if (!SetStatus(status))
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
                        RwV3d vecVel = Compositor().AttackParameter().m_vVelocity;
                        vecVel.y = 0.0f;

                        float fVel = Math::Vec3_Length(&vecVel);

                        vecVel.x = 0.0f;
                        vecVel.y = Compositor().AttackParameter().m_vVelocity.y;
                        vecVel.z = std::fabs(fVel);
                
                        RwMatrix matRotY;
                        RwMatrixSetIdentity(&matRotY);                        
                        Math::Matrix_RotateY(&matRotY, Compositor().GetDirection() + MATH_PI);
                        RwV3dTransformVector(&vecVel, &vecVel, &matRotY);

                        Compositor().AttackParameter().m_vVelocity = vecVel;

                        RwV3d vecDir = Math::VECTOR3_AXIS_Z;
                        RwV3dTransformVector(&vecDir, &vecDir, &matRotY);

                        Compositor().AttackParameter().m_vDirection = vecDir;
                    };

                    ENEMYTYPES::STATUS status = ENEMYTYPES::STATUS_HIDE;

                    if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
                        status = ENEMYTYPES::STATUS_FLYAWAY_FRONT;
                    else if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
                        status = ENEMYTYPES::STATUS_FLYAWAY_BACK;

                    if (SetStatus(status))
                    {
                        fSetCompleted = true;
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                    }
                    else
                    {
                        attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
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
                    if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_THROWFRONT);
                    else if (rCalc.DirectionType() == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_THROWBACK);

                    std::strcpy(Compositor().LiftInfo().m_szLiftObjectName, rCalc.GetAttacker()->GetName());
                    
                    fSetCompleted = true;
                }
                break;
                
            case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
            case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
            case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
            case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
            case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
                {
                    ENEMYTYPES::STATUS status = ENEMYTYPES::STATUS_HIDE;                    
                    switch (attackResult)
                    {
                    case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
                        status = ENEMYTYPES::STATUS_DINDLE;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
                        status = ENEMYTYPES::STATUS_STUN;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
                        status = ENEMYTYPES::STATUS_SLEEP;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
                        status = ENEMYTYPES::STATUS_FREEZE;
                        break;
                        
                    case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
                        status = ENEMYTYPES::STATUS_BIND;
                        break;

                    default:
                        break;
                    };

                    if (SetStatus(status))
                    {
                        fSetCompleted = true;
                        rCalc.GetAttack().GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                    }
                    else
                    {
                        attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
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
                        attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
                }
                break;

            default:
                {
                    fSetCompleted = true;
                    attackResult = CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
                }
                break;
            };
        } while (!fSetCompleted);
    };

    return attackResult;
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
                    CGameSound::PlayDeathSE(&Compositor());
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
                    CGameSound::PlayDeathSE(&Compositor());
                    break;

                default:
                    CGameSound::PlayDeathSE(&Compositor());
                    SetStatus(ENEMYTYPES::STATUS_FLYAWAY_FRONT);
                    break;
                };
            }
            else if (!SetStatus(ENEMYTYPES::STATUS_DEATH))
            {
                CGameSound::PlayDeathSE(&Compositor());
                SetStatus(ENEMYTYPES::STATUS_QUIT);
            };
        };
    }
    else
    {
        CharacterParameter().m_feature.m_iHP -= iAmount;
    };

    CEnemy* pOwner = static_cast<CEnemy*>(CGameObjectManager::GetObject(m_hOwner));
    CGameEvent::SetEnemyDamaged(pOwner, CharacterParameter().m_feature.m_iHP);
};


void CEnemyCharacter::OnSteppedDeathFloor(void)
{
    SetStatus(ENEMYTYPES::STATUS_QUIT);
};


void CEnemyCharacter::OnMessageCatch(void* pParam)
{
    CHitCatchData::RESULT* pCatchResult = static_cast<CHitCatchData::RESULT*>(pParam);
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

            CGameObject* pObj = CGameObjectManager::GetObject(Compositor().LiftInfo().m_szLiftObjectName);
            if (pObj && (pObj->GetType() == GAMEOBJECTTYPE::CHARACTER))
            {
                CCharacter* pCharacter = static_cast<CCharacter*>(pObj);

                RwV3d vecVelocity = Math::VECTOR3_AXIS_Z;
                pCharacter->RotateVectorByDirection(&vecVelocity, &vecVelocity);

                RwV3d vecPosChr = Math::VECTOR3_ZERO;
                pCharacter->GetPosition(&vecPosChr);

                RwV3d vecPosMe = Math::VECTOR3_ZERO;
                Compositor().GetPosition(&vecPosMe);

                Math::Vec3_Add(&vecVelocity, &vecVelocity, &vecPosChr);
                Math::Vec3_Sub(&vecVelocity, &vecVelocity, &vecPosMe);

                Compositor().SetVelocity(&vecVelocity);
                Compositor().SetDirection(pCharacter->GetDirection());
            };
        }
        break;

    default:
        break;
    };
};


void CEnemyCharacter::OnMessageLift(void* pParam)
{
    CCharacter::MSG_LIFT_INFO* pMsgLiftInfo = static_cast<CCharacter::MSG_LIFT_INFO*>(pParam);

    Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    Compositor().SetDirection(pMsgLiftInfo->m_fDirection);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vPosition);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.x = (pMsgLiftInfo->m_vPosition.x - vPosition.x) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.y = (pMsgLiftInfo->m_vPosition.y - vPosition.y) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.z = (pMsgLiftInfo->m_vPosition.z - vPosition.z) * (1.0f / CGameProperty::GetElapsedTime());

    Compositor().SetVelocity(&vVelocity);

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

    default:
        break;
    };
};


void CEnemyCharacter::OnMessageThrow(void* pParam)
{
    RwV3d* pvVelocity = static_cast<RwV3d*>(pParam);
    ASSERT(pvVelocity);

    Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    Compositor().SetVelocity(pvVelocity);
    
    SetStatus(ENEMYTYPES::STATUS_THROWN_FRONT);
};


void CEnemyCharacter::OnMessageMissThrow(void* pParam)
{
    RwV3d* pvVelocity = static_cast<RwV3d*>(pParam);
    ASSERT(pvVelocity);

    Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    Compositor().SetVelocity(pvVelocity);
    Compositor().RequestDamage(0);
    
    SetStatus(ENEMYTYPES::STATUS_AERIAL);
};


bool CEnemyCharacter::AttachStatusObserver(ENEMYTYPES::STATUS status, CStatusObserver* pStatusObserver)
{
    if (m_pStatusSubject)
        return m_pStatusSubject->Attach(status, pStatusObserver);
    
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
    ASSERT(eStatus >= ENEMYTYPES::STATUS_HIDE);
    ASSERT(eStatus <  ENEMYTYPES::STATUS_MAX);

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
    else if (StatusSubject().IsContained(eStatus))
    {
        ASSERT(StatusSubject().Status() != ENEMYTYPES::STATUS_QUIT);

        StatusSubject().OnEnd();
        StatusSubject().Status(eStatus);
        StatusSubject().OnStart();

        bResult = true;
    };

    return bResult;
};


ENEMYTYPES::STATUS CEnemyCharacter::GetStatus(void) const
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
    Compositor().SetPosition(pvPosition);
};


void CEnemyCharacter::GetBodyPosition(RwV3d* pvPosition) const
{
    Compositor().GetBodyPosition(pvPosition);
};


void CEnemyCharacter::SetDirection(float fDirection)
{
    Compositor().SetDirection(fDirection);
};


void CEnemyCharacter::SetSharedData(void* pSharedData)
{
    if (m_pStatusSubject)
        m_pStatusSubject->SetSharedData(pSharedData);
};


CEnemyCharacter::CHRTYPE CEnemyCharacter::GetAttackCharacterType(void)
{
    CCharacter::TYPE Type = Compositor().GetCharacterType();
    switch (Type)
    {
    case CCharacter::TYPE_ENEMY:
        return CHRTYPE_ENEMY;

    case CCharacter::TYPE_PLAYER:
        return CHRTYPE_PLAYER;

    default:
        ASSERT(false);
        return CHRTYPE_ENEMY;
    };
};


ENEMYID::VALUE CEnemyCharacter::GetID(void) const
{
    return m_ID;
};


const char* CEnemyCharacter::GetName(void) const
{
    return Compositor().GetName();
};


CHARACTERTYPES::DEFENCERSTATUSFLAG CEnemyCharacter::CheckDefenceStatusFlag(void)
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceFlag = CHARACTERTYPES::DEFENCERSTATUSFLAG_NONE;

    if (TestFlag(ENEMYTYPES::FLAG_AERIAL_STATUS))
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_AERIAL;

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_SUPERTOUGH))
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP;
    
    switch (GetStatus())
    {
    case ENEMYTYPES::STATUS_DINDLE:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DINDLE;
        break;

    case ENEMYTYPES::STATUS_STUN:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_STUN;
        break;

    case ENEMYTYPES::STATUS_COUNTERACT:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_STAGGER;
        break;

    case ENEMYTYPES::STATUS_SLEEP:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_SLEEP;
        break;

    case ENEMYTYPES::STATUS_FREEZE:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_FREEZE;
        break;

    case ENEMYTYPES::STATUS_BIND:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_BIND;
        break;

    case ENEMYTYPES::STATUS_GUARD:
        if (TestFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE))
            defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD_ALLRANGE;
        else
            defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD;
        break;

    default:
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
                defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW;
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
            defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW;
        }
        break;
    };
    
    return defenceFlag;
};


void CEnemyCharacter::Start(void)
{
    if (!m_bRunning)
    {
        m_bRunning = true;

        CCharacterCompositor* pCharacter = static_cast<CCharacterCompositor*>(CGameObjectManager::GetObject(m_hCharacter));
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

        CCharacterCompositor* pCharacter = static_cast<CCharacterCompositor*>(CGameObjectManager::GetObject(m_hCharacter));
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


void CEnemyCharacter::SetFlag(ENEMYTYPES::FLAG flag)
{
    m_eflag |= flag;
};


void CEnemyCharacter::ClearFlag(ENEMYTYPES::FLAG flag)
{
    m_eflag &= ~(flag);
};


bool CEnemyCharacter::TestFlag(ENEMYTYPES::FLAG flag) const
{
    return ((m_eflag & flag) == flag);
};


void* CEnemyCharacter::SharedData(void) const
{
    return StatusSubject().GetSharedData();
};


bool CEnemyCharacter::IsAIModeratorExist(void) const
{
    return (m_pAIModerator != nullptr);
};


CAIModerator& CEnemyCharacter::AIModerator(void)
{
    ASSERT(m_pAIModerator);
    return *m_pAIModerator;
};


const CAIModerator& CEnemyCharacter::AIModerator(void) const
{
    ASSERT(m_pAIModerator);
    return *m_pAIModerator;
};


CAIThinkOrder& CEnemyCharacter::AIThinkOrder(void) const
{    
    return m_AIThinkOrder;
};


CEnemyCharacter::CStatusSubject& CEnemyCharacter::StatusSubject(void) const
{
    ASSERT(m_pStatusSubject);
    return *m_pStatusSubject;
};


const ENEMYTYPES::CHARACTERISTIC& CEnemyCharacter::AICharacteristic(void) const
{
    return CharacterParameter().m_AICharacteristic;
};


CEnemyCharacter::PARAMETER& CEnemyCharacter::CharacterParameter(void)
{
    ASSERT(m_pParameter);
    return *m_pParameter;
};


const CEnemyCharacter::PARAMETER& CEnemyCharacter::CharacterParameter(void) const
{
    ASSERT(m_pParameter);
    return *m_pParameter;
};


uint8* CEnemyCharacter::FrequencyParameter(void) const
{
    ASSERT(m_puFrequencyParam);
    return m_puFrequencyParam;
};


uint8 CEnemyCharacter::FrequencyParameter(int32 idx) const
{
    ASSERT(idx >= 0);
    ASSERT(idx < m_pParameter->m_iFrequencyMax);

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    uint8* puFreqParamNode = &m_puFrequencyParam[difficulty * m_pParameter->m_iFrequencyMax];

    return puFreqParamNode[idx];
};


bool CEnemyCharacter::IsFrequencyParameterValid(void) const
{
    return (m_pParameter->m_iFrequencyMax > 0);
};


CCharacterCompositor& CEnemyCharacter::Compositor(void)
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hCharacter);
    ASSERT(pObj);

    return static_cast<CCharacterCompositor&>(*pObj);
};


const CCharacterCompositor& CEnemyCharacter::Compositor(void) const
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hCharacter);
    ASSERT(pObj);

    return static_cast<CCharacterCompositor&>(*pObj);
};


const ENEMYTYPES::FEATURE& CEnemyCharacter::Feature(void) const
{
    return CharacterParameter().m_feature;
};


bool CEnemyCharacter::IsRunning(void) const
{
    return m_bRunning;
};


bool CEnemyCharacter::IsRunningAI(void) const
{
    return m_bRunningAI;
};


void CEnemyCharacter::SetHPMax(int32 iHPMax)
{
    CharacterParameter().m_feature.m_iHPMax = iHPMax;
    CharacterParameter().m_feature.m_iHP = CharacterParameter().m_feature.m_iHPMax;
};