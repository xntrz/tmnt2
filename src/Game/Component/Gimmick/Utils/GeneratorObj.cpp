#include "GeneratorObj.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"


CGenEnemyManipTable::MANIP::MANIP(void)
: m_step(STEP_NONE)
, m_nActiveNum(0)
, m_hEnemy(0)
{
    ;
};


bool CGenEnemyManipTable::MANIP::Generate(ENEMYID::VALUE id, int32 nPattern, const RwV3d& rvPos, float fRotY, float fRange, ENEMYTYPES::STATUS status)
{
    ASSERT(m_step == STEP_NONE);

    ENEMYTYPES::CREATEINFO createinfo = {};
    createinfo.m_idEnemy    = id;
    createinfo.m_vPosition  = rvPos;
    createinfo.m_fDirection = fRotY;
    createinfo.m_status     = status;
    createinfo.m_iPattern   = nPattern;
    createinfo.m_fRadius    = fRange;
    
    CEnemy* pEnemy = CEnemy::New(&createinfo);
    if (pEnemy)
    {
        m_hEnemy = pEnemy->GetHandle();
        m_step = STEP_ENEMY_HIDE;
    };

    return true;
};


void CGenEnemyManipTable::MANIP::Show(void)
{
    if (m_step == STEP_ENEMY_HIDE)
    {
        CEnemy* pEnemy = GetEnemy();
        if (pEnemy)
            pEnemy->Start();

        m_step = STEP_ENEMY_APPEAR;
    };
};


void CGenEnemyManipTable::MANIP::Activate(void)
{
    if (IsActive())
    {
        CEnemy* pEnemy = GetEnemy();
        if (pEnemy)
            pEnemy->StartAI();
    };
};


void CGenEnemyManipTable::MANIP::Deactivate(void)
{
    if (IsActive())
    {
        CEnemy* pEnemy = GetEnemy();
        if (pEnemy)
            pEnemy->StopAI();
    };
};


void CGenEnemyManipTable::MANIP::Kill(void)
{
    if (IsActive())
    {
        CEnemy* pEnemy = GetEnemy();
        if (pEnemy)
            pEnemy->Kill();
    };
};


bool CGenEnemyManipTable::MANIP::IsOutOfSight(void)
{
    if (!IsActive())
        return true;

    CEnemy* pEnemy = GetEnemy();
    if (!pEnemy)
        return true;

    CModel* pModel = pEnemy->EnemyCharacter().CharacterCompositor().GetModel();
    if (pModel)
    {
        if (pModel->IsClippingEnable())
            return false;

        RwSphere BSphere = { 0 };
        pModel->CalcCurrentBoundingSphere(&BSphere);

        if (CGimmickUtils::IsPositionVisible(&BSphere.center, BSphere.radius, false))
            return false;
    }
    else
    {
        RwSphere BSphere = { 0 };
        pEnemy->GetPosition(&BSphere.center);

        if (CGimmickUtils::IsPositionVisible(&BSphere.center))
            return false;
    };

    return true;
};


bool CGenEnemyManipTable::MANIP::IsAlive(void)
{
    bool bResult = false;
    
    if (IsActive())
    {
        CEnemy* pEnemy = GetEnemy();
        if (pEnemy)
            bResult = (pEnemy->EnemyCharacter().GetStatus() != ENEMYTYPES::STATUS_DEATH);

        if (!bResult)
            m_step = STEP_ENEMY_DEAD;
    };

    return bResult;
};


CEnemy* CGenEnemyManipTable::MANIP::GetEnemy(void)
{
    return static_cast<CEnemy*>(CGameObjectManager::GetObject(m_hEnemy));
};


bool CGenEnemyManipTable::MANIP::IsActive(void) const
{
    return ((m_step >= STEP_ENEMY_APPEAR) && (m_step < STEP_ENEMY_DEAD));
};


CGenEnemyManipTable::CGenEnemyManipTable(void)
: m_nTableSize(0)
, m_pTableTop(nullptr)
{
    ;
};


CGenEnemyManipTable::~CGenEnemyManipTable(void)
{
    Cleanup();
};


void CGenEnemyManipTable::Setup(int32 nTableSize)
{
    ASSERT(nTableSize > 0);

    m_pTableTop = new MANIP[nTableSize];
    m_nTableSize = nTableSize;
};


