#include "TransferFloorGimmickMove.hpp"

#include "Game/System/Map/CameraDataMan.hpp"


CTransferFloorGimmickMove::CTransferFloorGimmickMove(void)
: m_fMoveTime(0.0f)
, m_fStopTime(0.0f)
, m_fTimer(0.0f)
, m_phase(PHASE_NONE)
, m_passingway(PASSINGWAY_GO)
, m_type(0)
, m_vVelocity(Math::VECTOR3_ZERO)
{
    ;
};


CTransferFloorGimmickMove::~CTransferFloorGimmickMove(void)
{
    ;
};


CTransferFloorGimmickMove::RESULT CTransferFloorGimmickMove::OnMove(float dt)
{
    m_vPosition.x += (m_vVelocity.x * dt);
    m_vPosition.y += (m_vVelocity.y * dt);
    m_vPosition.z += (m_vVelocity.z * dt);
    
    return RESULT_NONE;
};


void CTransferFloorGimmickMove::StartTransfer(void)
{
    if (m_phase == PHASE_NONE)
        m_phase = PHASE_MOVE;
};


//
// *********************************************************************************
//


CLinearTransferFloorGimmickMove::CLinearTransferFloorGimmickMove(void)
: m_vStartPosition(Math::VECTOR3_ZERO)
, m_vGoalPosition(Math::VECTOR3_ZERO)
{
    ;
};


CLinearTransferFloorGimmickMove::~CLinearTransferFloorGimmickMove(void)
{
    ;
};


CLinearTransferFloorGimmickMove::RESULT CLinearTransferFloorGimmickMove::OnMove(float dt)
{
    switch (m_phase)
    {
    case PHASE_NONE:
        {
            if (m_type != 2)
                m_phase = PHASE_MOVE;
        }
        break;

    case PHASE_STOP:
        {
            m_fTimer += dt;
            if (m_fTimer >= m_fStopTime)
            {
                m_phase = PHASE_MOVE;
                m_fTimer = 0.0f;
            };
        }
        break;

    case PHASE_MOVE:
        {
            RwV3d vMoveVelocity = Math::VECTOR3_ZERO;
            
			m_fTimer += dt;
            if (m_fTimer < m_fMoveTime)
            {
                if (m_passingway == PASSINGWAY_BACK)
                {
                    Math::Vec3_Sub(&vMoveVelocity, &m_vStartPosition, &m_vGoalPosition);
                }
                else
                {
                    Math::Vec3_Sub(&vMoveVelocity, &m_vGoalPosition, &m_vStartPosition);
                };

                float fPerSec = (1.0f / m_fMoveTime);
                Math::Vec3_Scale(&vMoveVelocity, &vMoveVelocity, fPerSec);
            }
            else
            {
                vMoveVelocity = Math::VECTOR3_ZERO;
                m_fTimer = 0.0f;

                if (m_passingway == PASSINGWAY_BACK)
                {
                    m_passingway = PASSINGWAY_GO;
                }
                else if (m_passingway == PASSINGWAY_GO)
                {
                    m_passingway = PASSINGWAY_BACK;
                }
                else
                {
                    ASSERT(false);
                };

                if (m_type != 2)
                    m_phase = PHASE_STOP;
                else
                    m_phase = PHASE_NONE;
            };

            SetVelocity(&vMoveVelocity);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return CTransferFloorGimmickMove::OnMove(dt);
};


//
// *********************************************************************************
//


CPathTransferFloorGimmickMove::CPathTransferFloorGimmickMove(void)
: m_fPathT(0.0f)
{
    m_szPathName[0] = '\0';
};


CPathTransferFloorGimmickMove::~CPathTransferFloorGimmickMove(void)
{
    ;
};


CPathTransferFloorGimmickMove::RESULT CPathTransferFloorGimmickMove::OnMove(float dt)
{
    switch (m_phase)
    {
    case PHASE_NONE:
        {
            if (m_type != 2)
                m_phase = PHASE_MOVE;
        }
        break;

    case PHASE_STOP:
        {
            m_fTimer += dt;
            if (m_fTimer >= m_fStopTime)
            {
                m_phase = PHASE_MOVE;
                m_fTimer = 0.0f;
            };
        }
        break;

    case PHASE_MOVE:
        {
			m_fTimer += dt;

            if ((m_fTimer < m_fMoveTime) || (m_type == 3))
            {
                int32 PathId = CCameraDataManager::GetPathIDFromName(m_szPathName);
                ASSERT(PathId >= 0);

				m_fPathT = (m_fTimer / m_fMoveTime);
                if ((m_fPathT >= 1.0f) && (m_type == 3))
                {
                    m_fPathT = 0.0f;
                    m_fTimer = 0.0f;
                };

                if (m_passingway == PASSINGWAY_BACK)
                    m_fPathT = (1.0f - m_fPathT);

                CCameraDataManager::GetSplinePos(&m_vPosition, PathId, m_fPathT);
            }
            else
            {
				static const PASSINGWAY aPassingwayInvTbl[] =
				{
					PASSINGWAY_BACK,// PASSINGWAY_GO
					PASSINGWAY_GO,	// PASSINGWAY_BACK
				};

				ASSERT((m_passingway >= 0) && (m_passingway < COUNT_OF(aPassingwayInvTbl)));

                m_fTimer = 0.0f;
				m_passingway = aPassingwayInvTbl[m_passingway];
				m_phase = (m_type != 2 ? PHASE_STOP : PHASE_NONE);
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_NONE;
};


void CPathTransferFloorGimmickMove::SetPathName(const char* pszName)
{
    std::strcpy(m_szPathName, pszName);
    
    int32 PathId = CCameraDataManager::GetPathIDFromName(m_szPathName);
    ASSERT(PathId >= 0);
    CCameraDataManager::GetSplinePos(&m_vPosition, PathId, 0.0f);
};


//
// *********************************************************************************
//


CRotateTransferFloorGimmickMove::CRotateTransferFloorGimmickMove(void)
: m_fRoundTime(0.0f)
, m_vRotAxis(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_bTurnRight(false)
{
    ;
};


CRotateTransferFloorGimmickMove::~CRotateTransferFloorGimmickMove(void)
{
    ;
};


CRotateTransferFloorGimmickMove::RESULT CRotateTransferFloorGimmickMove::OnMove(float dt)
{
    RESULT ret = RESULT_NONE;
    
    if (m_type != 3)
        return ret;

    if (m_fRoundTime <= 0.0f)
        return ret;

    float fStep = MATH_PI2 / m_fRoundTime;

    if (!m_bTurnRight)
        fStep *= -1.0f;

    m_vRotation.y += fStep * dt;
    m_vRotation.y = Math::RadianNormalize(m_vRotation.y);
    
    return ret;
};


void CRotateTransferFloorGimmickMove::SetRoundTime(float t)
{
    m_bTurnRight = (t < 0.0f);
    m_fRoundTime = (t >= 0.0f ? t : -t);
};