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

    m_subid         = static_cast<SUBID>(pInitParam->m_subid);
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
    float fPrevRotation = m_fRotation;

    m_fRotation += (CGameProperty::GetElapsedTime() * m_fRotVelocity);
    m_fRotation = Math::RadianCorrection(m_fRotation);

    RwV3d vRotation = { 0.0f, 0.0f, m_fRotation };
    m_model.SetRotation(&vRotation);

    if (m_bHitAttack)
    {
        //
        //  regist attack for each wing
        //
        CHitAttackData hitAttack;
        hitAttack.Cleanup();
        hitAttack.SetObject(GetHandle());
        hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER);
        hitAttack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        hitAttack.SetPower(10);
        hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        hitAttack.SetFlyawayParameter(10.0f, 0.0f);
        hitAttack.SetMotion("PropellerMotion");
        hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);

        const int32 nWingNum = 3;
        for (int32 i = 0; i < nWingNum; ++i)
        {
            static const RwV3d s_avWingLocalPos[] =
            {
                {     0.0f,  1.8f, 0.0f },
                {  1.5588f, -0.9f, 0.0f },
                { -1.5588f, -0.9f, 0.0f },
            };

            static_assert(COUNT_OF(s_avWingLocalPos) == nWingNum, "should equal");

            RwMatrix matRotZ;
            RwMatrixSetIdentityMacro(&matRotZ);
            Math::Matrix_RotateZ(&matRotZ, m_fRotation);

            RwV3d vWing = Math::VECTOR3_ZERO;
            RwV3dTransformPoint(&vWing, &s_avWingLocalPos[i], &matRotZ);
            Math::Vec3_Add(&vWing, &vWing, &m_vPos);

            RwV3d vObjPos = { vWing.x, vWing.y, vWing.z + 1.0f };
            hitAttack.SetObjectPos(&vObjPos);
            
            RwSphere hitSphere = { vWing, 0.5f };
            hitAttack.SetSphere(&hitSphere);

            CHitAttackManager::RegistAttack(&hitAttack);

            //
            //  play wing se
            //
            if (CGimmickManager::IsPlayStarted() && (m_subid == SUBID_PROPELLER_C))
            {
                //
                //  calc local pre rotation
                //
                RwMatrixSetIdentityMacro(&matRotZ);
                Math::Matrix_RotateZ(&matRotZ, fPrevRotation);

                RwV3d vWingPre = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&vWingPre, &s_avWingLocalPos[i], &matRotZ);

                //
                //  calc local post rotation
                //
                RwMatrixSetIdentityMacro(&matRotZ);
                Math::Matrix_RotateZ(&matRotZ, m_fRotation);

                RwV3d vWingPost = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&vWingPost, &s_avWingLocalPos[i], &matRotZ);

                //
                //  Play se
                //
                if ((vWingPost.x >= 0.0f) && (vWingPre.x < 0.0f))
                    CGameSound::PlayObjectSE(this, SDCODE_SE(4220));
            };
        };

        //
        //  regist attack for center
        //
        RwSphere hitSphere = { m_vPos, 0.7f };
        
        hitAttack.SetObjectPos(&m_vPos);
        hitAttack.SetSphere(&hitSphere);

        CHitAttackManager::RegistAttack(&hitAttack);
    };
};


void CPropellerGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        m_bHitAttack = false;    
};