void CGenEnemyManipTable::Cleanup(void)
{
    if (m_pTableTop)
    {
        delete[] m_pTableTop;
		m_pTableTop = nullptr;

        m_nTableSize = 0;
    };
};


void CGenEnemyManipTable::KillOutOfSightEnemy(void)
{
    if (!m_pTableTop)
        return;

    for (int32 i = 0; i < m_nTableSize; ++i)
    {
        MANIP* pManip = GetManipulator(i);
        
        if (!pManip->IsAlive())
            continue;
        
        if (!pManip->IsOutOfSight())
            continue;

        pManip->Kill();
    };
};


int32 CGenEnemyManipTable::GetSize(void) const
{
    return m_nTableSize;
};


CGenEnemyManipTable::MANIP* CGenEnemyManipTable::GetManipulator(int32 index) const
{
    ASSERT(index >= 0);
    ASSERT(index < m_nTableSize);
    ASSERT(m_pTableTop);

    return &m_pTableTop[index];
};


//
// *********************************************************************************
//


/*static*/ int32 CEnemySetData::ConvertEnemyPatternFromMGD(int32 nInternalPattern)
{
    ASSERT(nInternalPattern >= 0);
    ASSERT(nInternalPattern < CGimmickData::ENEMY_PATTERN_NUM);

    if ((nInternalPattern >= 0) && (nInternalPattern < CGimmickData::ENEMY_PATTERN_NUM))
        return (nInternalPattern - 1);
    else
        return 0;
};


CEnemySetData::CEnemySetData(void)
: m_nArraySize(0)
, m_aEnemyData()
{
    std::memset(m_aEnemyData, 0, sizeof(m_aEnemyData));
};


void CEnemySetData::Init(const CGimmickData::ENEMYSET_DATA& rSetData)
{
    m_nArraySize = rSetData.m_nEnemyKindNum;

    for (int32 i = 0; i < m_nArraySize; ++i)
    {
        m_aEnemyData[i].m_id = ENEMYID::VALUE(rSetData.m_aEnemyKind[i].m_id);
        m_aEnemyData[i].m_nPattern = ConvertEnemyPatternFromMGD(rSetData.m_aEnemyKind[i].m_pattern);
    };
};


const CEnemySetData::ENEMYDATA& CEnemySetData::GetEnemyData(int32 nIndex)
{
    ASSERT(m_nArraySize > 0);

    nIndex %= m_nArraySize;
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nArraySize);

    return m_aEnemyData[nIndex];
};


//
// *********************************************************************************
//


void CEnemyGeneratorBase::GENERATEPOINT::Init(GIMMICKPARAM::GIMMICK_BASIC* pParam)
{
    m_vPos = pParam->m_vPosition;
    m_fRotY = Math::ACos(pParam->m_quat.w) * 2.0f;
    
    if (pParam->m_quat.y < 0.0f)
        m_fRotY = Math::FNegate(m_fRotY);
};


void CEnemyGeneratorBase::GENERATEPOINT::Clear(void)
{
    m_vPos = Math::VECTOR3_ZERO;
    m_fRotY = 0.0f;
};


CEnemyGeneratorBase::CEnemyGeneratorBase(void)
: m_state(STATE_NONE)
, m_nSuspendCount(0)
, m_nAppearedEnemy(0)
, m_fRemainTime(0.0f)
, m_nTotalEnemy(0)
, m_nAliveEnemyMax(0)
, m_nEnemyPerAppear(0)
, m_fAppearInterval(0.0f)
, m_fActivateDistance(0.0f)
, m_fEnemyRange(0.0f)
, m_bAutoActivate(false)
, m_origin()
, m_enemyManipTable()
{
    m_origin.Clear();
    
    ChangeState(STATE_IDLE);
    Stop();
};


CEnemyGeneratorBase::~CEnemyGeneratorBase(void)
{
    m_enemyManipTable.Cleanup();
};


void CEnemyGeneratorBase::PreRunState(float dt)
{
    switch (m_state)
    {
    case STATE_GENERATE_ENEMY:
        {
            if (m_nAppearedEnemy < m_nTotalEnemy)
                ChangeState(STATE_IDLE);
            else
                ChangeState(STATE_END);
        }
        break;

    case STATE_HIDE_ENEMY:
    case STATE_APPEAR_ENEMY:
        {
            ChangeState(STATE_IDLE);
        }
        break;
   
    default:
        break;
    };
};


