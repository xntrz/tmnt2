#include "PropellerGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ bool CPropellerGimmick::m_bHitAttack = false;

/*static*/ const CPropellerGimmick::SUBINFO CPropellerGimmick::m_aSubInfo[] =
{
    { "propeller",  MATH_DEG2RAD(90.0f)    },
    { "propeller",  MATH_DEG2RAD(135.0f)   },
    { "propeller",  MATH_DEG2RAD(180.0f)   },
};


CPropellerGimmick::CPropellerGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_subid(SUBID_NUM)
, m_vPos(Math::VECTOR3_ZERO)
, m_fRotation(0.0f)
, m_fRotVelocity(0.0f)
{
    static_assert(COUNT_OF(m_aSubInfo) == SUBID_NUM, "should equal");

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pInitParam->m_subid >= SUBID_START);
    ASSERT(pInitParam->m_subid < SUBID_NUM);

    m_subid         = SUBID(pInitParam->m_subid);
    m_vPos          = pInitParam->m_vPosition;
    m_fRotation     = 0.0f;
    m_fRotVelocity  = m_aSubInfo[m_subid].fRotVelocity;
    m_bHitAttack    = true;

    //
    //  init disp model
    //
    CModel* pModel = CModelManager::CreateModel(m_aSubInfo[m_subid].pszModelName);
    ASSERT(pModel);

    pModel->SetLightingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init model pos & rot
    //
    m_model.SetPosition(&pInitParam->m_vPosition);

    //
    //  setup model strat
    //
    SetModelStrategy(&m_model);
};


CPropellerGimmick::~CPropellerGimmick(void)
{
    ;
};


void CPropellerGimmick::PostMove(void)
{
    float fDltRot = (CGameProperty::GetElapsedTime() * m_fRotVelocity);
    float fPreRot = m_fRotation;

    m_fRotation += fDltRot;
    m_fRotation = Math::RadianInvClamp(m_fRotation);

    RwV3d vRotation = { 0.0f, 0.0f, m_fRotation };
    m_model.SetRotation(&vRotation);

    if (m_bHitAttack)
    {
        CHitAttackData AttackData;
        AttackData.Cleanup();
        AttackData.SetObject(GetHandle());
        AttackData.SetTarget(CHitAttackData::TARGET_PLAYER);
        AttackData.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        AttackData.SetPower(10);
        AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        AttackData.SetFlyawayParameter(10.0f, 0.0f);
        AttackData.SetMotion("PropellerMotion");
        AttackData.SetShape(CHitAttackData::SHAPE_SPHERE);

        for (int32 i = 0; i < SUBID_NUM; ++i)
        {
            static const RwV3d s_avWingLocalPos[] =
            {
                {     0.0f,  1.8f, 0.0f },
                {  1.5588f, -0.9f, 0.0f },
                { -1.5588f, -0.9f, 0.0f },
            };

            static_assert(COUNT_OF(s_avWingLocalPos) == SUBID_NUM, "should equal");

            //
            //  regist attack for wing
            //
            RwMatrix rotMat;
            RwMatrixSetIdentityMacro(&rotMat);
            Math::Matrix_RotateZ(&rotMat, m_fRotation);

            RwV3d vWing = Math::VECTOR3_ZERO;
            RwV3dTransformPoint(&vWing, &s_avWingLocalPos[i], &rotMat);
            Math::Vec3_Add(&vWing, &vWing, &m_vPos);

            RwV3d vObjPos = { vWing.x, vWing.y, vWing.z + 1.0f };
            AttackData.SetObjectPos(&vObjPos);
            
            RwSphere HitSphereWing = { vWing, 0.5f };
            AttackData.SetSphere(&HitSphereWing);

            CHitAttackManager::RegistAttack(&AttackData);

            //
            //  play wing se
            //
            if (CGimmickManager::IsPlayStarted() && (m_subid == SUBID_PROPELLER_C))
            {
                //
                //  pre rotation
                //
                RwMatrixSetIdentityMacro(&rotMat);
                Math::Matrix_RotateZ(&rotMat, fPreRot);

                RwV3d vWingPre = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&vWingPre, &s_avWingLocalPos[i], &rotMat);

                //
                //  post rotation
                //
                RwMatrixSetIdentityMacro(&rotMat);
                Math::Matrix_RotateZ(&rotMat, m_fRotation);

                RwV3d vWingPost = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&vWingPost, &s_avWingLocalPos[i], &rotMat);

                if ((vWingPost.x >= 0.0f) && (vWingPre.x < 0.0f))
                {
                    //
                    //  TODO test this when SE subsystem ready
                    //
                    CGameSound::PlayObjectSE(this, SDCODE_SE(4220));
                };
            };
        };

        //
        //  regist attack for center
        //
        RwSphere HitSphereCenter = { m_vPos, 0.7f };
        
        AttackData.SetObjectPos(&m_vPos);
        AttackData.SetSphere(&HitSphereCenter);

        CHitAttackManager::RegistAttack(&AttackData);
    };
};


void CPropellerGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        m_bHitAttack = false;    
};