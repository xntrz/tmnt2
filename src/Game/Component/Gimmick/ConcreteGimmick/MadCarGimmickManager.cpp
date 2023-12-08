#include "MadCarGimmickManager.hpp"
#include "MadCarGimmick.hpp"

#include "Game/Component/Gimmick/MoveStrategy/PathMoveGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/Screen.hpp"


struct MADCARMAN_PATHINFO
{
    const char* m_pszPathName;
    float m_fPathLength;
    float m_fPathTimes;
    bool m_bPathDirection;
};


static MADCARMAN_PATHINFO s_aMadCarManPathInfo[] =
{
    { "car23", 0.0f, 42.0f, true    },
    { "car21", 0.0f, 47.0f, true    },
    { "car25", 0.0f, 65.0f, true    },
    { "car01", 0.0f, 27.0f, false   },
    { "car02", 0.0f, 27.0f, false   },
    { "car03", 0.0f, 27.0f, false   },
    { "car04", 0.0f, 27.0f, false   },
    { "car05", 0.0f, 27.0f, false   },
    { "car06", 0.0f, 27.0f, false   },
    { "car11", 0.0f, 30.0f, false   },
    { "car12", 0.0f, 30.0f, false   },
    { "car13", 0.0f, 30.0f, false   },
    { "car14", 0.0f, 30.0f, false   },
    { "car15", 0.0f, 30.0f, false   },
    { "car16", 0.0f, 30.0f, false   },
    { "car17", 0.0f, 30.0f, false   },
    { "car18", 0.0f, 30.0f, false   },
    { "car31", 0.0f, 40.0f, true    },
    { "car32", 0.0f, 45.0f, true    },
    { "car33", 0.0f, 50.0f, true    },
    { "car41", 0.0f, 45.0f, false   },
    { "car42", 0.0f, 50.0f, false   },
    { "car43", 0.0f, 55.0f, false   },
};


/*static*/ float CMadCarGimmickManager::GetPathLength(const char* pszPathName)
{
    ASSERT(pszPathName);

    for (int32 i = 0; i < COUNT_OF(s_aMadCarManPathInfo); ++i)
    {
        if (!std::strcmp(s_aMadCarManPathInfo[i].m_pszPathName, pszPathName))
        {
            ASSERT(s_aMadCarManPathInfo[i].m_fPathLength > 0.0f);
            return s_aMadCarManPathInfo[i].m_fPathLength;
        };
    };

    ASSERT(false);
    return 0.0f;
};


/*static*/ bool CMadCarGimmickManager::IsPathDirection(const char* pszPathName)
{
    ASSERT(pszPathName);

    for (int32 i = 0; i < COUNT_OF(s_aMadCarManPathInfo); ++i)
    {
        if (!std::strcmp(s_aMadCarManPathInfo[i].m_pszPathName, pszPathName))
            return s_aMadCarManPathInfo[i].m_bPathDirection;
    };

    ASSERT(false);
    return false;
};


CMadCarGimmickManager::CMadCarGimmickManager(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_subid(SUBID_START)
, m_eState(STATE_DEMO)
, m_fTimer(1.0f)
, m_anCarCreated()
, m_nRndCnt(0)
, m_nPathStart(0)
, m_nPathEnd(0)
, m_fTimerSE(0.0f)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    ASSERT(pInitParam);

    m_subid = pInitParam->m_subid;
    ASSERT((m_subid >= SUBID_START) && (m_subid < SUBID_NUM));

    const int32 anPathStart[] = { 0, 17, 20 };
    const int32 anPathEnd[] = { 17, 20, 23 };

    static_assert(COUNT_OF(anPathStart) == SUBID_NUM, "update me");
    static_assert(COUNT_OF(anPathEnd) == SUBID_NUM, "update me");

    m_nPathStart = anPathStart[m_subid];
    m_nPathEnd = anPathEnd[m_subid];

    for (int32 i = 0; i < COUNT_OF(m_anCarCreated); ++i)
        m_anCarCreated[i] = 0;

    {
        CPathMoveGimmickMove PathMove;
        for (int32 i = m_nPathStart; i < m_nPathEnd; ++i)
            s_aMadCarManPathInfo[i].m_fPathLength = PathMove.GetPathLength(s_aMadCarManPathInfo[i].m_pszPathName);
    }
};


CMadCarGimmickManager::~CMadCarGimmickManager(void)
{
    for (int32 i = m_nPathStart; i < m_nPathEnd; ++i)
        s_aMadCarManPathInfo[i].m_fPathLength = 0.0f;

    m_nPathStart = 0;
    m_nPathEnd = 0;
};


void CMadCarGimmickManager::PreMove(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();
    m_fTimerSE += CGameProperty::GetElapsedTime();
};