void CEnemyGeneratorBase::RunState(float dt)
{
    if (!IsActive())
        return;

    if (m_state == STATE_IDLE)
    {
        int32 numGenerate = calcGenerateNum(m_nEnemyPerAppear);
        if (numGenerate <= 0)
            return;

        if (m_fRemainTime > 0.0f)
            m_fRemainTime -= dt;

        if ((m_fRemainTime <= 0.0f) && generate(numGenerate))
        {
            m_fRemainTime += m_fAppearInterval;
            ChangeState(STATE_GENERATE_ENEMY);
        };
    };    
};


bool CEnemyGeneratorBase::IsActive(void) const
{
    if (!CGimmickManager::IsPlayStarted())
        return false;

    if ((m_state == STATE_END) || (m_state == STATE_NONE))
        return false;

    if (m_nSuspendCount)
        return false;

    if (m_bAutoActivate)
    {
        if (CGimmickUtils::CalcNearestPlayerDistance(&m_origin.m_vPos) > m_fActivateDistance)
            return false;
    };    

    return true;
};


CEnemyGeneratorBase::STATE CEnemyGeneratorBase::Run(float dt)
{
    PreRunState(dt);
    RunState(dt);
    
    return m_state;
};


void CEnemyGeneratorBase::Start(void)
{
    resume();
};


void CEnemyGeneratorBase::Stop(void)
{
    suspend();
};


void CEnemyGeneratorBase::Finish(void)
{
    m_nAppearedEnemy = m_nTotalEnemy;
    
    ChangeState(STATE_END);
};


int32 CEnemyGeneratorBase::CountAliveEnemy(void) const
{
    int32 nResult = 0;
    
    for (int32 i = 0; i < m_nAppearedEnemy; ++i)
    {
        CGenEnemyManipTable::MANIP* pManip = m_enemyManipTable.GetManipulator(i);
        if (pManip->IsAlive())
            ++nResult;
    };

    return nResult;
};


int32 CEnemyGeneratorBase::CountDeadEnemy(void) const
{
    return (m_nAppearedEnemy - CountAliveEnemy());
};


bool CEnemyGeneratorBase::HandleQuery(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    bool bHandled = false;

    switch (pQuery->GetType())
    {
    case GIMMICKTYPES::QUERY_GEN_ENEMY_MAX:
        {
            static_cast<CCounterGimmickQuery*>(pQuery)->m_nCounter += GetTotalEnemyNum();
            bHandled = true;
        };
        break;

    case GIMMICKTYPES::QUERY_GEN_DEAD_ENEMY_NUM:
        {
            static_cast<CCounterGimmickQuery*>(pQuery)->m_nCounter += CountDeadEnemy();
            bHandled = true;
        };
        break;
    };

    return bHandled;
};


bool CEnemyGeneratorBase::HandleEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    bool bHandled = false;

    if (eventtype == GIMMICKTYPES::EVENTTYPE_ENEMYKILL)
    {
        m_enemyManipTable.KillOutOfSightEnemy();
        Finish();
        bHandled = true;
    };

    return bHandled;
};


void CEnemyGeneratorBase::Setup(const INITPARAM& rParam)
{
    m_nTotalEnemy       = rParam.m_nTotalEnemy;
    m_nAliveEnemyMax    = rParam.m_nAliveEnemyMax;
    m_nEnemyPerAppear   = rParam.m_nEnemyPerAppear;
    m_fAppearInterval   = rParam.m_fAppearInterval;
    m_fActivateDistance = rParam.m_fActivateDistance;
    m_fEnemyRange       = rParam.m_fEnemyRange;
    m_bAutoActivate     = rParam.m_bAutoActivate;
    m_origin            = rParam.m_origin;

    ASSERT(m_nTotalEnemy > 0);
    m_enemyManipTable.Setup(m_nTotalEnemy);
};


void CEnemyGeneratorBase::Cleanup(void)
{
    m_enemyManipTable.Cleanup();
};


void CEnemyGeneratorBase::ChangeState(STATE eState)
{
    if (m_state != STATE_END)
        m_state = eState;
};


