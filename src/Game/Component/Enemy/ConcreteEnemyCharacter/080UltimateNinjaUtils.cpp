#include "080UltimateNinjaUtils.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace UltimateNinjaUtils
{
    CBodyPart::CBodyPart(void)
    : m_attackData()
    , m_fAttackRadius(1.0f)
    {
        ;
    };


    CBodyPart::~CBodyPart(void)
    {
        ;
    };


    void CBodyPart::Initialize(const CGameObject* pGameObj, int32 iAttackNo, float fAttackRadius)
    {
        m_attackData.SetTarget(CHitAttackData::TARGET_PLAYER);
        m_attackData.SetEffect(CHitAttackData::EFFECT_LARGE);
        m_attackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        m_attackData.SetPower(20);
        m_attackData.SetObject(pGameObj->GetHandle());
        m_attackData.SetAttackNo(iAttackNo);

        SetHitTypeSleep();

        m_fAttackRadius = fAttackRadius;
    };


    void CBodyPart::Update(void)
    {
        CHitAttackManager::RegistAttack(&m_attackData);
    };


    void CBodyPart::SetHitTypeSleep(void)
    {
        m_attackData.SetStatus(CHitAttackData::STATUS_SLEEP);
        m_attackData.SetTroubleParameter(2.0f);
    };


    void CBodyPart::SetHitTypeFlyaway(void)
    {
        m_attackData.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        m_attackData.SetFlyawayParameter(0.0f, 0.0f);
    };


    void CBodyPart::SetPosition(const RwV3d* pvecPos)
    {
        RwSphere hitSphere;
        hitSphere.center = *pvecPos;
        hitSphere.radius = m_fAttackRadius;

        m_attackData.SetSphere(&hitSphere);
    };


    //
    // *********************************************************************************
    //

    
    /*static*/ void CSplinePath::GetSplinePos(RwV3d* pvecPos, int32 iPathID, float t)
    {
        if (iPathID == -1)
        {
            *pvecPos = { 40.0f, 1.0f, 0.0f };
            Math::Vec3_Scale(pvecPos, pvecPos, t);

            return;
        };

        CCameraDataManager::GetSplinePos(pvecPos, iPathID, Clamp(t, 0.0f, 1.0f));
    };


    CSplinePath::CSplinePath(void)
    : m_iPathID(-1)
    {
        ;
    };


    CSplinePath::~CSplinePath(void)
    {
        ;
    };


    void CSplinePath::SetPathName(const char* pszPathName)
    {
        m_iPathID = CCameraDataManager::GetPathIDFromName(pszPathName);
    };


    void CSplinePath::GetPos(RwV3d* pvecPos, float t) const
    {
        GetSplinePos(pvecPos, m_iPathID, t);
    };


    void CSplinePath::GetDirection(RwV3d* pvecDir, float t) const
    {
        float fPrevT = (t - 0.01f);
        if (fPrevT < 0.0f)
            fPrevT = 0.0f;

        float fNextT = (t + 0.01f);
        if (fNextT > 1.0f)
            fNextT = 1.0f;

        RwV3d vecPrevPos = Math::VECTOR3_ZERO;
        GetPos(&vecPrevPos, fPrevT);

        RwV3d vecCurrPos = Math::VECTOR3_ZERO;
        GetPos(&vecCurrPos, t);

        RwV3d vecNextPos = Math::VECTOR3_ZERO;
        GetPos(&vecNextPos, fNextT);

        RwV3d vecDir = Math::VECTOR3_ZERO;
        vecDir.x = (vecCurrPos.x - vecPrevPos.x) + (vecNextPos.x - vecCurrPos.x);
        vecDir.y = (vecCurrPos.y - vecPrevPos.y) + (vecNextPos.y - vecCurrPos.y);
        vecDir.z = (vecCurrPos.z - vecPrevPos.z) + (vecNextPos.z - vecCurrPos.z);

        Math::Vec3_Normalize(&vecDir, &vecDir);

        *pvecDir = vecDir;
    };


    float CSplinePath::GetLength(void) const
    {
        RwV3d vecPos1 = Math::VECTOR3_ZERO;
        GetPos(&vecPos1, 0.5f);

        RwV3d vecPos2 = Math::VECTOR3_ZERO;
        GetPos(&vecPos2, 0.51f);

        return (CEnemyUtils::GetDistance(&vecPos1, &vecPos2) / 0.01f);
    };


    //
    // *********************************************************************************
    //

    static const float s_fBodyEffectScale   =  1.5f;
    static const float s_fBodyEffectOffsetY =  0.2f;
    static const float s_fBodyEffectOffsetX =  0.0f;
    static const float s_fBodyEffectOffsetZ = -1.2f;
       
    static const float s_fNeckEffectScale    =  0.4f;
    static const float s_fNeckEffectOffsetX  =  0.0f;
    static const float s_fNeckEffectOffsetY  =  0.2f;
    static const float s_fNeckEffectOffsetZ  = -1.0f;
    static const float s_fNeckEffectDistance =  0.3f;


    CDragonEffect::CDragonEffect(void)
    : m_pBodySmoke(nullptr)
    , m_bodyPositionTracer()
    , m_bodyEffect()
    , m_apNeckSmoke()
    , m_aNeckPositionTracer()
    , m_aNeckMagic()
    {
        m_bodyEffect.Initialize(EFFECTID::ID_DRAGONSMOKE, &m_bodyPositionTracer);
        m_bodyEffect.SetScale(s_fBodyEffectScale);
        m_pBodySmoke = &m_bodyEffect;

        m_aNeckMagic[0].Initialize(MAGICID::ID_ULNIN_ENMAKU, &m_aNeckPositionTracer[0]);
        m_aNeckMagic[0].SetScale(s_fNeckEffectScale);
        m_aNeckMagic[0].SetResetTime(0.7f);
        m_apNeckSmoke[0] = &m_aNeckMagic[0];

        m_aNeckMagic[1].Initialize(MAGICID::ID_ULNIN_ENMAKU, &m_aNeckPositionTracer[1]);
        m_aNeckMagic[1].SetScale(s_fNeckEffectScale);
        m_aNeckMagic[1].SetResetTime(1.1f);
        m_apNeckSmoke[1] = &m_aNeckMagic[1];
    };


    CDragonEffect::~CDragonEffect(void)
    {
        ;
    };


    void CDragonEffect::Update(void)
    {
        m_pBodySmoke->Update();

        for (int32 i = 0; i < COUNT_OF(m_apNeckSmoke); ++i)
            m_apNeckSmoke[i]->Update();
    };


    void CDragonEffect::Start(void)
    {
        m_pBodySmoke->Start();

        for (int32 i = 0; i < COUNT_OF(m_apNeckSmoke); ++i)
            m_apNeckSmoke[i]->Start();
    };


    void CDragonEffect::Stop(void)
    {
        m_pBodySmoke->Stop();

        for (int32 i = 0; i < COUNT_OF(m_apNeckSmoke); ++i)
            m_apNeckSmoke[i]->Stop();
    };


    void CDragonEffect::SetHeadData(const RwV3d* pvecPos, const RwV3d* pvecDir)
    {
        RwV3d vecAxisX = Math::VECTOR3_ZERO;
        RwV3d vecAxisY = Math::VECTOR3_ZERO;
        RwV3d vecAxisZ = Math::VECTOR3_ZERO;

        if (Utils6034::IsParallelVector(pvecDir, &Math::VECTOR3_AXIS_Y))
        {
            vecAxisX = Math::VECTOR3_AXIS_X;

            vecAxisY = Math::VECTOR3_AXIS_Z;
            Math::Vec3_Negate(&vecAxisY, &vecAxisY);
            
            vecAxisZ = Math::VECTOR3_AXIS_Y;
        }
        else
        {
            Utils6034::MakeAxisByDirection(&vecAxisX, &vecAxisY, &vecAxisZ, pvecDir, &Math::VECTOR3_AXIS_Y);
        };

        /*
         *  Set pos to body
         */
        RwV3d vecBodyPos = Math::VECTOR3_ZERO;

        vecBodyPos.x =
            (vecAxisX.x * s_fBodyEffectOffsetX) +
            (vecAxisY.x * s_fBodyEffectOffsetY) +
            (vecAxisZ.x * s_fBodyEffectOffsetZ);

        vecBodyPos.y =
            (vecAxisX.y * s_fBodyEffectOffsetX) +
            (vecAxisY.y * s_fBodyEffectOffsetY) +
            (vecAxisZ.y * s_fBodyEffectOffsetZ);

        vecBodyPos.z =
            (vecAxisX.z * s_fBodyEffectOffsetX) +
            (vecAxisY.z * s_fBodyEffectOffsetY) +
            (vecAxisZ.z * s_fBodyEffectOffsetZ);

        Math::Vec3_Add(&vecBodyPos, &vecBodyPos, pvecPos);

        m_bodyPositionTracer.SetPosition(&vecBodyPos);

        /*
         *  Set pos to neck
         */
        RwV3d vecOffset = Math::VECTOR3_ZERO;

        vecOffset.x =
            (vecAxisX.x * s_fNeckEffectOffsetX) +
            (vecAxisY.x * s_fNeckEffectOffsetY) +
            (vecAxisZ.x * s_fNeckEffectOffsetZ);

        vecOffset.y =
            (vecAxisX.y * s_fNeckEffectOffsetX) +
            (vecAxisY.y * s_fNeckEffectOffsetY) +
            (vecAxisZ.y * s_fNeckEffectOffsetZ);

        vecOffset.z =
            (vecAxisX.z * s_fNeckEffectOffsetX) +
            (vecAxisY.z * s_fNeckEffectOffsetY) +
            (vecAxisZ.z * s_fNeckEffectOffsetZ);

        RwV3d vecDist = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecDist, &vecAxisX, s_fNeckEffectDistance);

        RwV3d vecNeckPos0 = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecNeckPos0, &vecOffset, &vecDist);
        Math::Vec3_Add(&vecNeckPos0, &vecNeckPos0, pvecPos);

        m_aNeckPositionTracer[0].SetPosition(&vecNeckPos0);

        RwV3d vecNeckPos1 = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecNeckPos1, &vecOffset, &vecDist);
        Math::Vec3_Add(&vecNeckPos1, &vecNeckPos1, pvecPos);

        m_aNeckPositionTracer[1].SetPosition(&vecNeckPos1);
    };


    //
    // *********************************************************************************
    //


    static const float s_fDragonSpeed = 9.0f;


    CDragon::CDragon(const CCharacterCompositor* pChrCompositor)
    : m_bActive(false)
    , m_bDraw(false)
    , m_aBodyParts()
    , m_splinePath()
    , m_pModel(CModelManager::CreateModel("snakehead"))
    , m_vecHeadPos(Math::VECTOR3_ZERO)
    , m_vecHeadDir(Math::VECTOR3_ZERO)
    , m_fCurrentPathT(0.0f)
    , m_fUpdatePathT(0.0f)
    , m_fBodyPosDiffTime(0.0f)
    , m_dragonEffect()
    , m_dragonStartTimer(0.8f)
    , m_startPositionTracer()
    , m_startMagic()
    , m_startEffectTimer(1.6f)
    , m_vecPosition(Math::VECTOR3_ZERO)
    , m_pChrCompositor(pChrCompositor)
    , m_fMoveSpeedFrame(CGameProperty::GetElapsedTime() * s_fDragonSpeed)
    {
        for (int32 i = 0; i < COUNT_OF(m_aBodyParts); ++i)
        {
            if (i)
            {
                m_aBodyParts[i].Initialize(pChrCompositor, 1, 1.2f);
            }
            else
            {
                m_aBodyParts[i].Initialize(pChrCompositor, 0, 1.4f);
                m_aBodyParts[i].SetHitTypeFlyaway();
            };
        };

        SetPathID(0);

        m_startMagic.Initialize(MAGICID::ID_ULNIN_ENMAKU, &m_startPositionTracer);
        m_startMagic.SetScale(1.4f);
        m_startMagic.SetResetTime(0.5f);
    };


    CDragon::~CDragon(void)
    {
        if (m_pModel)
        {
            CModelManager::DestroyModel(m_pModel);
            m_pModel = nullptr;
        };
    };


    void CDragon::Update(void)
    {
        if (!m_bActive)
            return;

        m_startMagic.Update();

        m_startEffectTimer.Update();
        if (m_startEffectTimer.IsAlarm())
        {
            m_startEffectTimer.Reset();
            m_startEffectTimer.Pause();

            m_startMagic.Stop();
        };

        m_dragonStartTimer.Update();
        if (m_dragonStartTimer.IsAlarm())
        {
            m_dragonStartTimer.Reset();
            m_dragonStartTimer.Pause();

            ClearData();

            m_dragonEffect.SetHeadData(&m_vecHeadPos, &m_vecHeadDir);
            m_dragonEffect.Start();

            m_bDraw = true;
        };

        if (m_bDraw)
        {
            m_fCurrentPathT += m_fUpdatePathT;

            UpdateData();

            for (int32 i = 0; i < COUNT_OF(m_aBodyParts); ++i)
                m_aBodyParts[i].Update();

            m_dragonEffect.SetHeadData(&m_vecHeadPos, &m_vecHeadDir);
            m_dragonEffect.Update();

            if (m_fCurrentPathT >= 1.0f)
                Stop();
        };
    };


    void CDragon::Draw(void)
    {
        if (IsActive() && m_bDraw)
            m_pModel->Draw();
    };


    void CDragon::Start(void)
    {
        m_bActive = true;
        m_bDraw = false;

        m_pChrCompositor->GetPosition(&m_vecPosition);
        m_vecPosition.y = CWorldMap::GetMapHeight(&m_vecPosition) + 1.0f;

        ClearData();
        UpdateData();

        m_startPositionTracer.SetPosition(&m_vecHeadPos);
        m_startMagic.Start();
        m_startEffectTimer.Start();
        m_dragonStartTimer.Start();

        CGameSound::PlayPositionSE(&m_vecHeadPos, SDCODE_SE(4391));
    };


    void CDragon::Stop(void)
    {
        m_bActive = false;
        m_dragonEffect.Stop();
    };


    void CDragon::SetPathID(int32 iPathID)
    {
        ASSERT(!IsActive());

        static const char* s_apszPathName[] =
        {
            "dragon01",
            "dragon02",
            "dragon03",
        };

        ASSERT(iPathID >= 0);
        ASSERT(iPathID < COUNT_OF(s_apszPathName));

        m_splinePath.SetPathName(s_apszPathName[iPathID]);
    };


    bool CDragon::IsActive(void) const
    {
        return m_bActive;
    };


    void CDragon::GetStartPos(int32 iPathID, RwV3d* pvecPos)
    {
        SetPathID(iPathID);
        GetSplinePos(pvecPos, 0.0f);    
    };


    void CDragon::GetSplinePos(RwV3d* pvecPos, float t)
    {
        m_splinePath.GetPos(pvecPos, t);
        Math::Vec3_Add(pvecPos, pvecPos, &m_vecPosition);
    };


    void CDragon::GetRotationByDirection(RwV3d* pvecRot, const RwV3d* pvecDir)
    {
        RwV3d vecDir = *pvecDir;
        vecDir.y = 0.0f;

        float fLen = Math::Vec3_Length(&vecDir);

        *pvecRot = { -std::atan2(pvecDir->y, fLen), std::atan2(pvecDir->x, pvecDir->z), 0.0f };
    };


    void CDragon::ClearData(void)
    {
        float fPathLen = m_splinePath.GetLength();
            
        m_fCurrentPathT = 0.0f;
		m_fUpdatePathT = (m_fMoveSpeedFrame / fPathLen);
		m_fBodyPosDiffTime = (1.6f / fPathLen);

        UpdateData();
    };


    void CDragon::UpdateData(void)
    {
        GetSplinePos(&m_vecHeadPos, m_fCurrentPathT);

        m_splinePath.GetDirection(&m_vecHeadDir, m_fCurrentPathT);

        RwV3d vecRotation = Math::VECTOR3_ZERO;
        GetRotationByDirection(&vecRotation, &m_vecHeadDir);

        m_pModel->SetPosition(&m_vecHeadPos);
        m_pModel->UpdateFrame();

        m_pModel->SetRotation(&vecRotation);
        m_pModel->UpdateFrame();

        for (int32 i = 0; i < COUNT_OF(m_aBodyParts); ++i)
        {
            float fDiffPathT = (static_cast<float>(i) * m_fBodyPosDiffTime);

            RwV3d vecBodyPos = Math::VECTOR3_ZERO;
            GetSplinePos(&vecBodyPos, m_fCurrentPathT - fDiffPathT);

            m_aBodyParts[i].SetPosition(&vecBodyPos);
        };
    };
};