void CMadCarGimmickManager::PostMove(void)
{
    bool bCarCreate = false;
    int32 PathStart = 0;
    int32 PathEnd = 0;
    int32 CarStart = 0;
    int32 CarEnd = 0;
    int32 Chance = Math::RandRange(0, uint32(CScreen::Framerate() * 2.0f));
    
    switch (m_subid)
    {
    case SUBID_BRIDGE_CHASE:
        {
            switch (m_eState)
            {
            case STATE_DEMO:
                {
                    if (m_fTimer > 0.6f)
                    {
                        PathStart = m_nRndCnt;
                        PathEnd = (PathStart + 1);
						CarStart = CMadCarGimmick::SUBID_START_LIGHON;
						CarEnd = CMadCarGimmick::SUBID_END_LIGHTON;
						bCarCreate = true;
						if (++m_nRndCnt > 2)
							m_nRndCnt = 0;
                    };
                }
                break;

            case STATE_BATTLE_1:
                {
                    if (m_fTimer > 0.9f)
                    {
                        PathStart = 3;
                        PathEnd = 9;
                        CarStart = CMadCarGimmick::SUBID_START_LIGHON;
                        CarEnd = CMadCarGimmick::SUBID_END_LIGHTON;
                        bCarCreate = true;
                    };
                }
                break;
                
            case STATE_BATTLE_2:
                {
                    if (m_fTimer > 0.7f)
                    {
                        PathStart = 9;
                        PathEnd = 17;
                        CarStart = CMadCarGimmick::SUBID_START_LIGHON;
                        CarEnd = CMadCarGimmick::SUBID_END_LIGHTON;
                        bCarCreate = true;
                    };
                }
                break;
            };
        }
        break;

    case SUBID_BRIDGE_FIGHT_L:
        {
            if ((m_fTimer >= 1.7f) && (Chance < 12))
            {
                PathStart = 17;
                PathEnd = 20;
                CarStart = CMadCarGimmick::SUBID_START_LIGHTOFF;
                CarEnd = CMadCarGimmick::SUBID_END_LIGHTOFF;
                bCarCreate = true;
            };
        }
        break;

    case SUBID_BRIDGE_FIGHT_R:
        {
            if ((m_fTimer >= 1.5f) && (Chance < 4))
            {
                PathStart = 20;
                PathEnd = 23;
                CarStart = CMadCarGimmick::SUBID_START_LIGHTOFF;
                CarEnd = CMadCarGimmick::SUBID_END_LIGHTOFF;
                bCarCreate = true;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (bCarCreate)
    {
        int32 Subid = Math::RandRange(CarStart, CarEnd);
        int32 CarType = CMadCarGimmick::GetCarType(CMadCarGimmick::SUBID(Subid));
        ASSERT((CarType >= 0) && (CarType < COUNT_OF(m_anCarCreated)));
        ++m_anCarCreated[CarType];

        int32 Path = Math::RandRange(PathStart, PathEnd);

        GIMMICKPARAM::GIMMICK_TERMS_TIME Param;
        std::memset(&Param, 0x00, sizeof(Param));
        Param.m_subid = Subid;
        CGimmickInfo::MakeName(Param.m_szName, GIMMICKID::ID_K_M44N, Subid, m_anCarCreated[CarType]);
        std::strcpy(Param.m_szTargetGimmick, s_aMadCarManPathInfo[Path].m_pszPathName);
        Param.m_fTime = (s_aMadCarManPathInfo[Path].m_fPathTimes / s_aMadCarManPathInfo[Path].m_fPathLength);

        CGimmick* pGimmick = CGimmickManager::Create(GIMMICKID::ID_K_M44N, 0, &Param);
        ASSERT(pGimmick);

        m_fTimer = 0.0f;
    };

    if ((m_subid == SUBID_BRIDGE_FIGHT_L) || (m_eState == STATE_BATTLE_1) || (m_eState == STATE_BATTLE_2))
    {
        if (((m_fTimerSE > 2.6f) && (Chance < 4)) || (m_fTimerSE > 4.0f))
        {
            if ((m_subid == SUBID_BRIDGE_CHASE) && (Chance < 1))
                CGameSound::PlayObjectSE(this, SDCODE_SE(0x1091), m_subid);

            CGameSound::PlayObjectSE(this, SDCODE_SE(0x1092));
            m_fTimerSE = 0.0f;
        };
    };
};


void CMadCarGimmickManager::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE type)
{
    if (m_subid == SUBID_BRIDGE_CHASE)
    {
        if (type == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        {
            switch (m_eState)
            {
            case STATE_DEMO:
                {
                    m_eState = STATE_BATTLE_1;
                    m_fTimer = 0.0f;
                }
                break;

            case STATE_BATTLE_1:
                {
                    m_eState = STATE_BATTLE_1_END;
                    m_fTimer = 0.0f;
                }
                break;

            case STATE_BATTLE_1_END:
                {
                    m_eState = STATE_BATTLE_2;
                    m_fTimer = 0.0f;
                    m_fTimerSE = 0.0f;
                }
                break;

            case STATE_BATTLE_2:
                {
                    m_eState = STATE_BATTLE_2_END;
                    m_fTimer = 0.0f;
                    Release();
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};