void CEnemyGeneratorBase::CalcWorldPoint(GENERATEPOINT& rWorldPoint, const GENERATEPOINT rOffsetPoint) const
{
    float s = Math::Sin(m_origin.m_fRotY);
    float c = Math::Cos(m_origin.m_fRotY);

    rWorldPoint.m_vPos.x = m_origin.m_vPos.x + ((rOffsetPoint.m_vPos.x * c) + (rOffsetPoint.m_vPos.z * s));
    rWorldPoint.m_vPos.y = m_origin.m_vPos.y + rOffsetPoint.m_vPos.y;
    rWorldPoint.m_vPos.z = m_origin.m_vPos.z + ((rOffsetPoint.m_vPos.z * c) - (rOffsetPoint.m_vPos.x * s));

    rWorldPoint.m_fRotY = m_origin.m_fRotY + rOffsetPoint.m_fRotY;
};


int32 CEnemyGeneratorBase::generate(int32 nGenerateNum)
{
    int32 numGenerated = 0;
    
    for (int32 i = 0; i < nGenerateNum; ++i)
    {
        CGenEnemyManipTable::MANIP* pManip = m_enemyManipTable.GetManipulator(i);        
        if (!OnGenerate(pManip, m_nAppearedEnemy))
            break;
        
        ++numGenerated;
        ++m_nAppearedEnemy;
    };

    return numGenerated;
};


int32 CEnemyGeneratorBase::calcGenerateNum(int32 nRequestNum) const
{
    if ((m_nAppearedEnemy + nRequestNum) > m_nTotalEnemy)
        nRequestNum = (m_nTotalEnemy - m_nAppearedEnemy);

    int32 nAliveEnemy = CountAliveEnemy();

    if ((nAliveEnemy + nRequestNum) <= m_nAliveEnemyMax)
        return nRequestNum;
    else
        return (m_nAliveEnemyMax - nAliveEnemy);
};


void CEnemyGeneratorBase::suspend(void)
{
    ++m_nSuspendCount;
};


void CEnemyGeneratorBase::resume(void)
{
    ASSERT(m_nSuspendCount > 0);
    --m_nSuspendCount;
};


//
// *********************************************************************************
//


CSettingEnemyGenerator::CSettingEnemyGenerator(void)
: m_nGenerateNum(0)
, m_pGenerateInfoTable(nullptr)
{
    ;
};


CSettingEnemyGenerator::~CSettingEnemyGenerator(void)
{
    if (m_pGenerateInfoTable)
    {
        delete[] m_pGenerateInfoTable;
        m_pGenerateInfoTable = nullptr;
    };
};


bool CSettingEnemyGenerator::OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo)
{
    ASSERT(nGenerateNo >= 0);
    ASSERT(nGenerateNo < m_nGenerateNum);
    
    INFO* pInfo = &m_pGenerateInfoTable[nGenerateNo];
    
    GENERATEPOINT point;
    CalcWorldPoint(point, pInfo->m_point);

    if (pManip->Generate(pInfo->m_id, pInfo->m_nPattern, point.m_vPos, point.m_fRotY, m_fEnemyRange, ENEMYTYPES::STATUS_IDLE))
    {
        pManip->Show();
        pManip->Deactivate();
        return true;
    };

    return false;
};


void CSettingEnemyGenerator::Init(const RwV3d& rvPos, float fRotY, int32 nEnemyNum, float fEnemyRange, bool bAutoActivate)
{
    INITPARAM InitParam = { 0 };
    InitParam.m_bAutoActivate       = bAutoActivate;
    InitParam.m_nTotalEnemy         = nEnemyNum;
    InitParam.m_nAliveEnemyMax      = nEnemyNum;
    InitParam.m_nEnemyPerAppear     = nEnemyNum;
    InitParam.m_fAppearInterval     = 0.0f;
    InitParam.m_fActivateDistance   = CStageInfo::GetEnemyAppearDistance(CGameData::PlayParam().GetStage());
    InitParam.m_fEnemyRange         = fEnemyRange;
    InitParam.m_origin              = GENERATEPOINT(rvPos, fRotY);

    Setup(InitParam);

    initTable(ENEMYID::ID_DUMMY, 0, nEnemyNum);
};


void CSettingEnemyGenerator::Init(GIMMICKPARAM::GIMMICK_ENEMY_PLACE* pParam, int32 nEnemyNum, bool bAutoActivate)
{
    INITPARAM InitParam = { 0 };
    InitParam.m_bAutoActivate       = bAutoActivate;
    InitParam.m_nTotalEnemy         = nEnemyNum;
    InitParam.m_nAliveEnemyMax      = nEnemyNum;
    InitParam.m_nEnemyPerAppear     = nEnemyNum;
    InitParam.m_fAppearInterval     = 0.0f;
    InitParam.m_fActivateDistance   = CStageInfo::GetEnemyAppearDistance(CGameData::PlayParam().GetStage());
    InitParam.m_fEnemyRange         = pParam->m_fEnemyRange;
    InitParam.m_origin.Init(pParam);

    Setup(InitParam);

    initTable(
        ENEMYID::VALUE(pParam->m_enemyID),
        CEnemySetData::ConvertEnemyPatternFromMGD(pParam->m_enemyPattern),
        nEnemyNum
    );
};


void CSettingEnemyGenerator::SetGenerateInfo(int32 nIndex, const RwV3d& rvPos, float fRotY, ENEMYID::VALUE idEnemy, int32 nPattern)
{
    ASSERT(m_pGenerateInfoTable);
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < GetTotalEnemyNum());

    INFO* pInfo = &m_pGenerateInfoTable[nIndex];
    
    pInfo->m_id         = idEnemy;
    pInfo->m_nPattern   = nPattern;
    pInfo->m_point      = GENERATEPOINT(rvPos, fRotY);
};


void CSettingEnemyGenerator::ActivateEnemy(void)
{
    int32 nManipCnt = m_enemyManipTable.GetSize();

    for (int32 i = 0; i < nManipCnt; ++i)
    {
        CGenEnemyManipTable::MANIP* pManip = m_enemyManipTable.GetManipulator(i);
        if (pManip->IsAlive())
            pManip->Activate();
    };
};


void CSettingEnemyGenerator::initTable(ENEMYID::VALUE idEnemy, int32 nPattern, int32 nEnemyNum)
{
    ASSERT(nEnemyNum >= 0);
    
    if (!nEnemyNum)
        return;

    m_pGenerateInfoTable = new INFO[nEnemyNum];
    m_nGenerateNum = nEnemyNum;
    
    switch (nEnemyNum)
    {
    case 1:
        {
            SetGenerateInfo(0, Math::VECTOR3_ZERO, 0.0f, idEnemy, nPattern);
        }
        break;

    case 2:
        {
            RwV3d vLeft  = {  0.5f, 0.0f, 0.0f };
            RwV3d vRight = { -0.5f, 0.0f, 0.0f };

            SetGenerateInfo(0, vLeft,  0.0f, idEnemy, nPattern);
            SetGenerateInfo(1, vRight, 0.0f, idEnemy, nPattern);
        }
        break;

    default:
        {
            RwV3d vLeft  = Math::VECTOR3_ZERO;
            RwV3d vRight = Math::VECTOR3_ZERO;
            int32 nIndex = 0;
            
            if ((nEnemyNum % 2) != 0)
            {
                SetGenerateInfo(0, Math::VECTOR3_ZERO, 0.0f, idEnemy, nPattern);

                vLeft.x =  0.5f;
                vRight.x= -0.5f;
                nIndex  = 1;                
            };

            for (int32 i = nIndex; i < nEnemyNum; i += 2)
            {
                SetGenerateInfo(i,      vLeft, 0.0f, idEnemy, nPattern);
                SetGenerateInfo(i + 1,  vRight, 0.0f, idEnemy, nPattern);

                vLeft.x += 1.0f;
                vRight.x-= 1.0f;
            };
        }
        break;
    };
};


//
// *********************************************************************************
//


CSingleEnemyGenerator::CSingleEnemyGenerator(void)
: m_genpoint()
, m_id(ENEMYID::ID_DUMMY)
, m_nPattern()
{
    m_genpoint.Clear();
};


CSingleEnemyGenerator::~CSingleEnemyGenerator(void)
{
    ;
};


bool CSingleEnemyGenerator::OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo)
{
    GENERATEPOINT point;
    CalcWorldPoint(point, m_genpoint);
    
    if (pManip->Generate(m_id, m_nPattern, point.m_vPos, point.m_fRotY, m_fEnemyRange, ENEMYTYPES::STATUS_HIDE))
    {
        pManip->Show();
        return true;
    };

    return false;
};


void CSingleEnemyGenerator::Init(GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE* pParam, bool bAutoActivate)
{
    INITPARAM InitParam = { 0 };
    InitParam.m_bAutoActivate       = bAutoActivate;
    InitParam.m_nTotalEnemy         = pParam->m_numTotal;
    InitParam.m_nAliveEnemyMax      = pParam->m_numAppearMax;
    InitParam.m_nEnemyPerAppear     = 1;
    InitParam.m_fAppearInterval     = pParam->m_fIntervalTime;
    InitParam.m_fActivateDistance   = CStageInfo::GetEnemyAppearDistance(CGameData::PlayParam().GetStage());
    InitParam.m_fEnemyRange         = pParam->m_fEnemyRange;
    InitParam.m_origin.Init(pParam);

    Setup(InitParam);

    m_id = ENEMYID::VALUE(pParam->m_enemyID);
    m_nPattern = CEnemySetData::ConvertEnemyPatternFromMGD(pParam->m_enemyPattern);
};


//
// *********************************************************************************
//


CMultipleEnemyGenerator::CMultipleEnemyGenerator(void)
: m_genpoint()
, m_enemySetData()
{
    m_genpoint.Clear();
};


CMultipleEnemyGenerator::~CMultipleEnemyGenerator(void)
{
    ;
};


bool CMultipleEnemyGenerator::OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo)
{
    const CEnemySetData::ENEMYDATA& enemyData = m_enemySetData.GetEnemyData(nGenerateNo);

    GENERATEPOINT point;
    CalcWorldPoint(point, m_genpoint);

    if (pManip->Generate(enemyData.m_id, enemyData.m_nPattern, point.m_vPos, point.m_fRotY, m_fEnemyRange, ENEMYTYPES::STATUS_HIDE))
    {
        pManip->Show();
        return true;
    };

    return false;
};


void CMultipleEnemyGenerator::Init(GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE* pParam, bool bAutoActivate)
{
    INITPARAM InitParam = { 0 };
    InitParam.m_bAutoActivate = bAutoActivate;
    InitParam.m_nTotalEnemy = pParam->m_numTotal;
    InitParam.m_nAliveEnemyMax = pParam->m_numAppearanceEnemyMax;
    InitParam.m_nEnemyPerAppear = pParam->m_numAppearanceEnemyOnSameTime;
    InitParam.m_fAppearInterval = pParam->m_fIntervalTime;
    InitParam.m_fActivateDistance = CStageInfo::GetEnemyAppearDistance(CGameData::PlayParam().GetStage());
    InitParam.m_fEnemyRange = pParam->m_fEnemyRange;
    InitParam.m_origin.Init(pParam);

    Setup(InitParam);

    const CGimmickData::ENEMYSET_DATA* pEnemySetData = CGimmickDataManager::GetEnemySetData(pParam->m_enemySetID, CGimmickDataManager::KIND_NORMAL);
    ASSERT(pEnemySetData);

    m_enemySetData.Init(*pEnemySetData);
};


//
// *********************************************************************************
//


CDoorEnemyGenerator::CDoorEnemyGenerator(void)
{
    ;
};


CDoorEnemyGenerator::~CDoorEnemyGenerator(void)
{
    ;
};


void CDoorEnemyGenerator::PreRunState(float dt)
{
    switch (m_state)
    {
    case STATE_GENERATE_ENEMY:
        {
            if (m_nAppearedEnemy < m_nTotalEnemy)
                ChangeState(STATE_HIDE_ENEMY);
            else
                ChangeState(STATE_END);
        }
        break;

    case STATE_HIDE_ENEMY:
    case STATE_APPEAR_ENEMY:
        {
            ChangeState(STATE_IDLE);
        }
        break;

    default:
        break;
    };
};


bool CDoorEnemyGenerator::OnGenerate(CGenEnemyManipTable::MANIP* pManip, int32 nGenerateNo)
{
    const CEnemySetData::ENEMYDATA& enemyData = m_enemySetData.GetEnemyData(nGenerateNo);

    GENERATEPOINT point;
    CalcWorldPoint(point, m_genpoint);

    return pManip->Generate(enemyData.m_id, enemyData.m_nPattern, point.m_vPos, point.m_fRotY, m_fEnemyRange, ENEMYTYPES::STATUS_HIDE);
};


void CDoorEnemyGenerator::ShowEnemy(void)
{
    int32 nManipCnt = m_enemyManipTable.GetSize();
    
    for (int32 i = 0; i < nManipCnt; ++i)
    {
        CGenEnemyManipTable::MANIP* pManip = m_enemyManipTable.GetManipulator(i);
        if (pManip->IsAlive())
            pManip->Show();
    };
};